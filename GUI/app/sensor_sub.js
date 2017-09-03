var zmq = require('zeromq')
  , sock = zmq.socket('sub');

sock.connect('tcp://127.0.0.1:3000');
sock.subscribe('{');
console.log('Subscriber connected to port 3000');

sock.on('message', function(message) {
    str = String(message);
    var json = JSON.parse(str);
    console.log(json.FL[0]);
});
