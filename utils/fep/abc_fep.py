import asyncio
from typing import Callable


OnPacketCallback = Callable[[int, bytes], None]


class FepBase:
    def __init__(self):
        self.on_packet_callbacks: list[OnPacketCallback] = []

    async def _dispatch_packet(self, addr: int, data: bytes) -> None:
        for callback in self.on_packet_callbacks:
            ret = callback(addr, data)
            if asyncio.iscoroutine(ret):
                await ret

    async def init(self) -> None:
        raise NotImplementedError()

    async def get_reg(self, reg: int) -> int:
        raise NotImplementedError()

    async def set_reg(self, reg: int, value: int) -> None:
        raise NotImplementedError()

    async def reset(self) -> None:
        raise NotImplementedError()

    async def transmit_withspans(self, addr: int, data: bytes) -> list[float]:
        raise NotImplementedError()

    async def transmit(self, addr: int, data: bytes) -> None:
        await self.transmit_withspans(addr, data)

    def on_packet(self, callback: OnPacketCallback) -> None:
        self.on_packet_callbacks.append(callback)
