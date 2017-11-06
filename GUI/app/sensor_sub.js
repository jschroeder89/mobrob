var zmq = require('zeromq')
  , sock = zmq.socket('sub');

sock.connect('tcp://127.0.0.1:5555');
sock.subscribe('');
console.log('Subscriber connected to port 5555');

initializeSvgs();
sock.on('message', function(message) {
    var json = JSON.parse(String(message));
    for (var i = 0; i < json.sensor.length; i++) {
        console.log(json.sensor);
        colorizeSensorField(i, valueToHexStringColor(json.sensor[i]));
    }
});
