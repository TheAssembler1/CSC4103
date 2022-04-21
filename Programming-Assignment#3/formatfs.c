#include "filesystem.h"

int main(){
    if(!(init_software_disk()))
        sd_print_error();

    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE);

    //getting bitmap size in bytes and blocks
    unsigned int bitmap_size_blocks = get_bitmap_size_blocks();
    //getting file descriptors size in bytes and blocks
    unsigned int fd_size_blocks = get_file_descriptors_size_blocks();
    //getting the fat table size in bytes and blocks
    unsigned int fat_table_blocks = get_fat_table_size_blocks();

    //getting blocks needed to set in bitmap
    unsigned int total_blocks = bitmap_size_blocks + fd_size_blocks + fat_table_blocks;

    //FIXME::Remove this
    printf("Bitmap blocks used: %u\n", bitmap_size_blocks);
    printf("File descriptors blocks used: %u\n", fd_size_blocks);
    printf("Fat table blocks used: %u\n", fat_table_blocks);
    printf("Starting blocks used: %u\n", total_blocks);

    //setting blocks that are turned to 1 in bitmap
    set_next_bits_of_bitmap(total_blocks);

    //TEST FILE DRIVER HERE
    //FIXME::Remove this
    char* test_file_name = "test";
    char* message = "testing this message";
    
    File file = create_file(test_file_name);

    uint8_t buf[SOFTWARE_DISK_BLOCK_SIZE * 2];
    memset(buf, 0, SOFTWARE_DISK_BLOCK_SIZE * 2);
    buf[SOFTWARE_DISK_BLOCK_SIZE * 2 - 1] = '\0';

    printf("buffer string: %s\n", buf);
    printf("file size: %u\n", file->file_block.file_size);

    close_file(file);
    //delete_file(test_file_name);
    return 0;
}