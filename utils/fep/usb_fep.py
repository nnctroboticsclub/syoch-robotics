import asyncio
from serial import Serial

from .abc_fep import FepBase


class UsbFep(FepBase):
    def __init__(self, path: str):
        print("---------------------------------------------")
        super().__init__()
        # self.serial = open(path, "r+b", buffering=0)
        self.serial = Serial(path, baudrate=115200)

        self.status_queue = asyncio.Queue()
        self.value_queue = asyncio.Queue()
        self.line_queue = asyncio.Queue()

    async def init(self):
        asyncio.create_task(self.task())

    async def task(self):
        while True:
            header = await self.recv(3)

            if not header:
                await asyncio.sleep(0.01)
                continue

            if header == b"RBN":
                addr = int(await self.recv(3))
                length = int(await self.recv(3))
                data = await self.recv(length)
                await self.recv(2)

                print(f"Received packet: #{addr:03} {data.hex()}")
                self._dispatch_packet(addr, data)

            elif header[0] == ord("P") or header[0] == ord("N"):
                # P, n, \r, \n
                await self.recv(1)

                status = header[0] == ord("P")
                code = header[1] - ord("0")

                await self.status_queue.put((status, code))
            else:
                line = header + await self.readline()

                await self.line_queue.put(line)

    def send_line(self, data: bytes) -> None:
        data += b"\r\n"
        # print(f"--> {data}")
        self.serial.write(data)
        self.serial.flush()

    async def recv(self, size: int) -> bytes:
        while not self.serial.in_waiting:
            await asyncio.sleep(0.01)
        line = self.serial.read(size)
        # print(f"<-- {line}")
        return line

    async def readline(self) -> bytes:
        line = b""
        while True:
            char = await self.recv(1)
            if char == b"\n":
                break
            line += char
        return line

    async def get_reg(self, reg: int) -> int:
        self.send_line(f"@REG{reg:02d}".encode())
        line = await self.line_queue.get()
        value = int(line[0:2], 16)

        print(f"\x1b[33mREG{reg:02d}\x1b[m  =  \x1b[34m{value:02x}\x1b[m")
        return value

    async def set_reg(self, reg: int, value: int) -> None:
        self.send_line(f"@REG{reg:02d}:{value:03d}".encode())

        res = await self.status_queue.get()

        print(f"\x1b[33mREG{reg:02d}\x1b[m <-- \x1b[34m{value:02x}\x1b[m : {res}")

    async def reset(self) -> None:
        self.send_line(b"@RST")
        await self.status_queue.get()

    async def transmit(self, addr: int, data: bytes) -> None:
        payload = b"@TBN"
        payload += str(addr).zfill(3).encode()
        payload += str(len(data)).zfill(3).encode()
        payload += data

        print(f"Transmitting packet: #{addr:03} {data.hex()}")

        self.send_line(payload)
        await self.status_queue.get()
        await self.status_queue.get()
