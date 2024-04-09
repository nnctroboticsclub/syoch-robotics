#pragma once

#include <cstring>

#include <queue>
#include <memory>
#include <string>
#include <thread>
#include <optional>
#include <stdexcept>
#include <functional>

#include <unordered_map>

#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

namespace simulation {

class NotImplemenetedError : public std::exception {
  char *message;

 public:
  NotImplemenetedError(const char *message) {
    this->message = new char[strlen(message) + 20];
    memset(this->message, 0, strlen(message) + 20);
    strcpy(this->message, "Not implemented: ");
    strcat(this->message, message);
  }
  ~NotImplemenetedError() { delete[] message; }
  const char *what() const noexcept override { return message; }
};

class ConnectionClosedError : public std::exception {
  char *message;

 public:
  ConnectionClosedError() {}
  const char *what() const noexcept override { return "Connection Closed"; }
};

struct Message {
  std::string tag;
  int id;
  std::string method;
  std::vector<std::string> args;

  Message(std::string tag, int id, std::string method,
          std::vector<std::string> args = {})
      : tag(tag), id(id), method(method), args(args) {}

  static std::pair<std::string, Message> ParseString(const std::string &data) {
    auto it = data.begin();
    while (*it == '\n' || *it == ' ') {
      it++;
    }

    // Parse tag
    std::string tag = "";
    while (*it != '@') {
      tag.push_back(*it);
      it++;
    }
    it++;  // Skip '@'

    // Parse id
    int id = 0;
    while (*it != ';') {
      id = id * 10 + (*it - '0');
      it++;
    }
    it++;  // Skip ';'

    // Parse method
    std::string method;
    while (*it != ';') {
      method.push_back(*it);
      it++;
    }
    it++;  // Skip ';'

    // Parse args
    std::vector<std::string> args;
    while (*it != '\n' && it != data.end()) {
      std::string arg;
      while (*it != ';' && it != data.end()) {
        if (*it == '\\') {
          it++;
        }
        arg.push_back(*it);
        it++;
      }
      args.push_back(arg);
      if (it != data.end()) {
        it++;  // Skip ';'
      }
    }

    while (*it == '\n' || *it == ' ') {
      it++;
    }

    std::string rest(it, data.end());
    Message msg(tag, id, method, args);

    return {rest, msg};
  }

  std::string ToString() const {
    std::string data;
    data += tag + "@";
    data += std::to_string(id) + ";";
    data += method + ";";
    for (const auto &arg : args) {
      for (const auto &c : arg) {
        if (c == ';' || c == '\\') {
          data += '\\';
        }
        data += c;
      }
      data += ";";
    }
    data += "\n";
    return data;
  }
};

class RawDriver {
  const int fd;
  std::string rx_string_buffer;
  std::vector<std::function<void(Message)>> rx_callbacks;

  std::thread thread;

  void Send(const char *data, size_t size) {
    if (send(fd, data, size, 0) < 0) {
      printf("Error: %s\n", strerror(errno));
      throw std::runtime_error("[RawDriver] Failed to send data");
    }
  }

  void ReceiveNewMessage() {
    std::string buffer;
    buffer.resize(1024);

    int size = recv(fd, &buffer[0], buffer.size(), 0);
    if (size < 0) {
      throw std::runtime_error("[RawDriver] Failed to receive data");
    }

    if (size == 0) {
      throw ConnectionClosedError();
    }

    buffer.resize(size);
    rx_string_buffer += buffer;

    auto [rest, msg] = Message::ParseString(rx_string_buffer);
    rx_string_buffer = rest;

    for (const auto &callback : rx_callbacks) {
      callback(msg);
    }
  }

  void Thread() {
    while (true) {
      // printf("[RawDriver] Waiting message\n");
      try {
        ReceiveNewMessage();
      } catch (const ConnectionClosedError &e) {
        printf("[RawDriver] Connection closed\n");
        break;
      }
    }
  }

 public:
  RawDriver(int fd) : fd(fd), thread(&RawDriver::Thread, this) {
    printf("[RawDriver] Created.");
  }

  void Send(const Message &msg) {
    std::string data = msg.ToString();
    Send(data.c_str(), data.size());
  }

  void OnReceive(const std::function<void(const Message &)> &callback) {
    rx_callbacks.push_back(callback);
  }
};
class Driver {
  template <typename T>
  using MethodTable = std::unordered_map<std::string, T>;

  template <typename T>
  using MessageTable = std::unordered_map<std::string, MethodTable<T>>;

  std::string rx_string_buffer;
  MessageTable<std::queue<Message>> rx_message_queue;
  MessageTable<std::vector<std::function<void(Message)>>> message_callbacks;
  RawDriver raw_driver;

  bool HasReceivedMessage(const Message &msg) {
    if (rx_message_queue.find(msg.tag) == rx_message_queue.end()) {
      return false;
    }

    if (rx_message_queue[msg.tag].find(msg.method) ==
        rx_message_queue[msg.tag].end()) {
      return false;
    }

    return !rx_message_queue[msg.tag][msg.method].empty();
  }
  std::optional<Message> GetReceivedMessageWithId(const Message &msg) {
    if (rx_message_queue.find(msg.tag) == rx_message_queue.end()) {
      return std::nullopt;
    }

    if (rx_message_queue[msg.tag].find(msg.method) ==
        rx_message_queue[msg.tag].end()) {
      return std::nullopt;
    }

    auto queue = rx_message_queue[msg.tag][msg.method];

    for (size_t i = 0; i < queue.size(); i++) {
      if (queue.front().id == msg.id) {
        auto msg = queue.front();
        queue.pop();
        return msg;
      }
      queue.push(queue.front());
      queue.pop();
    }

    return std::nullopt;
  }
  void BufferReceivedMessage(const Message &msg) {
    if (rx_message_queue.find(msg.tag) == rx_message_queue.end()) {
      rx_message_queue[msg.tag] = {};
    }

    if (rx_message_queue[msg.tag].find(msg.method) ==
        rx_message_queue[msg.tag].end()) {
      rx_message_queue[msg.tag][msg.method] = {};
    }

    rx_message_queue[msg.tag][msg.method].push(msg);
  }

  bool HasMessageCallback(const Message &msg) {
    if (message_callbacks.find(msg.tag) == message_callbacks.end()) {
      return false;
    }

    if (message_callbacks[msg.tag].find(msg.method) ==
        message_callbacks[msg.tag].end()) {
      return false;
    }

    return true;
  }

  void ProcessReceivedMessage(Message message) {
    if (HasMessageCallback(message)) {
      for (const auto &callback :
           message_callbacks[message.tag][message.method]) {
        callback(message);
      }
    } else {
      printf("[Driver] No callback for message: %s\n",
             message.ToString().c_str());
      BufferReceivedMessage(message);
    }
  }

  void ProcessBufferedMessageAsPossible(std::string tag, std::string method) {
    Message dummy_msg(tag, -1, method);
    if (!HasMessageCallback(dummy_msg)) {
      return;
    }

    while (HasReceivedMessage(dummy_msg)) {
      auto msg = rx_message_queue[tag][method].front();
      rx_message_queue[tag][method].pop();
      ProcessReceivedMessage(msg);
    }
  }

 public:
  Driver(int fd) : raw_driver(fd) {
    printf("[Driver] created.");

    raw_driver.OnReceive(
        [this](const Message &msg) { ProcessReceivedMessage(msg); });
  }

  void Send(const Message &msg) { raw_driver.Send(msg); }

  void OnReceive(const std::string &tag, const std::string &method,
                 const std::function<void(const Message &)> &callback) {
    if (message_callbacks.find(tag) == message_callbacks.end()) {
      message_callbacks[tag] = {};
    }

    if (message_callbacks[tag].find(method) == message_callbacks[tag].end()) {
      message_callbacks[tag][method] = {};
    }

    message_callbacks[tag][method].push_back(callback);

    ProcessBufferedMessageAsPossible(tag, method);
  }

  Message GetMessage(const std::string &tag, int id, const std::string &method,
                     std::chrono::duration<float> timeout) {
    auto start = std::chrono::system_clock::now();
    while (true) {
      if (GetReceivedMessageWithId(Message(tag, id, method))) {
        auto msg = rx_message_queue[tag][method].front();
        rx_message_queue[tag][method].pop();
        return msg;
      }

      auto now = std::chrono::system_clock::now();
      if (now - start > timeout) {
        throw std::runtime_error("[Driver] Timeout");
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
};

class Client {
 public:
  std::shared_ptr<Driver> Connect(int device_id) {
    printf("[Driver::Client] Connecting...\n");
    int fd;
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
      throw std::runtime_error("[Driver] Failed to create socket");
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "emu/emu.sock");

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      throw std::runtime_error("[Driver] Failed to connect to socket");
    }

    auto driver = std::make_shared<Driver>(fd);
    driver->Send(Message("Sys", device_id, "init"));

    return driver;
  }
};

class Server {
  int server_fd_ = -1;
  std::unordered_map<int, std::shared_ptr<Driver>> drivers;
  std::vector<std::function<void(std::shared_ptr<Driver>)>>
      on_connect_callbacks;

  std::thread thread;

  void Listen() {
    printf("[Driver] Listening...\n");
    server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
      throw std::runtime_error("[Driver] Failed to create socket");
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "emu/emu.sock");

    unlink(addr.sun_path);
    if (bind(server_fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      throw std::runtime_error("[Driver] Failed to bind socket");
    }

    if (listen(server_fd_, 1) < 0) {
      throw std::runtime_error("[Driver] Failed to listen socket");
    }
  }

  void Accept() {
    if (server_fd_ <= 0) {
      throw std::runtime_error("[Driver] Server not listening");
    }

    printf("[Driver] Accepting...\n");
    int fd;
    struct sockaddr_un addr;
    socklen_t addr_len = sizeof(addr);

    fd = accept(server_fd_, (struct sockaddr *)&addr, &addr_len);
    if (fd < 0) {
      throw std::runtime_error("[Driver] Failed to accept socket");
    }

    drivers[fd] = std::make_shared<Driver>(fd);

    for (const auto &callback : on_connect_callbacks) {
      callback(drivers[fd]);
    }

    printf("[Driver] Accepted\n");
  }

  void Thread() {
    Listen();

    while (1) {
      Accept();
    }
  }

 public:
  Server() { thread = std::thread(&Server::Thread, this); }

  void OnConnect(const std::function<void(std::shared_ptr<Driver>)> &callback) {
    on_connect_callbacks.push_back(callback);
  }
};

}  // namespace simulation