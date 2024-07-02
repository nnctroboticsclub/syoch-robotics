import asyncio
from asyncio import StreamReader, StreamWriter

from utils.fep.abc_fep import FepBase
from utils.fep.usb_fep import UsbFep

PACKET = "! b b"


class FepServer:
    def __init__(self, fep: FepBase):
        self.fep = fep

        self.writers: list[StreamWriter] = []

        self.need_to_drain = False
        self.task = None

        @self.fep.on_packet
        def on_packet(addr: int, data: bytes):
            payload = addr.to_bytes(1, "big")
            payload += len(data).to_bytes(1, "big")
            payload += data
            for writer in self.writers:
                writer.write(payload)

    async def drain_if_need(self):
        while True:
            if self.need_to_drain:
                for writer in self.writers:
                    await writer.drain()
                self.need_to_drain = False

            await asyncio.sleep(0.1)

    async def init(self):
        self.task = asyncio.create_task(self.drain_if_need())

        await self.fep.init()

        # await self.fep.set_reg(18, await self.fep.get_reg(18) & 0xFC)
        await self.fep.set_reg(0, 0x0F)
        await self.fep.set_reg(1, 0xF0)
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
        except BrokenPipeError:
            pass

        self.writers.remove(writer)

        writer.close()

    async def run(self, path: str):
        server = await asyncio.start_unix_server(self.handle_client, path)
        await self.init()
        addr = [sock.getsockname() for sock in server.sockets]

        print(f"Serving on {addr}")

        async with server:
            await server.serve_forever()


if __name__ == "__main__":
    import os

    dev = os.environ.get("FEP_DEVICE", "/dev/ttyUSB0")
    path = os.environ.get("FEP_SOCKET", "/tmp/fep.sock")

    fep = UsbFep(dev)
    fep_server = FepServer(fep)
    asyncio.run(fep_server.run(path))
