// basic class implementation
function myCounter() {
    // privileged property for iteration
    this.i = 30;

    // privileged init method
    this.init();
}

// defining init method
myCounter.prototype.init = function () {
    var _this = this;
    setInterval(function () {
        _this.countDown();
    }, 1000);
};

// defining the counter method
myCounter.prototype.countDown = function () {
    document.getElementById("countdown").innerHTML = this.i + "s";
    this.i--;
    if(this.i <= 0) {
        this.i = 30;
        location.reload();
    }
};

// create a new instance of our counter class
var counter = new myCounter();