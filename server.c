#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transfer.h"

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

void start_receiver() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
#endif

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("[*] Waiting for incoming transfer on port %d...\n", PORT);
    client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    // 1. Receive Metadata
    FileMetadata meta;
    recv(client_sock, (char*)&meta, sizeof(FileMetadata), 0);
    printf("[+] Receiving: %s (%llu GB)\n", meta.filename, meta.filesize / (1024*1024*1024));

    // 2. Receive File Data in Chunks
    FILE *fp = fopen("received_output.dat", "wb");
    char *buffer = malloc(BUFFER_SIZE);
    uint64_t total_received = 0;
    int bytes_read;

    while (total_received < meta.filesize) {
        bytes_read = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) break; // Error or connection lost
        fwrite(buffer, 1, bytes_read, fp);
        total_received += bytes_read;
        
        // Progress Reporting
        printf("\rProgress: %.2f%%", (double)total_received / meta.filesize * 100);
        fflush(stdout);
    }

    printf("\n[+] Transfer Complete. Verifying integrity...\n");
    fclose(fp);
    free(buffer);
#ifdef _WIN32
    closesocket(client_sock); WSACleanup();
#else
    close(client_sock);
#endif
}

int main() { start_receiver(); return 0; }
