# #!/usr/bin/env python

# import asyncio
# import websockets


# async def hello():
#     async with websockets.connect("ws://localhost:8080") as websocket:
#         await websocket.send("Hello from server")
#         await websocket.send("Again Hello from server")

#         # await websocket.recv()

# asyncio.run(hello())

# !/usr/bin/env python

import asyncio
import websockets


async def handler(websocket):
    while True:
        # print("Enter message: ")
        # message = input()
        # await websocket.send(message)
        message = await websocket.recv()
        print(message)


async def main():
    async with websockets.serve(handler, "127.0.0.1", 8080):
        await asyncio.Future()  # run forever


if __name__ == "__main__":
    asyncio.run(main())
