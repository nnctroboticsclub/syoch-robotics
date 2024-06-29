import asyncio
import datetime
import struct
import time
from utils.fep.fep_client import FepClient
from random import randint


def checksum(data: bytes) -> bytes:
    x = 0
    for b in data:
        x = ((x & 0x7F) << 9 | x >> 7) ^ 0x35CA ^ b ^ (b << 8)
    return x.to_bytes(2, "big")


def encode_rep(data: bytes) -> bytes:
    ret = checksum(data)
    ret += data
    return ret


def encode_ssp(svc: int, data: bytes) -> bytes:
    ret = svc.to_bytes(1, "big")
    ret += data
    return ret


def encode_froute(dest: int, data: bytes) -> bytes:
    address_pair = (0x0F << 4) | dest
    flags = 0

    ret = b""
    ret += struct.pack("<BB", address_pair, flags)
    ret += data
    return ret


def encode_froute_adv(addr_self: int) -> bytes:
    address_pair = (addr_self << 4) | 0xF0
    flags = 0x02

    return struct.pack("<BB", address_pair, flags)


def encode(dest: int, svc_id: int, key_id: int, value: bytes) -> bytes:
    ret = encode_ssp(svc_id, key_id.to_bytes(1, "big") + value)
    ret = encode_froute(dest, ret)
    ret = encode_rep(ret)
    return ret


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


class Timeline:
    def __init__(self) -> None:
        self.datas: list[float] = []
        self.start: float = 0

    def start_timer(self) -> None:
        self.datas = []
        self.start = time.time()

    def add_data(self) -> None:
        self.datas.append(time.time())

    def latency(self) -> float:
        if not self.datas:
            return 0
        first = sorted(self.datas)[0]
        return first - self.start

    def ave_speed(self) -> float:
        spans = [self.datas[i + 1] - self.datas[i] for i in range(len(self.datas) - 1)]
        if not spans:
            return 0
        return sum(spans) / len(spans)


class RoboFEPClient:
    def __init__(self, self_addr: int) -> None:
        self.fep: FepClient = None
        self.tl = Timeline()
        self.packets_received = 0
        self.self_addr = self_addr

        self.tx_packets = 0

    async def on_packet(self, addr: int, packet: bytes):
        rep_data = packet[2:]

        frt_header = rep_data[:2]
        frt_from = frt_header[0] >> 4
        frt_to = frt_header[0] & 0x0F
        frt_flags = frt_header[1] & 0x03
        frt_data = rep_data[2:]

        svc_id = int.from_bytes(frt_data[:1], "big")

        payload = frt_data[1:]

        print(
            f"Received packet: #{addr:03} [{frt_from:03} --> {frt_to:03}] (f{frt_flags}) ${svc_id}: {payload.hex()}"
        )

        self.tl.add_data()
        self.packets_received += 1

        if frt_flags == 1:
            self_addr_byte = bytes([self.self_addr])
            if self_addr_byte not in payload:
                await self.advertise_to(frt_from)

    async def connect(self, host: str, port: int) -> None:
        self.fep = await FepClient.connect(host, port)

        @self.fep.on_packet
        async def on_packet(addr, packet):
            await self.on_packet(addr, packet)

    def get_packets_received(self) -> int:
        return self.packets_received

    async def raw_transmit(self, dest: int, raw_packet: bytes) -> None:
        print(f"Transmitting packet: {raw_packet.hex()}")
        await self.fep.transmit(dest, raw_packet)

    async def advertise_to(self, dest: int) -> None:
        await self.raw_transmit(dest, encode_rep(encode_froute_adv(self.self_addr)))


DEV1_ADDR = 1
DEV2_ADDR = 2

GROUP_ADDRESS = 0xF0


class App:
    def __init__(self) -> None:
        self.fep = RoboFEPClient(0x0F)
        self.rx_should_come = 1

    async def send_bad_command(self):
        print("\x1b[1;32m[*]\x1b[m Sending the bad command")
        await self.fep.raw_transmit(
            DEV2_ADDR, encode_rep(encode_froute(0x02, encode_ssp(0x00, b"\x05")))
        )
        await asyncio.sleep(1)

    async def show_status(self):
        packets_received = self.fep.get_packets_received()
        loss = 1 - packets_received / self.rx_should_come

        print("==========")
        print(f"Loss, Packets: {loss * 100:6.2f}%, {packets_received}")
        print(
            f"Ave Speed, Latency: {self.fep.tl.ave_speed():5.1f} packets/sec, {self.fep.tl.latency()}"
        )

    async def status_task(self):
        while True:
            await self.show_status()
            await asyncio.sleep(1)

    async def send_benchmark(self):
        delay = 0.1
        self.rx_should_come = 100
        print(f"----- Sending {self.rx_should_come} packets with {delay}s delay...")
        self.fep.tl.start_timer()
        for _ in range(self.rx_should_come):
            await self.fep.raw_transmit(DEV2_ADDR, encode(0x01, 0x00, 2, b""))
            await asyncio.sleep(delay)

    async def main(self):
        await self.fep.connect("localhost", 31337)

        await self.send_bad_command()

        asyncio.create_task(self.send_benchmark())
        asyncio.create_task(self.status_task())

        await asyncio.sleep(999999999999999999999)


async def main():
    app = App()
    await app.main()


if __name__ == "__main__":
    asyncio.run(main())
