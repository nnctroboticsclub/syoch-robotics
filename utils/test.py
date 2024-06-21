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
    ret += data
    return ret


def encode_froute(dest: int, data: bytes) -> bytes:
    ret = b""
    ret += b"\x80"  # life
    ret += b"\x80"  # from
    ret += dest.to_bytes(1, "big")
    ret += b"\x00"  # Flags
    ret += data
    return ret


def encode_froute_adv(addr_self: int) -> bytes:
    ret = b""
    ret += b"\x80"  # life
    ret += addr_self.to_bytes(1, "big")
    ret += b"\0"  # goal
    ret += b"\x02"  # Flags

    return ret


def encode(dest: int, svc_id: int, key_id: int, value: bytes) -> bytes:
    ret = encode_ssp(svc_id, key_id.to_bytes(2, "big") + value)
    ret = encode_froute(dest, ret)
    ret = encode_rep(ret)
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


def generate_packet_ni_init():
    payload = b"\x00"
    return encode_rep(encode_ssp(0x0400, payload))


async def main():
    fep = await FepClient.connect("localhost", 31337)

    packets_received = 0

    @fep.on_packet
    def on_packet(addr, packet):
        nonlocal packets_received
        rep_data = packet[4:-2]

        frt_header = rep_data[:4]
        frt_data = rep_data[4:]

        svc_id = int.from_bytes(frt_data[:2], "big")
        svc_payload = frt_data[2:]

        key_id = int.from_bytes(svc_payload[:2], "big")
        value = svc_payload[2:]

        print(f"Received packet: {addr} {svc_id}-{key_id} {value}")

        packets_received += 1

    print("\x1b[1;32m[*]\x1b[m Sending the bad command")
    await fep.transmit(
        DEV2_ADDR, encode_rep(encode_froute(0x02, encode_ssp(0x0000, b"\x00\x05")))
    )

    print("\x1b[1;32m[*]\x1b[m Advertising self")
    await fep.transmit(DEV1_ADDR, encode_rep(encode_froute_adv(0x80)))
    await fep.transmit(DEV2_ADDR, encode_rep(encode_froute_adv(0x80)))
    await asyncio.sleep(0.5)

    print("----- Sending 20 packets with 50ms delay...")
    for _ in range(20):
        await fep.transmit(DEV2_ADDR, encode(0x02, 0x0000, 2, b""))
        await asyncio.sleep(0.05)
    print("----- Result")

    score = packets_received
    print(f"Score: {score}")

    print(f"{packets_received/20*100}% of packets received in 2secs")
    speed = packets_received / (20 * 0.5)
    print(f"Speed: {speed} packets/sec")

    print("\x1b[1;32m[*]\x1b[m Done")


if __name__ == "__main__":
    import asyncio

    asyncio.run(main())
