#include "filesystem.h"

int main(){
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE);

    //getting bitmap size in bytes and blocks
    unsigned int bitmap_size_bytes = software_disk_size() / 8;
    if(software_disk_size() % 8) { bitmap_size_bytes++; }
    unsigned int bitmap_size_blocks = bitmap_size_bytes / SOFTWARE_DISK_BLOCK_SIZE;
    if(bitmap_size_bytes % SOFTWARE_DISK_BLOCK_SIZE) { bitmap_size_blocks++; }

    //getting fat size in bytes and blocks
    unsigned int fat_size_bytes = sizeof(FileBlockInternals);

    set_bits_of_buffer(buffer, bitmap_size_blocks);

    return 0;
}