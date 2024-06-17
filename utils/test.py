from utils.fep.fep_client import FepClient
from random import randint


def checksum(data: bytes) -> bytes:
    x = 0
    for b in data:
        x = ((x & 0x7F) << 9 | x >> 7) ^ 0x35CA ^ b ^ (b << 8)
    return x.to_bytes(2, "big")


def encode_rep(data: bytes) -> bytes:
    content = b""
    content += len(data).to_bytes(1, "big")
    content += data

    ret = b"\x55\xaa\xcc"
    ret += content
    ret += checksum(content)
    return ret


def encode_ssp(svc: int, data: bytes) -> bytes:
    ret = svc.to_bytes(2, "big")
    ret += b"\x00\x00"
    ret += data
    return ret


DEV1_ADDR = 1
DEV2_ADDR = 2

GROUP_ADDRESS = 0xF0


def generate_packet_relay_test():
    packet = encode_ssp(0x0003, b"ABCD")
    for i in range(3):
        packet = encode_ssp(0x0002, DEV1_ADDR.to_bytes(1, "big") + packet)
        packet = encode_ssp(0x0002, DEV2_ADDR.to_bytes(1, "big") + packet)
    packet = encode_rep(packet)

    return packet


def generate_packet_vs_test1():
    value = randint(0, 0xFFFF)
    payload = b"\x60" + value.to_bytes(2, "big")
    return encode_rep(encode_ssp(0x8000, payload))


async def main():
    fep = await FepClient.connect("localhost", 31337)

    @fep.on_packet
    def on_packet(addr, packet):
        rep_data = packet[4:-2]

        svc_id = int.from_bytes(rep_data[:2], "big")
        svc_payload = rep_data[4:]

        key_id = int.from_bytes(svc_payload[:2], "big")
        value = svc_payload[2:]

        print(f"Received packet: {addr} {svc_id}-{key_id} {value}")

    print("\x1b[1;32m[*]\x1b[m Sending the test command")

    """ await fep.transmit(DEV1_ADDR, encode_rep(encode_ssp(0x0000, b"\x00\x00")))
    await asyncio.sleep(0.1)
    await fep.transmit(DEV1_ADDR, encode_rep(encode_ssp(0x0000, b"\x00\x01")))
    await asyncio.sleep(0.1)
    await fep.transmit(DEV1_ADDR, encode_rep(encode_ssp(0x0000, b"\x00\x02")))
    await asyncio.sleep(0.1)

    await fep.transmit(DEV2_ADDR, encode_rep(encode_ssp(0x0000, b"\x00\x00")))
    await asyncio.sleep(0.1)
    await fep.transmit(DEV2_ADDR, encode_rep(encode_ssp(0x0000, b"\x00\x01")))
    await asyncio.sleep(0.1)
    await fep.transmit(DEV2_ADDR, encode_rep(encode_ssp(0x0000, b"\x00\x02")))
    await asyncio.sleep(2.1) """

    await fep.transmit(DEV1_ADDR, generate_packet_vs_test1())

    print("\x1b[1;32m[*]\x1b[m Done")


if __name__ == "__main__":
    import asyncio

    asyncio.run(main())
