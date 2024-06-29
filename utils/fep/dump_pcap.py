import asyncio
import struct

from utils.fep.fep_client import FepClient
from utils.network.pcap import Pcap

BFEP_PROTOCOL = "! b b"


async def dump_pcap(socket: str, pcap_path: str):
    reader, writer = await asyncio.open_unix_connection(socket)
    pcap_output = Pcap(pcap_path)

    client = FepClient(writer, reader)

    @client.on_packet
    def on_packet(addr: int, data: bytes):
        print(f"Received packet from {addr:02x} with {len(data)} bytes")
        bfep = struct.pack(BFEP_PROTOCOL, addr, len(data)) + data
        pcap_output.add_packet(bfep)

    print("FEP Dump To Pcap Service Started")


if __name__ == "__main__":
    import os

    sock = os.environ.get("FEP_SOCKET", "/tmp/fep.sock")
    pcap = os.environ.get("FEP_PCAP", "output.pcap")

    asyncio.run(dump_pcap(sock, pcap))
