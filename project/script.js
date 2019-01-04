var ws = new WebSocket('ws://localhost:8001/websocket');


ws.onopen = function(event) {
    console.log("Connection open.")
}

ws.onmessage = function(event) {
    console.log("Receiving data.")
    console.log("Data : " + event.data)
}
