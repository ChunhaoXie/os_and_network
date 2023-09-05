#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/tcp.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        std::cerr << "Failed to set signal handler for SIGPIPE" << std::endl;
        return 1;
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create a socket file descriptor

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return 1;
    }

    // Set socket options to reuse the socket address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        return 1;
    }

    // Bind the socket to a specific IP address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // Accept incoming connections and handle them
    while (true) {
        printf("accepting\n");
        if ((new_socket = accept4(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen, SOCK_NONBLOCK)) < 0) {
            perror("accept failed");
            return 1;
        }
        int optval = 1;
        if (setsockopt(new_socket,IPPROTO_TCP,TCP_NODELAY,(char *)&optval,sizeof(optval)) == -1) {
            perror("setsockopt failed");
            return -1;
        }

        // Read data from the client and send back the same data
        int bytes_read;
        while (1) {
            bytes_read = read(new_socket, buffer, 30);
            if (bytes_read < 0){
                if (errno == EAGAIN)
                    continue;
                else{
                    perror("read");
                    break;
                }
            }
            else if (bytes_read == 0) {
                // eof
                break;
            }
            if (write(new_socket, buffer, 30) != 30){
                break;
            }
        }

        // Close the connection when the client has finished sending data
        close(new_socket);
    }

    // Close the server socket
    close(server_fd);

    return 0;
}
