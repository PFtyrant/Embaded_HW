import time
import random
import json
import datetime
from tornado import websocket, web, httpserver,ioloop
from random import randint
from pymongo import MongoClient
from bson.json_util import dumps

client = MongoClient('localhost',27017)
db = client.RS
collect = db.toilet

class webSocketHandler(websocket.WebSocketHandler):


    def check_origin(self,origin):
        return True
    
    def open(self):
        print('connection established.')
        ioloop.IOLoop.instance().add_timeout(datetime.
        timedelta(seconds=1), self.on_send)
    
    def on_close(self):
        print('connection closed.')

    def on_message(self,message):
        print(message)
        
    def on_send(self):
        js = dumps(list(collect.find()))
        self.write_message(js)
        ioloop.IOLoop.instance().add_timeout(datetime.timedelta(seconds=5),self.on_send)

if __name__ == "__main__":
    print("String websocket server program.")
    print("Awating client requests to open websocket...")
    application = web.Application([(r'/websocket', webSocketHandler)])
    application.listen(8001)
    ioloop.IOLoop.instance().start()