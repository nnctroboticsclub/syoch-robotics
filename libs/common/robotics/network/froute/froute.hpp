#pragma once

#include <cstdint>
#include <cstring>

#include "../stream.hpp"
#include "../../logger/logger.hpp"
#include "../../platform/thread.hpp"
#include "../../utils/no_mutex_lifo.hpp"

#include "actions.hpp"
#include "addr_records.hpp"
#include "packet.hpp"

namespace robotics::network::froute {

namespace {
robotics::logger::Logger logger{"frt.nw", "\x1b[1;36mFRt\x1b[m"};
robotics::logger::Logger rx_logger{"rx.frt.nw",
                                   "\x1b[1;36mFRt - \x1b[34mRX\x1b[m"};
robotics::logger::Logger tx_logger{"tx.frt.nw",
                                   "\x1b[1;36mFRt - \x1b[32mTX\x1b[m"};
}  // namespace

class ProtoFRoute : public robotics::network::Stream<uint8_t, uint8_t> {
  const uint8_t kFlagsDetectDevices = 0x01;
  const uint8_t kFlagsNewDevice = 0x02;
  const uint8_t kFlagsPacketDied = 0x03;

  // queue
  robotics::utils::NoMutexLIFO<Action, 4> remaining_actions_;
  robotics::utils::NoMutexLIFO<Packet, 4> remaining_packets_;

  // avoid bidirectional loop
  AddrRecords incoming_data_from_;

  // thread
  robotics::system::Thread thread;

  // connection state
  robotics::network::Stream<uint8_t, uint8_t>& upper_stream_;
  uint8_t next_hop_ = 0;

  // address
  uint8_t broadcast_addr_;
  uint8_t self_addr_;

  void ProcessAction(Action& action) {
    switch (action.type) {
      case Action::Type::kFindNewHopTo: {
        if (next_hop_ != 0) {
          return;
        }

        SendEx(broadcast_addr_, nullptr, 0, kFlagsDetectDevices, self_addr_);

        return;
      }

      case Action::Type::kReportDiedPacket: {
        SendEx(action.report_to, &self_addr_, 1, kFlagsPacketDied, self_addr_);
        return;
      }

      case Action::Type::kAdvertiseSelf: {
        SendEx(action.report_to, nullptr, 0, kFlagsNewDevice, self_addr_);
        return;
      }
    }
  }

  void DoSend(Packet* packet) {
    static char buffer[32];
    buffer[0] = packet->life;
    buffer[1] = packet->from;
    buffer[2] = packet->goal;
    buffer[3] = packet->flags;
    if (packet->data != nullptr) {
      std::memcpy(buffer + 4, packet->data, packet->size);
    }

    tx_logger.Info("Send Data = %p (%d B) --> %d (l%d %d --> %d f%#02x)",
                   packet->data, packet->size, packet->goal, packet->life,
                   packet->from, packet->goal, packet->flags);
    tx_logger.Hex(robotics::logger::core::Level::kInfo, packet->data,
                  packet->size);

    if (packet->using_hop == 0 && next_hop_ != 0) {
      packet->using_hop = next_hop_;
    }

    upper_stream_.Send(packet->using_hop, (uint8_t*)buffer, packet->size + 4);
  }

  void Thread() {
    using namespace std::chrono_literals;
    int ticks_no_hops = 0;
    while (1) {
      ticks_no_hops = next_hop_ == 0 ? ticks_no_hops + 1 : 0;
      if (ticks_no_hops == 100) {
        logger.Debug("No Hop for 100 ticks, Find New Hop");
        remaining_actions_.Push(Action::FindNewHopTo());
        ticks_no_hops = 0;
      }

      if (remaining_actions_.Empty() && remaining_packets_.Empty()) {
        robotics::system::SleepFor(10ms);
        continue;
      }

      while (!remaining_actions_.Empty()) {
        auto action = remaining_actions_.Pop();
        ProcessAction(action);
      }

      for (size_t i = 0; i < remaining_packets_.Size(); i++) {
        auto packet = remaining_packets_.Pop();
        DoSend(&packet);
      }
    }
  }

  bool ProcessFlags(uint8_t from, uint8_t flags, uint8_t* data, uint32_t size) {
    if (flags == kFlagsDetectDevices) {
      for (uint i = 0; i < size; i++) {
        if (data[i] == self_addr_) return true;
      }

      remaining_actions_.Push(Action::AdvertiseSelf(from));
    } else if (flags == kFlagsNewDevice) {
      if (incoming_data_from_.Recorded(from)) return true;
      logger.Info("Using Hop: %d", from);
      next_hop_ = from;
    } else if (flags == kFlagsPacketDied) {
      logger.Info("Packet Died from %d", from);
      if (from == next_hop_) {
        next_hop_ = 0;
        remaining_actions_.Push(Action::FindNewHopTo());
      }
    } else {
      return false;
    }

    return true;
  }

 public:
  ProtoFRoute(robotics::network::Stream<uint8_t, uint8_t>& upper_stream,
              uint8_t broadcast_addr, uint8_t self_addr)
      : upper_stream_(upper_stream),
        broadcast_addr_(broadcast_addr),
        self_addr_(self_addr) {
    incoming_data_from_ = {0, 0};
    remaining_actions_.Push(Action::FindNewHopTo());

    upper_stream_.OnReceive([this](uint8_t from, uint8_t* data, uint32_t size) {
      auto ptr = data;

      auto life = *ptr++;
      auto original_from = *ptr++;
      auto goal = *ptr++;
      auto flags = *ptr++;

      auto payload = ptr;
      auto payload_size = size - 4;

      rx_logger.Info("Recv Data = %p (%d B) (l%d %d --> %d f%#02x)", payload,
                     payload_size, life, original_from, goal, flags);
      rx_logger.Hex(robotics::logger::core::Level::kInfo, payload,
                    payload_size);

      if (ProcessFlags(from, flags, payload, payload_size)) {
        return;
      }

      incoming_data_from_.Add(from);

      if (life == 0) {
        remaining_actions_.Push(Action::ReportDiedPacket(from));
        return;
      }

      if (goal == self_addr_) {
        DispatchOnReceive(original_from, payload, payload_size);
      } else {
        SendEx(goal, payload, payload_size, flags, original_from);
      }

      if (from == next_hop_) {
        next_hop_ = 0;
        remaining_actions_.Push({Action::Type::kFindNewHopTo, 0});
      }
    });

    thread.SetStackSize(1024);
  }

  void Start() {
    thread.Start([this]() { Thread(); });
  }

  void SendEx(uint8_t to, uint8_t* data, uint32_t length, uint8_t flags,
              uint8_t from) {
    Packet packet;
    packet.using_hop = to == broadcast_addr_ ? broadcast_addr_
                       : next_hop_ == 0      ? to
                                             : next_hop_;
    packet.life = 0x80;
    packet.from = from;
    packet.goal = to;
    packet.flags = flags;
    packet.size = length;
    std::memcpy(packet.data, data, length);

    remaining_packets_.Push(packet);
  }
  void Send(uint8_t to, uint8_t* data, uint32_t length) override {
    SendEx(to, data, length, 0, self_addr_);
  }
};

}  // namespace robotics::network::froute