var zmq = require('zeromq')
  , sock = zmq.socket('sub');

sock.connect('tcp://127.0.0.1:5555');
sock.subscribe('');
console.log('Subscriber connected to port 5555');

initializeSvgs();
console.log(returnSensorField(2));
var test = valueToHexStringColor(1000);
console.log(test);

sock.on('message', function(message) {
    console.log(String(message));
    str = String(message);
    var json = JSON.parse(str);
    console.log(json.FL[0]);
});
