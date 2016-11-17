# Socket.IO vs. Deepstream.IO pub/sub performance
This repository aims to shed some light on the pub/sub scene and display some performance differerences of various pub/sub implementations. Starting with Socket.IO vs. Deepstream.IO, we currently have one benchmark to present:

### Event broadcasting performance
The purpose of any pub/sub server is to propagate *publications* to a set of *subscribers*. Clients that are subscribed to a *topic* expect to receive a notification when other clients publish within this topic. It is similar to how the EventEmitter behaves, but spans the network and not just the local process.
