from ws4py.websocket import WebSocket

class EchoWebSocket(WebSocket):
    def received_message(self, message):
        print(message)


if __name__ == "__main__":
