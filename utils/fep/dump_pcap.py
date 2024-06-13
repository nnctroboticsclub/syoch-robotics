import asyncio
import struct

from utils.fep.fep_client import FepClient
from utils.network.pcap import Pcap

BFEP_PROTOCOL = "! b b"


async def dump_pcap(remote: str, pcap_path: str):
    print("Connecting...")
    reader, writer = await asyncio.open_connection(remote, 31337)

    print("...")
    pcap_output = Pcap(pcap_path)

    client = FepClient(writer, reader)

    @client.on_packet
    def on_packet(addr: int, data: bytes):
        print(f"Received packet from {addr:02x} with {len(data)} bytes")
        bfep = struct.pack(BFEP_PROTOCOL, addr, len(data)) + data
        pcap_output.add_packet(bfep)

    print("FEP Dump To Pcap Service Started")

    await client.recv_forever()


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <remote> <pcap>")
        sys.exit(1)

    asyncio.run(dump_pcap(sys.argv[1], sys.argv[2]))
