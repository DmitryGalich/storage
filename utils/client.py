#!/usr/bin/env python

import asyncio
import websockets


async def main():
    async with websockets.connect("ws://localhost:8080") as websocket:
        while True:
            # print("Waiting for message...")
            # message = await websocket.recv()
            # print("Received: " + message)

            print("Enter message: ")
            message = input()
            await websocket.send(message)


if __name__ == "__main__":
    asyncio.run(main())
