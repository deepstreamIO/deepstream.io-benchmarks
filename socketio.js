var io = require('socket.io')(3000);

io.on('connection', function(socket) {
    socket.on('eventName', function(message) {
      socket.broadcast.emit('eventName');
    });
});
