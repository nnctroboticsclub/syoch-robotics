#pragma once

#include <cstdint>
#include <cstring>

#include <robotics/network/stream.hpp>
#include <robotics/utils/no_mutex_lifo.hpp>
#include "logger/logger.hpp"

#include <robotics/system/thread.hpp>

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

  // avoid bidirectional loop
  AddrRecords addresses_imcoming_;
  AddrRecords addresses_hop_candidate_;

  // thread
  robotics::system::Thread thread;

  // connection state
  robotics::network::Stream<uint8_t, uint8_t>& upper_stream_;
  uint8_t next_hop_ = 0;

  // address
  uint8_t broadcast_addr_;
  uint8_t self_addr_;

  void FindHopTo() {
    logger.Info("Find New Hop, Avoid list:");
    logger.Hex(robotics::logger::core::Level::kInfo,
               addresses_imcoming_.records, addresses_imcoming_.recorded);

    SendEx(broadcast_addr_, this->addresses_imcoming_.records,
           this->addresses_imcoming_.recorded, kFlagsDetectDevices, self_addr_);
  }

  void UpdateNextHop() {
    if (addresses_imcoming_.Recorded(next_hop_)) {
      next_hop_ = 0;
    } else if (next_hop_ != 0) {
      return;
    }

    if (addresses_hop_candidate_.recorded == 0) {
      FindHopTo();
      return;
    }

    next_hop_ = addresses_hop_candidate_.records[0];
    addresses_hop_candidate_.Remove(next_hop_);

    logger.Info("Using Hop: %d (from cache)", next_hop_);
  }

  void ProcessAction(Action& action) {
    switch (action.type) {
      case Action::Type::kFindNewHopTo: {
        if (next_hop_ != 0) {
          break;
        }

        logger.Info("Find New Hop, Avoid list:");
        logger.Hex(robotics::logger::core::Level::kInfo,
                   addresses_imcoming_.records, addresses_imcoming_.recorded);

        SendEx(broadcast_addr_, addresses_imcoming_.records,
               addresses_imcoming_.recorded, kFlagsDetectDevices, self_addr_);

        break;
      }

      case Action::Type::kAdvertiseSelf: {
        SendEx(broadcast_addr_, nullptr, 0, kFlagsNewDevice, self_addr_);
        break;
      }
    }
  }

  void DoSend(Packet* packet) {
    static char buffer[32];

    if (packet->goal != broadcast_addr_)
      addresses_hop_candidate_.Add(packet->goal);

    auto send_to =
        addresses_imcoming_.Recorded(packet->goal) ? next_hop_ : packet->goal;

    if (packet->goal == broadcast_addr_) {
      packet->goal = 0;
    }

    buffer[0] = (packet->from << 4) | (packet->goal & 0xF);
    buffer[1] = packet->flags;

    if (packet->data != nullptr) {
      std::memcpy(buffer + 2, packet->data, packet->size);
    }

    tx_logger.Info("Send %3d --> %3d ==> %3d f%#02x (%3d Bytes)", packet->from,
                   send_to, packet->goal, packet->flags, packet->size);
    tx_logger.Hex(robotics::logger::core::Level::kInfo, packet->data,
                  packet->size);

    upper_stream_.Send(send_to, (uint8_t*)buffer, packet->size + 2);
  }

  void Thread() {
    using namespace std::chrono_literals;
    int ticks_no_hops = 0;
    while (1) {
      ticks_no_hops = next_hop_ == 0 ? ticks_no_hops + 1 : 0;
      if (ticks_no_hops == 10) {
        logger.Debug("No Hop for 10 ticks, Find New Hop");
        ticks_no_hops = 0;

        auto action = Action::FindNewHopTo();
        ProcessAction(action);
      }
      robotics::system::SleepFor(100ms);
    }
  }

  bool ProcessFlags(uint8_t from, uint8_t flags, uint8_t* data, uint32_t size) {
    if (flags == kFlagsDetectDevices) {
      for (uint i = 0; i < size; i++) {
        if (data[i] == self_addr_)
          return true;
      }

      auto action = Action::AdvertiseSelf(from);
      ProcessAction(action);
    } else if (flags == kFlagsNewDevice) {
      if (addresses_imcoming_.Recorded(from))
        return true;
      addresses_hop_candidate_.Add(from);
      UpdateNextHop();
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
    addresses_imcoming_ = {0, 0};

    upper_stream_.OnReceive([this](uint8_t from, uint8_t* data, uint32_t size) {
      auto ptr = data;

      auto address_pair = *ptr++;
      auto flag = *ptr++;

      auto original_from = address_pair >> 4;
      auto goal = address_pair & 0x0F;

      auto payload = ptr;
      auto payload_size = size - 2;

      rx_logger.Info("Recv %3d ==> %3d --> %3d f%#02x (%3d Bytes)",
                     original_from, self_addr_, goal, flag, payload_size);
      rx_logger.Hex(robotics::logger::core::Level::kInfo, payload,
                    payload_size);

      if (goal != 0) {
        addresses_imcoming_.Add(from);
        if (from == next_hop_) {
          next_hop_ = 0;
          auto action = Action::FindNewHopTo();
          ProcessAction(action);
        }
      }

      if (goal != 0 && goal != self_addr_) {
        SendEx(goal, payload, payload_size, flag, original_from);
        return;
      }

      if (ProcessFlags(from, flag, payload, payload_size)) {
        return;
      }

      DispatchOnReceive(original_from, payload, payload_size);
    });

    thread.SetStackSize(4096);
  }

  void Start() {
    auto action = Action::FindNewHopTo();
    ProcessAction(action);

    thread.Start([this]() { Thread(); });
  }

  void SendEx(uint8_t to, uint8_t* data, uint32_t length, uint8_t flags,
              uint8_t from) {
    Packet packet;
    packet.from = from;
    packet.goal = to;
    packet.flags = flags;
    packet.size = length;
    std::memcpy(packet.data, data, length);

    DoSend(&packet);
  }
  void Send(uint8_t to, uint8_t* data, uint32_t length) override {
    SendEx(to, data, length, 0, self_addr_);
  }
};

}  // namespace robotics::network::froute