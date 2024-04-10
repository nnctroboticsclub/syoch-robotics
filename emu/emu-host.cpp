#include <chrono>
#include <iostream>

#include "simulation.hpp"

using namespace std::chrono_literals;

class SPITransferable {
  virtual void Transfer(std::vector<uint8_t>& vec) = 0;
};

class SPIBus {
  std::unordered_map<int, std::shared_ptr<SPITransferable>> devices;

 public:
  void AddDevice(int master_side_nss, std::shared_ptr<SPITransferable> device) {
    devices[master_side_nss] = device;
  }
};

class DeviceContext {};

class Host {
  simulation::Server server;

 public:
  Host() {
    server.OnConnect([](std::shared_ptr<simulation::Driver> driver) {
      driver->OnReceive("CCa", "init", [](const simulation::Message& msg) {
        // Args: (rx, tx, frequency)
      });
      driver->OnReceive("CCa", "write", [](const simulation::Message& msg) {

      });
      driver->OnReceive("CCa", "read", [](const simulation::Message& msg) {
        // Args: ()
      });
      driver->OnReceive("OI2", "init", [](const simulation::Message& msg) {
        // Args: (sda, scl)
      });
      driver->OnReceive("OPw", "init", [](const simulation::Message& msg) {
        // Args: ()
        printf("OPw init\n");
      });

      driver->OnReceive("OPw", "period", [](const simulation::Message& msg) {
        // Args: (time_us)
        printf("OPw period -> %d: %d\n", msg.id, std::stoi(msg.args[0]));
      });

      driver->OnReceive("OPw", "pulsewidth",
                        [](const simulation::Message& msg) {
                          // Args: (value)
                          printf("OPw pulsewidth -> %d: %d\n", msg.id,
                                 std::stoi(msg.args[0]));
                        });
      driver->OnReceive("ODi", "init", [](const simulation::Message& msg) {
        // Args: ()
      });
      driver->OnReceive("ODi", "write", [](const simulation::Message& msg) {
        // Args: (bool)
      });
      driver->OnReceive("OI2", "write", [](const simulation::Message& msg) {
        // Args: (addr, repeated, data...)
      });
      driver->OnReceive("OI2", "read", [](const simulation::Message& msg) {
        // Args: (addr, len, repeated)
      });
    });
  }
};

int main(int argc, char const* argv[]) {
  Host host;

  while (1) {
    std::this_thread::sleep_for(1s);
  }

  return 0;
}