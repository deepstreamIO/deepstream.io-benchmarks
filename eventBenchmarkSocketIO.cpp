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
std::vector<uWS::WebSocket<uWS::CLIENT>> sockets;

const char emitMessage[] = {'4', '2', '[', '\"', 'e', 'v', 'e', 'n', 't', 'N', 'a', 'm', 'e', '\"', ']'};
auto preparedMessage = uWS::WebSocket<uWS::CLIENT>::prepareMessage((char *) emitMessage, sizeof(emitMessage), uWS::OpCode::TEXT, false);

std::chrono::high_resolution_clock::time_point start;
int iterations = 0;
int iterationsPerPrint = 10;

void nextConnection() {
    static int addr = 1;
    hub.connect("ws://127.0.0." + std::to_string(addr) + ":3000/socket.io/?EIO=4&transport=websocket", nullptr, 10000);
    if (sockets.size() % 20000 == 0) {
        addr++;
    }
}

void iterate() {
    if (received != 0) {
        if (++iterations % iterationsPerPrint == 0) {
            int ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
            std::cout << (float(ms) / iterationsPerPrint) << ", ";

            hub.getDefaultGroup<uWS::CLIENT>().close();
            return;

            start = std::chrono::high_resolution_clock::now();
        }
    } else {
        start = std::chrono::high_resolution_clock::now();
    }

    received = 0;

    for (int i = 0; i < messages; i++) {
        sockets[rand() % sockets.size()].sendPrepared(preparedMessage);
    }
}

void beginIterating() {
    hub.onMessage([](uWS::WebSocket<uWS::CLIENT> ws, char *message, size_t length, uWS::OpCode opCode) {
        if (length % 15 != 0) {
            if (length == 1 && message[0] == '3') {
                return;
            } else {
                // this should never happen
                std::cout << "Error: Invalid message received!" << std::endl;
                exit(-1);
            }
        }

        received += length / 15;
        if (received == (connections - 1) * messages) {
            iterate();
        }
    });
    iterate();
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "Usage: eventBenchmark numberOfConnections numberOfEmitPublishes" << std::endl;
        return 0;
    } else {
        connections = atoi(argv[1]);
        messages = atoi(argv[2]);
    }

    batchConnect = std::min<int>(100, connections);

    int port = 3000;
    char line[10240] = {};
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

    hub.onMessage([](uWS::WebSocket<uWS::CLIENT> ws, char *message, size_t length, uWS::OpCode opCode) {
        if (length == 2 && message[0] == '4' && message[1] == '0') {
            int enable = 1;
            setsockopt(ws.getPollHandle()->io_watcher.fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));

            sockets.push_back(ws);
            if (++logins == connections) {
                beginIterating();
            } else if (logins + batchConnect <= connections) {
                nextConnection();
            }
        }
    });

    hub.onError([](void *user) {
        nextConnection();
    });

    for (int i = 0; i < batchConnect; i++) {
        nextConnection();
    }
    hub.run();
}
