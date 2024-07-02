import asyncio
from asyncio import StreamReader, StreamWriter
import time

from .abc_fep import FepBase, OnPacketCallback

PACKET = "! b b"


class FepClient(FepBase):
    def __init__(self, writer: StreamWriter, reader: StreamReader):
        self.writer = writer
        self.reader = reader

        self.on_packet_callbacks: list[OnPacketCallback] = []

        asyncio.create_task(self.recv_forever())

    async def recv_forever(self):
        while True:
            addr = int.from_bytes(await self.reader.readexactly(1), "big")
            length = int.from_bytes(await self.reader.readexactly(1), "big")
            data = await self.reader.readexactly(length)

            for callback in self.on_packet_callbacks:
                ret = callback(addr, data)
                if asyncio.iscoroutine(ret):
                    await ret

    def on_packet(self, callback: OnPacketCallback):
        self.on_packet_callbacks.append(callback)

    async def get_reg(self, reg: int):
        raise NotImplementedError()

    async def set_reg(self, reg: int, value: int):
        raise NotImplementedError()

    async def reset(self):
        raise NotImplementedError()

    async def transmit_withspans(self, addr: int, data: bytes):
        start = time.time()
        self.writer.write(addr.to_bytes(1, "big"))
        self.writer.write(len(data).to_bytes(1, "big"))
        self.writer.write(data)
        t1 = time.time()
        await self.writer.drain()
        t2 = time.time()

        return [t1 - start, t2 - t1]

    @staticmethod
    async def connect(remote: str, port: int):
        reader, writer = await asyncio.open_connection(remote, port)
        return FepClient(writer, reader)

    @staticmethod
    async def connect_unix(path: str):
        reader, writer = await asyncio.open_unix_connection(path)
        return FepClient(writer, reader)
