import asyncio
import json
import logging
from websockets import serve, exceptions

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s"
)

clients = {}
host_client = None
current_map = ''
next_client_id = 0

async def broadcast(message, exclude=None):
    payload = json.dumps(message)
    for ws, client in clients.items():
        if exclude and ws == exclude:
            continue
        try:
            await ws.send(payload)
        except exceptions.ConnectionClosedError:
            logging.warning(f"Connection closed for client {client['id']}")
            await unregister(client)

async def unregister(client):
    global host_client
    ws = client['ws']
    if ws in clients:
        del clients[ws]
    if client == host_client:
        logging.info("Host has disconnected")
        host_client = None
    logging.info(f"Cleaned up client {client['id']}")

async def process_message(client, message):
    global host_client
    try:
        data = json.loads(message)
        command = data.get('command')
        logging.info(f"[Client {client['id']}] Received command: {command}")

        if command == "HOST":
            if host_client: return
            client['is_host'] = True
            client['name'] = data.get('data', 'Player')
            host_client = client
            logging.info(f"Client {client['id']} is now host named '{client['name']}'")
            await broadcast({'command': 'HOST_UPDATE', 'host_id': client['id'], 'host_name': client['name']})

        elif command == "UPDATE_MAP":
            if not isinstance(data.get('data'), str):
                await client['ws'].send(json.dumps({'error': 'Invalid map format'}))
                return
            global current_map
            current_map = data['data']
            segments = [seg for seg in current_map.split(';') if seg]
            last = segments[-1] if segments else ''
            logging.info(f"Host updated map. Last segment: '{last}'")
            await broadcast({'command': 'UPDATE_MAP', 'data': f"{segments[0]};{last}"}, exclude=client['ws'])

        elif command == "MOVE_OBJECT":
            
            await broadcast({'command': 'MOVE_OBJECT', 'data': {'BlockX':data['data']['blockX'],'BlockY':data['data']['blockY'],'MoveX':data['data']['moveX'],'MoveY':data['data']['moveY']}}, exclude=client['ws'])
    except json.JSONDecodeError:
        logging.warning(f"invalid json")

async def handler(websocket):
    global next_client_id
    client_id = next_client_id
    next_client_id += 1
    client = {'ws': websocket, 'id': client_id, 'is_host': False, 'name': 'Player'}
    clients[websocket] = client
    logging.info(f"Client {client_id} connected from {websocket.remote_address}")

    try:
        async for message in websocket:
            await process_message(client, message)
    except exceptions.ConnectionClosedError:
        logging.info(f"Client {client_id} disconnected")
    finally:
        await unregister(client)

async def main():
    async with serve(handler, "localhost", 5656) as server:
        logging.info("WebSocket server running on ws://localhost:5656")
        await server.serve_forever()

if __name__ == "__main__":
    asyncio.run(main())
