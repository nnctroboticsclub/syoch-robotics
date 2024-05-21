from utils.fep.fep_client import FepClient


def checksum(data: bytes) -> bytes:
    x = 0
    for b in data:
        x = ((x & 0x7F) << 9 | x >> 7) ^ 0x35CA ^ b ^ (b << 8)
    return x.to_bytes(2, "big")


def encode_rep(key: int, data: bytes) -> bytes:
    content = b""
    content += key.to_bytes(1, "big")
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


SELF_KEY = 0x00

DEV1_KEY = 9
DEV1_ADDR = 1

DEV2_ADDR = 2

SELF_ADDRESS = 0x80
GROUP_ADDRESS = 0xF0


async def main():
    fep = await FepClient.connect("localhost", 31337)

    print("\x1b[1;32m[*]\x1b[m Broadcasting the test command")

    await fep.transmit(GROUP_ADDRESS, encode_rep(0x00, bytes([SELF_KEY, 0x00])))
    await fep.transmit(GROUP_ADDRESS, encode_rep(0x00, bytes([SELF_KEY, 0x00])))
    to1_packet = encode_ssp(0x0003, b"ABCD")
    to2_packet = encode_ssp(0x0002, DEV1_ADDR.to_bytes(1, "big") + to1_packet)
    to1_packet = encode_ssp(0x0002, DEV2_ADDR.to_bytes(1, "big") + to2_packet)
    to1_packet = encode_rep(DEV1_KEY, to1_packet)
    await fep.transmit(DEV1_ADDR, to1_packet)

    print("\x1b[1;32m[*]\x1b[m Done")


if __name__ == "__main__":
    import asyncio

    asyncio.run(main())
