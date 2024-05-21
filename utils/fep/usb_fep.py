import asyncio
from serial import Serial

from .abc_fep import FepBase


class UsbFep(FepBase):
    def __init__(self, path: str):
        super().__init__()
        self.serial = Serial(path, timeout=0.1)

        self.status_queue = asyncio.Queue()
        self.value_queue = asyncio.Queue()

    async def init(self):
        asyncio.create_task(self.task())

    async def task(self):
        while True:
            header = self.recv(3)

            if not header:
                await asyncio.sleep(0.01)
                continue

            if header == b"RBN":
                addr = int(self.recv(3))
                length = int(self.recv(3))
                data = self.recv(length)
                self.recv(2)

                self._dispatch_packet(addr, data)

            elif header[0] == ord("P") or header[0] == ord("N"):
                # P, n, \r, \n
                self.recv(1)

                status = header[0] == ord("P")
                code = header[1] - ord("0")

                await self.status_queue.put((status, code))
            elif header[2] == ord("H"):  # Register response
                self.recv(2)
                value = int(header[0:2].decode(), 16)
                await self.value_queue.put(value)

    def send_line(self, line: bytes) -> None:
        self.serial.write(line + b"\r\n")

    def recv(self, size: int) -> bytes:
        line = self.serial.read(size)
        return line

    async def get_reg(self, reg: int) -> int:
        self.send_line(f"@REG{reg:02d}".encode())
        value = await self.value_queue.get()

        print(f"\x1b[33mREG{reg:02d}\x1b[m  =  \x1b[34m{value:02x}\x1b[m")
        return value

    async def set_reg(self, reg: int, value: int) -> None:
        print(f"\x1b[33mREG{reg:02d}\x1b[m <-- \x1b[34m{value:02x}\x1b[m")
        self.send_line(f"@REG{reg:02d}:{value:03d}".encode())
        await self.status_queue.get()

    async def reset(self) -> None:
        self.send_line(b"@RST")
        await self.status_queue.get()

    async def transmit(self, addr: int, data: bytes) -> None:
        payload = b"@TBN"
        payload += str(addr).zfill(3).encode()
        payload += str(len(data)).zfill(3).encode()
        payload += data

        self.send_line(payload)
        await self.status_queue.get()
        await self.status_queue.get()
