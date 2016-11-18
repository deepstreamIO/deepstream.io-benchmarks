// Highly optimized event broadcast benchmark client for measuring Socket.IO vs. DeepStream.IO
// This client properly stresses both servers to 100% CPU usage at any given time

#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include <chrono>

uWS::Hub hub;
int received = 0;
int connections = 0;
int logins = 0;
int messages = 0;
int batchConnect;
int port;
std::vector<uWS::WebSocket<uWS::CLIENT>> sockets;

enum Protocol {
    SOCKET_IO,
    DEEPSTREAM_IO
} protocol;

uWS::WebSocket<uWS::CLIENT>::PreparedMessage *preparedMessage;
std::chrono::high_resolution_clock::time_point start;
int iterations = 0;

void sendPong(uWS::WebSocket<uWS::CLIENT> ws) {
    switch (protocol) {
    case DEEPSTREAM_IO:
        const char CPO[] = {'C', 31, 'P', 'O', 30};
        ws.send(CPO, sizeof(CPO), uWS::OpCode::TEXT);
        break;
    }
}

void nextConnection() {
    static int addr = 1;
    switch (protocol) {
    case SOCKET_IO:
        hub.connect("ws://127.0.0." + std::to_string(addr) + ":" + std::to_string(port) + "/socket.io/?EIO=4&transport=websocket", nullptr, 10000);
        break;
    case DEEPSTREAM_IO:
        hub.connect("ws://127.0.0." + std::to_string(addr) + ":" + std::to_string(port) + "/deepstream", nullptr, 10000);
        break;
    }
    if (sockets.size() % 20000 == 0) {
        addr++;
    }
}

void iterate() {
    if (received != 0) {
        iterations++;
        int ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        if (ms > 1000) {
            std::cout << connections << "," << messages << "," << (float(ms) / iterations) << std::endl;
            hub.getDefaultGroup<uWS::CLIENT>().close();
            return;
        }
    } else if (iterations == 0) {
        start = std::chrono::high_resolution_clock::now();
    }

    received = 0;
    for (int i = 0; i < messages; i++) {
        sockets[rand() % sockets.size()].sendPrepared(preparedMessage);
    }
}

void beginIterating() {
    switch (protocol) {
    case SOCKET_IO:
        hub.onMessage([](uWS::WebSocket<uWS::CLIENT> ws, char *message, size_t length, uWS::OpCode opCode) {
            if (length % 15 == 0) {
                received += length / 15;
                if (received == (connections - 1) * messages) {
                    iterate();
                }
            }
        });
        break;
    case DEEPSTREAM_IO:
        hub.onMessage([](uWS::WebSocket<uWS::CLIENT> ws, char *message, size_t length, uWS::OpCode opCode) {
            if (length % 16 != 0) {
                if (length == 5) {
                    sendPong(ws);
                }
            } else {
                received += length / 16;
                if (received == (connections - 1) * messages) {
                    iterate();
                }
            }
        });
        break;
    }
    iterate();
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        showUsage:
        std::cout << "Usage: eventBenchmark sub pub port socketio|deepstreamio" << std::endl;
        return 0;
    } else {
        connections = atoi(argv[1]);
        messages = atoi(argv[2]);
        batchConnect = std::min<int>(100, connections);
        port = atoi(argv[3]);
        std::string protocolString = argv[4];
        protocol = (protocolString == "socketio" ? SOCKET_IO : DEEPSTREAM_IO);
        if (protocol == DEEPSTREAM_IO && protocolString != "deepstreamio") {
            goto showUsage;
        }
    }

    char line[1024] = {};
    do {
        FILE *pipe = popen(("fuser " + std::to_string(port) + "/tcp 2> /dev/null").c_str(), "r");
        fgets(line, sizeof(line), pipe);
        pclose(pipe);
        int pid = atoi(line);
        pipe = popen(("ps -p " + std::to_string(pid) + " -o state").c_str(), "r");
        fgets(line, sizeof(line), pipe);
        pclose(pipe);
        usleep(100);
    } while (line[0] != 'S');

    switch (protocol) {
    case SOCKET_IO:
    {
        static const char emitMessage[] = {'4', '2', '[', '\"', 'e', 'v', 'e', 'n', 't', 'N', 'a', 'm', 'e', '\"', ']'};
        preparedMessage = uWS::WebSocket<uWS::CLIENT>::prepareMessage((char *) emitMessage, sizeof(emitMessage), uWS::OpCode::TEXT, false);
    }

        hub.onMessage([](uWS::WebSocket<uWS::CLIENT> ws, char *message, size_t length, uWS::OpCode opCode) {
            if (length == 2 && message[0] == '4' && message[1] == '0') {
                sockets.push_back(ws);
                if (++logins == connections) {
                    beginIterating();
                } else if (logins + batchConnect <= connections) {
                    nextConnection();
                }
            }
        });
        break;
    case DEEPSTREAM_IO:
    {
        static const char emitMessage[] = {'E', 31, 'E', 'V', 'T', 31, 'e', 'v', 'e', 'n', 't', 'N', 'a', 'm', 'e', 30};
        preparedMessage = uWS::WebSocket<uWS::CLIENT>::prepareMessage((char *) emitMessage, sizeof(emitMessage), uWS::OpCode::TEXT, false);
    }

        hub.onMessage([](uWS::WebSocket<uWS::CLIENT> ws, char *message, size_t length, uWS::OpCode opCode) {
            const char CCH[] = {'C', 31, 'C', 'H', 30};
            const char CCHR[] = {'C', 31, 'C', 'H', 'R', 31, 'a', 'd', 'd', 'r', 30};
            const char CA[] = {'C', 31, 'A', 30};
            const char AREQ[] = {'A', 31, 'R', 'E', 'Q', 31, '{', '}', 30};
            const char AA[] = {'A', 31, 'A', 30};
            const char ES[] = {'E', 31, 'S', 31, 'e', 'v', 'e', 'n', 't', 'N', 'a', 'm', 'e', 30};

            if (!strncmp(CCH, message, std::min<int>(sizeof(CCH), length))) {
                ws.send(CCHR, sizeof(CCHR), uWS::OpCode::TEXT);
            } else if (!strncmp(CA, message, std::min<int>(sizeof(CA), length))) {
                ws.send(AREQ, sizeof(AREQ), uWS::OpCode::TEXT);
            } else if (!strncmp(AA, message, std::min<int>(sizeof(AA), length))) {
                ws.send(ES, sizeof(ES), uWS::OpCode::TEXT);
            } else if (length == 16) {
                sockets.push_back(ws);
                if (++logins == connections) {
                    beginIterating();
                } else if (logins + batchConnect <= connections) {
                    nextConnection();
                }
            } else if (length == 5) {
                sendPong(ws);
            }
        });
        break;
    }

    hub.onError([](void *user) {
        nextConnection();
    });

    for (int i = 0; i < batchConnect; i++) {
        nextConnection();
    }
    hub.run();
}

