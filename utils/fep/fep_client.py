import asyncio
from asyncio import StreamReader, StreamWriter

from .abc_fep import OnPacketCallback

PACKET = "! b b"


class FepClient:
    def __init__(self, writer: StreamWriter, reader: StreamReader):
        self.writer = writer
        self.reader = reader

        self.on_packet_callbacks: list[OnPacketCallback] = []

    async def recv_forever(self):
        while True:
            addr = int.from_bytes(await self.reader.readexactly(1), "big")
            length = int.from_bytes(await self.reader.readexactly(1), "big")
            data = await self.reader.readexactly(length)

            for callback in self.on_packet_callbacks:
                callback(addr, data)

    def on_packet(self, callback: OnPacketCallback):
        self.on_packet_callbacks.append(callback)

    async def transmit(self, addr: int, data: bytes):
        self.writer.write(addr.to_bytes(1, "big"))
        self.writer.write(len(data).to_bytes(1, "big"))
        self.writer.write(data)
        await self.writer.drain()

    @staticmethod
    async def connect(remote: str, port: int):
        reader, writer = await asyncio.open_connection(remote, port)
        return FepClient(writer, reader)
