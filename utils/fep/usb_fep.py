import asyncio
import time
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

        self.send_line(b"")
        self.send_line(b"")
        self.send_line(b"")

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

                print(
                    "-" * 20
                    + ">  "
                    + f"\x1b[34mReceived packet: #{addr:03} {data.hex()}\x1b[m"
                )
                await self._dispatch_packet(addr, data)

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
        # print(f"--> {data.hex()}")
        self.serial.write(data)
        self.serial.flush()
        # print("--> [OK]")

    async def recv(self, size: int) -> bytes:
        # print("<-- wait")
        while self.serial.in_waiting < size:
            await asyncio.sleep(0.01)
        # print("<-- ...")
        line = self.serial.read(size)
        # print(f"<-- {line.hex()}")
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

    async def transmit_withspans(self, addr: int, data: bytes) -> list[float]:
        payload = b"@TBN"
        payload += str(addr).zfill(3).encode()
        payload += str(len(data)).zfill(3).encode()
        payload += data

        start = time.time()
        print("[1/3]")
        self.send_line(payload)

        print("[2/3]")
        await self.status_queue.get()
        t1 = time.time()

        print("[3/3]")
        await self.status_queue.get()
        t2 = time.time()

        spans = [t1 - start, t2 - t1]
        print(
            "-" * 20
            + f">  \x1b[32mTransmitted with spans = [{spans[0]:6.4f} {spans[1]:6.4f}]\x1b[m"
        )
        return spans
