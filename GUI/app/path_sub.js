var zmq = require('zeromq')
  , sock = zmq.socket('sub');

sock.connect('tcp://127.0.0.1:5555');
sock.subscribe('');
console.log('Subscriber connected to port 5555');

sock.on('message', function(message) {
    str = String(message);
    console.log(str);
    /*var json = JSON.parse(str);
    trace1.x.push(7);
    trace1.y.push(8);
    console.log(trace1.x[8]);
    console.log(json.FL[0]);*/
});
