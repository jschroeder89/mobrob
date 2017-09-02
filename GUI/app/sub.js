var zmq = require('zeromq')
  , sock = zmq.socket('sub');

sock.connect('tcp://127.0.0.1:3000');
sock.subscribe('');
console.log('Subscriber connected to port 3000');

sock.on('message', function(message) {
    var obj = "";
    for (var i = 0; i < message.length; i++) {
        if (message[i] !== 0) {
            obj = obj.concat(String.fromCharCode(message[i]));
        } else {
            break;
        }
    }
    var json = JSON.parse(obj);
    console.log(json.FL[1], typeof json.FL[0]);
});
