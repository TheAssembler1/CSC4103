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
    char* test_file_name1 = "test1";

    File file = create_file(test_file_name);
    File file_1 = create_file(test_file_name1);
    close_file(file_1);
    close_file(file);

    if(file_exists(test_file_name))
        printf("file %s did exist\n", test_file_name);
    else
        printf("file %s did not exist\n", test_file_name);

    if(file_exists(test_file_name1))
        printf("file %s did exist\n", test_file_name1);
    else
        printf("file %s did not exist\n", test_file_name1);

    return 0;
}