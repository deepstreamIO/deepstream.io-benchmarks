# Socket.IO vs. Deepstream.IO pub/sub performance
This repository aims to shed some light on the pub/sub scene and display some performance differerences of various pub/sub implementations. Starting with Socket.IO vs. Deepstream.IO, we currently have one benchmark to present:

### Event broadcasting performance
The purpose of any pub/sub server is to propagate *publications* to a set of *subscribers*. Clients that are subscribed to a *room* expect to receive a notification when other clients publish within this room. It is similar to how the EventEmitter behaves, but spans the network and not just the local process.

In this benchmark we are going to investigate the time spent while propagating X simultaneous publications to a room of Y subscribed clients. Every data point is generated with a highly performant C++ client specially hardcoded for each pub/sub protocol:

* Wait until the server is completely idle
* Establish Y subscribed clients, all to the same room.
* Repeat 10 times:
  * Start timer
  * For X randomly selected connections: publish an event to the server
  * Wait until all Y clients have received all expected notifications from the server
  * Store time delay
* Print average delay for these 10 iterations

#### Results
Test case | Socket.IO 1.5.1 | Deepstream.IO v2.0 | Multiplier
--- | --- | --- | ---
10 sub, 1 pub | 0.3 ms | 1.3 ms | 0.23x
100 sub, 10 pub | 27.5 ms | 1.6 ms | 17x
1000 sub, 100 pub | 2303.8 ms | 15.5 ms | 153x
10000 sub, 1000 pub | 274335.6 ms | 206.5 ms | 1331x
