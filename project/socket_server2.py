from websocket_server import WebsocketServer
from pymongo import MongoClient
import json

from bson.json_util import dumps
client = MongoClient('localhost',27017)
db = client.RS
collect = db.toilet



def updateDB(data):
    collect.delete_one({"ID" : 203})
    json_string = data 
    obj = json.loads(json_string)
    collect.insert(obj)


# Called for every client connecting (after handshake)
def new_client(client, server):
    print("New client connected and was given id %d" % client['id'])
 
# Called for every client disconnecting
def client_left(client, server):
    print("Client(%d) disconnected" % client['id'])
 
 
# Called when a client sends a message
def message_received(client, server, message):
    print (message)
    updateDB(message)
    
    #server.send_message(client,'HIHI'+str(client['id'])+':'+message)

if __name__ == "__main__":
    PORT=9081
    server = WebsocketServer(PORT,host="192.168.1.15")
    server.set_fn_new_client(new_client)
    server.set_fn_client_left(client_left)
    server.set_fn_message_received(message_received)
    server.run_forever()