from typing import Callable


OnPacketCallback = Callable[[int, bytes], None]


class FepBase:
    def __init__(self):
        self.on_packet_callbacks: list[OnPacketCallback] = []

    def _dispatch_packet(self, addr: int, data: bytes) -> None:
        for callback in self.on_packet_callbacks:
            callback(addr, data)

    async def init(self) -> None:
        raise NotImplementedError()

    async def get_reg(self, reg: int) -> int:
        raise NotImplementedError()

    async def set_reg(self, reg: int, value: int) -> None:
        raise NotImplementedError()

    async def reset(self) -> None:
        raise NotImplementedError()

    async def transmit(self, addr: int, data: bytes) -> None:
        raise NotImplementedError()

    def on_packet(self, callback: OnPacketCallback) -> None:
        self.on_packet_callbacks.append(callback)
