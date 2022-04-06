#include "filesystem.h"

// open existing file with pathname 'name' and access mode 'mode'.  Current file
// position is set at byte 0.  Returns NULL on error. Always sets 'fserror' global.
File open_file(char *name, FileMode mode){
    return 0;
}

// create and open new file with pathname 'name' and (implied) access
// mode READ_WRITE. The current file position is set at byte 0.
// Returns NULL on error. Always sets 'fserror' global.
File create_file(char *name){
    printf("Create_file with name: %s\n", name);

    File file = malloc(sizeof(struct FileInternals));
    memset(file, 0, sizeof(struct FileInternals));

    memcpy(file->file_block.file_name, name, MAX_FILE_NAME);
    file->file_block.starting_block = 0;
    file->file_block.file_size = 0;

    file->mode = READ_WRITE;
    file->fp = 0;

    //loop through file descriptor blocks
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE);
    for(int file_descriptor_block = get_file_descriptors_start_block(); file_descriptor_block < get_file_descriptors_end_block(); file_descriptor_block++){
        read_sd_block(buffer, file_descriptor_block);

        for(int file_block_ptr = 0; file_block_ptr < SOFTWARE_DISK_BLOCK_SIZE; file_block_ptr += sizeof(struct FileBlock)){
            struct FileBlock* file_block = (struct FileBlock*)&buffer[file_block_ptr];

            //file descriptor is available
            if(file_block->file_name[0] == 0){
                memcpy(&buffer[file_block_ptr], name, strlen(name));

                printf("Found empty file descriptor\n");
                printf("Writing file to block: %d\n", file_descriptor_block);
                printf("Writing file at offset: %d\n", file_block_ptr);

                write_sd_block(buffer, file_descriptor_block);
                return file;
            }
        }
    }

    return NULL;
}

// close 'file'.  Always sets 'fserror' global.
void close_file(File file){
    free(file);
}

// read at most 'numbytes' of data from 'file' into 'buf', starting at the 
// current file position.  Returns the number of bytes read. If end of file is reached,
// then a return value less than 'numbytes' signals this condition. Always sets
// 'fserror' global.
unsigned long read_file(File file, void *buf, unsigned long numbytes){
    return 0;
}

// write 'numbytes' of data from 'buf' into 'file' at the current file position. 
// Returns the number of bytes written. On an out of space error, the return value may be
// less than 'numbytes'.  Always sets 'fserror' global.
unsigned long write_file(File file, void *buf, unsigned long numbytes){
    return 0;
}

// sets current position in file to 'bytepos', always relative to the
// beginning of file.  Seeks past the current end of file should
// extend the file. Returns 1 on success and 0 on failure.  Always
// sets 'fserror' global.
int seek_file(File file, unsigned long bytepos){
    return 0;
}

// returns the current length of the file in bytes. Always sets 'fserror' global.
unsigned long file_length(File file){
    return file->file_block.file_size;
}

// deletes the file named 'name', if it exists. Returns 1 on success, 0 on failure. 
// Always sets 'fserror' global.   
int delete_file(char *name){
    return 0;
}

// determines if a file with 'name' exists and returns 1 if it exists, otherwise 0.
// Always sets 'fserror' global.
int file_exists(char *name){
    //loop through file descriptor blocks
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE);
    for(int file_descriptor_block = get_file_descriptors_start_block(); file_descriptor_block < get_file_descriptors_end_block(); file_descriptor_block++){
        read_sd_block(buffer, file_descriptor_block);

        for(int file_block_ptr = 0; file_block_ptr < SOFTWARE_DISK_BLOCK_SIZE; file_block_ptr += sizeof(struct FileBlock)){
            struct FileBlock* file_block = (struct FileBlock*)&buffer[file_block_ptr];

            //file descriptor with name exists
            if(!memcmp(file_block->file_name, name, strlen(name)))
                return FILE_FOUND;
        }
    }

    return FILE_NOT_FOUND;
}

// describe current filesystem error code by printing a descriptive message to standard
// error.
void fs_print_error(void){

}

//sets n bits of buffer
void set_bits_of_buffer(uint8_t* buffer, unsigned int bits){
  unsigned int bytes_to_set = bits / 8;
  unsigned int bits_remaining = bits % 8;

  for(unsigned int i = 0; i < bytes_to_set; i++)
    buffer[i] = 0xFF;

  for(unsigned int i = 0; i < bits_remaining; i++)
    SET_BIT(buffer[bytes_to_set], i);
}

//print bits of byte
void print_bits_of_byte(uint8_t value){
   for(int bit = 0; bit < 8; bit++){
      printf("%i", value & 0x01);
      value = value >> 1;
   }
}

//returns bytes used by bitmap
static unsigned int get_bitmap_size_bytes(){
    unsigned int bitmap_size_bytes = software_disk_size() / 8;
    if(software_disk_size() % 8) { bitmap_size_bytes++; }

    return bitmap_size_bytes;
}

//returns blocks used by bitmap
unsigned int get_bitmap_size_blocks(){
    unsigned int bitmap_size_bytes = get_bitmap_size_bytes();
    unsigned int bitmap_size_blocks = bitmap_size_bytes / SOFTWARE_DISK_BLOCK_SIZE;
    if(bitmap_size_bytes % SOFTWARE_DISK_BLOCK_SIZE) { bitmap_size_blocks++; }

    return bitmap_size_blocks;   
}

//returns blocks used by file descriptors
unsigned int get_file_descriptors_size_blocks(){
    unsigned int fd_size_bytes = sizeof(struct FileBlock) * MAX_FILES;
    unsigned int fd_size_blocks = fd_size_bytes / SOFTWARE_DISK_BLOCK_SIZE;
    if(fd_size_bytes % SOFTWARE_DISK_BLOCK_SIZE) { fd_size_blocks++; };

    return fd_size_blocks;
}

//returns blocks used by fat table
unsigned int get_fat_table_size_blocks(){
    unsigned int fat_table_bytes = get_bitmap_size_bytes() * 8;
    unsigned int fat_table_blocks = fat_table_bytes / SOFTWARE_DISK_BLOCK_SIZE;
    if(fat_table_bytes % SOFTWARE_DISK_BLOCK_SIZE) { fat_table_blocks++; }

    return fat_table_blocks;
}

//returns starting block of bitmap
unsigned int get_bitmap_start_block(){
    return BITMAP_START_BLOCK;
}

//returns starting block of file descriptors
unsigned int get_file_descriptors_start_block(){
    return get_bitmap_size_blocks();
}

//returns starting block of fat table
unsigned int get_fat_table_start_block(){
    return get_bitmap_size_blocks() + get_file_descriptors_size_blocks();
}

//returns end block of bitmap
unsigned int get_bitmap_end_block(){
    return get_bitmap_size_blocks();
}

//returns end block of file descriptors
unsigned int get_file_descriptors_end_block(){
    return get_file_descriptors_start_block() + get_file_descriptors_size_blocks();
}

//returns end block of fat table
unsigned int get_fat_table_end_block(){
    return get_bitmap_size_blocks() + get_file_descriptors_size_blocks() + get_fat_table_size_blocks();
}