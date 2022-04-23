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
    printf("----------------------------------\n");

    //setting blocks that are turned to 1 in bitmap
    set_next_bits_of_bitmap(total_blocks);

    //FIXME::there is a bug in the gettign the most recent nonused block think it needs to be plus one or something

    //TEST FILE DRIVER HERE
    char* file1_name = "file1_name";
    char* file1_message = "0123456789";

    char* file2_name = "file2_name";
    char* file2_message = "file2_message";

    char* file3_name = "file3_name";
    char* file3_message = "file3_message";

    File file1 = create_file(file1_name);
    for(int i = 0; i < SOFTWARE_DISK_BLOCK_SIZE; i++)
        write_file(file1, file1_message, strlen(file1_message));
    close_file(file1);
    /*
    File file2 = create_file(file2_name);
    write_file(file2, file2_message, strlen(file2_message));
    close_file(file2);

    File file3 = create_file(file3_name);
    write_file(file3, file3_message, strlen(file3_message));
    close_file(file3);

    delete_file(file1_name);

    file1 = create_file(file1_name);
    for(int i = 0; i < SOFTWARE_DISK_BLOCK_SIZE; i++)
        write_file(file1, file1_message, strlen(file1_message));
    close_file(file1);

    delete_file(file2_name);
    create_file(file2_name);

    char message[strlen("THIS IS A TEST THAT I CAN READ BACK THIS MESSAGE")];
    memset(message, 0, strlen("THIS IS A TEST THAT I CAN READ BACK THIS MESSAGE"));
    file2->fp = 0;
    write_file(file2, "THIS IS A TEST THAT I CAN READ BACK THIS MESSAGE", strlen("THIS IS A TEST THAT I CAN READ BACK THIS MESSAGE"));
    file2->fp = 0;
    read_file(file2, message, strlen("THIS IS A TEST THAT I CAN READ BACK THIS MESSAGE"));
    close_file(file2);

    printf("printing file2 message: %s\n", message);

    if(FILE_EXIST == file_exists(file1_name))
        printf("%s does exist\n", file1_name);
    else
        printf("%s did not exist\n", file1_name);

    if(FILE_EXIST == file_exists(file2_name))
        printf("%s does exist\n", file2_name);
    else
        printf("%s did not exist\n", file2_name);

    if(FILE_EXIST == file_exists(file3_name))
        printf("%s does exist\n", file3_name);
    else
        printf("%s did not exist\n", file3_name);
    */

    print_fat_table();

    return 0;
}