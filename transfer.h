#include <stdint.h>

#define PORT 9000
#define BUFFER_SIZE 1048576 // 1 MB buffer for high throughput

typedef struct {
    char filename[256];
    uint64_t filesize;
    uint32_t checksum; // Simple CRC32 or checksum
} FileMetadata;
