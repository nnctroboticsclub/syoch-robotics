#!/usr/bin/env python3

# import module
import struct
import time

# PCAP file settings
# https://wiki.wireshark.org/Development/LibpcapFileFormat
MAGIC_NUMBER = 0xA1B2C3D4
VERSION_MAJOR = 2
VERSION_MINOR = 4
THISZONE = 0  # GMT to local correction
SIGFIGS = 0  # Accuracy of timestamps
SNAPLEN = 65535  # Max packet length
NETWORK = 150  # Data link type. 147 = DLT_USER0, 148 = DLT_USER1, etc.
PCAP_GLOBAL_HEADER_FORMAT = "! I H H i I I I"  # @ = System Endian
PCAP_PACKET_HEADER_FORMAT = "! I I I I"


class Pcap:
    def __init__(self, filename, network=NETWORK):
        self.pcap_file = open(filename, "wb")
        self.pcap_file.write(
            struct.pack(
                PCAP_GLOBAL_HEADER_FORMAT,  #
                MAGIC_NUMBER,
                VERSION_MAJOR,
                VERSION_MINOR,
                THISZONE,
                SIGFIGS,
                SNAPLEN,
                network,
            )
        )

    def add_packet(self, data):
        ts_sec, ts_usec = map(int, str(time.time()).split("."))
        length = len(data)

        payload = struct.pack(
            PCAP_PACKET_HEADER_FORMAT,
            ts_sec,
            ts_usec,
            length,
            length,
        )
        payload += data
        self.pcap_file.write(payload)

        self.pcap_file.flush()
