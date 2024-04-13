import asyncio
from typing import Any
import websockets
import hashlib


async def main(host: str, filter_tag: bytes = b""):
    async with websockets.connect(
        f"ws://{host}/sp/watch", ping_timeout=3600
    ) as websocket:
        print(f"Connected to {host}")
        prev_received_message_hash = None
        while True:
            data_: Any = await websocket.recv()
            data: bytes = data_

            tag_length = int.from_bytes(data[0:2], "big")
            buf_length = int.from_bytes(data[2:4], "big")

            tag = data[4 : 4 + tag_length]
            buf = data[4 + tag_length : 4 + tag_length + buf_length]

            remaining_data = data[4 + tag_length + buf_length :]

            if tag != filter_tag:
                continue

            data_hash = hashlib.sha1(data).digest()
            if prev_received_message_hash:
                if data_hash == prev_received_message_hash:
                    continue

            prev_received_message_hash = data_hash

            print(buf.decode("utf-8", errors="ignore"), end="")
            """ if remaining_data:
                print("<" + " ".join(f"{x:02x}" for x in remaining_data) + ">", end="") """


if __name__ == "__main__":
    import sys
    import os

    filter_tag = b""

    if len(sys.argv) > 1:
        filter_tag = sys.argv[1].encode("utf-8")
    else:
        print("Usage: python log-reader.py <filter_tag>")
        sys.exit(1)

    host = os.environ.get("ESP32_IP", "localhost:8011")

    asyncio.run(main(host, filter_tag))
