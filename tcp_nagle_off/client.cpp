#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <netinet/tcp.h>

#define PORT 8080
#define SERVER_ADDRESS "127.0.0.1"

#define ECHO_TIMES 100000

inline auto GetMicros(){
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}


const char *message = "1asdasdasdasdasdasdasdasdasdasdasdasdasdasdasd";

int main() {
    int sockfd;
    struct sockaddr_in address;
    char buffer[322] = {0};

    // Create a socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return 1;
    }
    int optvalg;
    socklen_t optlen = sizeof(optvalg);
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optvalg, &optlen) == -1) {
        perror("getsockopt failed");
        return -1;
    }
    std::cout << "TCP_NODELAY " << optvalg << std::endl;


//    int optval = 1;
//    if (setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,(char *)&optval,sizeof(optval)) == -1) {
//        perror("setsockopt failed");
//        return -1;
//    }

    if (getsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optvalg, &optlen) == -1) {
        perror("getsockopt failed");
        return -1;
    }
    std::cout << "TCP_NODELAY " << optvalg << std::endl;

    // Set server address and port
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_ADDRESS, &address.sin_addr) <= 0) {
        perror("inet_pton failed");
        return 1;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("connect failed");
        return 1;
    }

    auto t1 = GetMicros();
    for (int i=0; i<ECHO_TIMES; i++) {
        // Send a character '1' to the server
        if (send(sockfd, message, 30, 0) < 0) {
            printf("send failed");
            return 1;
        }

        // Receive a single byte '1' from the server
        auto r = 0;
        while (r < 30){
            auto ttt = recv(sockfd, buffer + r, 30 - r, 0);
            if (ttt < 0) exit(-1);
            r += ttt;
        }
        // std::cout << r << std::endl;

    }

    auto t = GetMicros() - t1;
    auto rate = static_cast<double>(t) / ECHO_TIMES;
    std::cout << "time used for " << ECHO_TIMES << " rounds: " << t << " us. speed: " << rate << " us/op" << std::endl;

    // Close the socket
    close(sockfd);

    return 0;
}
