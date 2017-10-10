var zmq = require('zeromq')
  , sock = zmq.socket('sub');

sock.connect('tcp://127.0.0.1:5555');
sock.subscribe('');
console.log('Subscriber connected to port 5555');
Plotly.plot('path', data, layout);

sock.on('message', function(message) {
    var json = JSON.parse(String(message));
    console.log(json.path[1], json.path[2]);
    trace1.x.push(json.path[1]);
    trace1.y.push(json.path[2]);
    //setTimeout(function(){}, 10000);
    Plotly.update('path', data, layout);
});
