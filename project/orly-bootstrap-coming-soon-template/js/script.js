jQuery(document).ready(function() {	
var ws = new WebSocket('ws://localhost:8001/websocket');

document.getElementById("supply").onclick = function()
{
        swal("Thank you!", ", We will service as soon as possible", "success");
        console.log("Send notify to server.")
        ws.send(JSON.stringify({
            text: "supply"
          }));
};

ws.onopen = function(event) {
    console.log("Connection open.")
}

ws.onmessage = function(event) {
    console.log("Receiving data.")
    console.log("Data : " + event.data)
    var obj = JSON.parse(event.data)
    for ( var i = 0 ; i < obj.length; i++ ){
        if ( obj[i].ID == 103) {
            var T1 = document.getElementById("t1")
            var T1_people = document.getElementById("t1-people")
            var T1_index1 = document.getElementById("t1-index1")
            var T1_index2 = document.getElementById("t1-index2")
            T1_people.innerHTML = "Waiting People : " + obj[i].WaitingPeople
            if ( obj[i].locked0 == 1)
                T1_index1.innerHTML = "Toilet1 : " + "Locked"
            else
                T1_index1.innerHTML = "Toilet1 : " + "Empty"

            if ( obj[i].locked1 == 1)
                T1_index2.innerHTML = "Toilet2 : " + "Locked"
            else
                T1_index2.innerHTML = "Toilet2 : " + "Empty"


            T1.innerHTML = " Rest room 1 : " + obj[i].Score

            var colorset = Math.floor( obj[i].Score / 20)
            if (colorset >=0 && colorset < 1)
                T1.classList.add("w3-win8-red")
            else if ( colorset >=1 && colorset < 2)
                T1.classList.add("w3-win8-orange")
            else if ( colorset >=2 && colorset < 3)
                T1.classList.add("w3-win8-yellow")
            else if ( colorset >=3 && colorset < 4)
                T1.classList.add("w3-win8-lime")
            else
                T1.classList.add("w3-win8-emerald")
        }
        else {
            var T2 = document.getElementById("t2")
            
            var T2_people = document.getElementById("t2-people")
            var T2_index1 = document.getElementById("t2-index1")
            var T2_index2 = document.getElementById("t2-index2")
            T2_people.innerHTML = "Waiting People : " + obj[i].WaitingPeople
            if ( obj[i].locked0 == 1)
                T2_index1.innerHTML = "Toilet1 : " + "Locked"
            else
                T2_index1.innerHTML = "Toilet1 : " + "Empty"

            if ( obj[i].locked1 == 1)
                T2_index2.innerHTML = "Toilet2 : " + "Locked"
            else
                T2_index2.innerHTML = "Toilet2 : " + "Empty"


            T2.innerHTML = " Rest room 2 : " + obj[i].Score

            var colorset = Math.floor( obj[i].Score / 20)
            if (colorset >=0 && colorset < 1)
                T2.classList.add("w3-win8-red")
            else if ( colorset >=1 && colorset < 2)
                T2.classList.add("w3-win8-orange")
            else if ( colorset >=2 && colorset < 3)
                T2.classList.add("w3-win8-yellow")
            else if ( colorset >=3 && colorset < 4)
                T2.classList.add("w3-win8-lime")
            else
                T2.classList.add("w3-win8-emerald")
        }
    }    
}
ws.onclose = function(event) {
    console.log("Close connection.")
}
});