#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "transfer.h"

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
#endif
// ... (Socket headers same as Receiver) ...

void send_file(const char *ip, const char *filepath) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    FILE *fp = fopen(filepath, "rb");
    
    if (!fp) { perror("File error"); return; }

    // Get File Size (64-bit)
    fseeko(fp, 0, SEEK_END);
    uint64_t fsize = ftello(fp);
    fseeko(fp, 0, SEEK_SET);

    // Setup Socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed. Check if Receiver is running.\n");
        return;
    }

    // 1. Send Metadata
    FileMetadata meta;
    strcpy(meta.filename, filepath);
    meta.filesize = fsize;
    send(sock, (char*)&meta, sizeof(FileMetadata), 0);

    // 2. Stream File in Chunks (Resource Economy)
    char *buffer = malloc(BUFFER_SIZE);
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        if (send(sock, buffer, bytes_read, 0) < 0) {
            fprintf(stderr, "Error: Data transmission failed.\n");
            break;
        }
    }

    printf("[+] File sent successfully.\n");
    fclose(fp);
    free(buffer);
    // ... Close socket logic ...
}

int main(int argc, char *argv[]) {
    if(argc < 3) { printf("Usage: ./sender <IP> <File>\n"); return 1; }
    send_file(argv[1], argv[2]);
    return 0;
}
