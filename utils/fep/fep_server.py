import asyncio
from asyncio import StreamReader, StreamWriter

from utils.fep.abc_fep import FepBase
from utils.fep.usb_fep import UsbFep

PACKET = "! b b"


class FepServer:
    def __init__(self, fep: FepBase):
        self.fep = fep

        self.writers: list[StreamWriter] = []

        @self.fep.on_packet
        def on_packet(addr: int, data: bytes):
            for writer in self.writers:
                writer.write(addr.to_bytes(1, "big"))
                writer.write(len(data).to_bytes(1, "big"))
                writer.write(data)

    async def init(self):
        await self.fep.init()

        await self.fep.set_reg(18, await self.fep.get_reg(18) & 0xFC)
        await self.fep.set_reg(0, 0x01)
        await self.fep.set_reg(1, 0x02)
        await self.fep.reset()

    async def handle_client(self, reader: StreamReader, writer: StreamWriter):
        self.writers.append(writer)
        try:
            while True:
                addr = int.from_bytes(await reader.readexactly(1), "big")
                length = int.from_bytes(await reader.readexactly(1), "big")
                data = await reader.readexactly(length)
                await self.fep.transmit(addr, data)
        except asyncio.IncompleteReadError:
            pass

        self.writers.remove(writer)

        writer.close()

    async def run(self, port: int):
        server = await asyncio.start_server(self.handle_client, "", port)
        await self.init()
        addr = [sock.getsockname() for sock in server.sockets]

        print(f"Serving on {addr}")

        async with server:
            await server.serve_forever()


if __name__ == "__main__":
    fep = UsbFep("/dev/ttyUSB0")
    fep_server = FepServer(fep)
    asyncio.run(fep_server.run(31337))
