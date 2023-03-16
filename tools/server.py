#!/usr/bin/env python

import asyncio
import websockets


async def hello(websocket, path):
    client_message = await websocket.recv()
    print("< {}".format(client_message))

    server_message = "Server KEK"
    await websocket.send(server_message)
    print("> {}".format(server_message))

start_server = websockets.serve(hello, 'localhost', 8000)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
