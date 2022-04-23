#include "filesystem.h"

#define BYTE_SIZE 8

FSError fserror = FS_NONE;

//defining all static funcctions

//gets the upper boundary of two integers divided
static unsigned int round_up_division(unsigned int number, unsigned int divisor){
    unsigned int result = number / divisor;
    if(number % divisor) { result++; };
    
    return result;
}

//returns bytes used by bitmap
static unsigned int get_bitmap_size_bytes(){
    return round_up_division((unsigned int)software_disk_size(), BYTE_SIZE);
}

//returns starting block of bitmap
static unsigned int get_bitmap_start_block(){
    return BITMAP_START_BLOCK;
}

//returns starting block of file descriptors
static unsigned int get_file_descriptors_start_block(){
    return get_bitmap_size_blocks();
}

//returns starting block of fat table
static unsigned int get_fat_table_start_block(){
    return get_bitmap_size_blocks() + get_file_descriptors_size_blocks();
}

//returns end block of bitmap
static unsigned int get_bitmap_end_block(){
    return get_bitmap_size_blocks() - 1;
}

//returns end block of file descriptors
static unsigned int get_file_descriptors_end_block(){
    return get_file_descriptors_start_block() + get_file_descriptors_size_blocks() - 1;
}

//returns end block of fat table
static unsigned int get_fat_table_end_block(){
    return get_bitmap_size_blocks() + get_file_descriptors_size_blocks() + get_fat_table_size_blocks() - 1;
}

//clears the bit specified starting at 0 from bitmap
static void clear_bit_bitmap(int bit){
    //loop through bitmap
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE * get_bitmap_size_blocks()];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_bitmap_size_blocks());
    for(int i = get_bitmap_start_block(); i <= get_bitmap_end_block(); i++)
        read_sd_block(&buffer[i * SOFTWARE_DISK_BLOCK_SIZE], i);

    //round up division and subtract by one to account for 0 starting index
    unsigned int actual_bit = 0;
    if(bit)
        actual_bit = round_up_division(bit, BYTE_SIZE) - 1;

    //clearing the bit
    CLEAR_BIT(buffer[actual_bit], bit % 8);

    //writing bitmap back to disk
    for(int i = 0; i < SOFTWARE_DISK_BLOCK_SIZE * get_bitmap_size_blocks(); i += SOFTWARE_DISK_BLOCK_SIZE)
        write_sd_block(&buffer[i], i / SOFTWARE_DISK_BLOCK_SIZE);
}

//gets first unused bit in the bitmap
static unsigned int find_first_unused_bit_bitmap(){
    //loop through bitmap
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE * get_bitmap_size_blocks()];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_bitmap_size_blocks());
    for(int i = get_bitmap_start_block(); i <= get_bitmap_end_block(); i++)
        read_sd_block(&buffer[i * SOFTWARE_DISK_BLOCK_SIZE], i);

    //loop through bitmap and find first free bit
    for(int i = 0; i < get_bitmap_size_bytes(); i++)
        for(int j = 0; j < 8; j++)
            if(!(CHECK_BIT(buffer[i], j)))
                return (i * 8) + j;

    //FIXME::remove this
    printf("ERROR\n");
    return 0;
}

//set the entry in the fat table to the value
static void set_fat_entry(uint32_t entry, uint32_t value){
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks()];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks());

    //reading fat table into buffer
    for(unsigned int fat_block = get_fat_table_start_block(); fat_block <= get_fat_table_end_block(); fat_block++)
        read_sd_block(&buffer[(fat_block - get_fat_table_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], fat_block);

    uint32_t* fat_ptr = (uint32_t*)buffer;
    fat_ptr[entry] = value;

    for(unsigned int fat_block = get_fat_table_start_block(); fat_block <= get_fat_table_end_block(); fat_block++)
        write_sd_block(&buffer[(fat_block - get_fat_table_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], fat_block);
}

//returns the number of blocks used by file
static uint32_t number_of_blocks_of_file(uint32_t start_block){
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks()];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks());

    //reading fat table into buffer
    for(unsigned int fat_block = get_fat_table_start_block(); fat_block <= get_fat_table_end_block(); fat_block++)
        read_sd_block(&buffer[(fat_block - get_fat_table_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], fat_block);

    uint32_t* fat_ptr = (uint32_t*)buffer;

    uint32_t blocks = 1;
    while(fat_ptr[start_block] != 1){
        start_block = fat_ptr[start_block];
        blocks++;
    }

    return blocks;
}

//adds blocks to file
static void add_blocks_to_file(uint32_t start_block, uint32_t blocks){
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks()];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks());

    //reading fat table into buffer
    for(unsigned int fat_block = get_fat_table_start_block(); fat_block <= get_fat_table_end_block(); fat_block++)
        read_sd_block(&buffer[(fat_block - get_fat_table_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], fat_block);

    uint32_t* fat_ptr = (uint32_t*)buffer;

    for(int i = 0; i < blocks; i++){
        //adding it to the bitmap
        unsigned int open_block = find_first_unused_bit_bitmap();
        set_next_bits_of_bitmap(1);

        unsigned int last_block = start_block;
        while(fat_ptr[last_block] != 1)
            last_block = fat_ptr[last_block];

        fat_ptr[last_block] = open_block;
        fat_ptr[open_block] = 1;
    }

    for(unsigned int fat_block = get_fat_table_start_block(); fat_block <= get_fat_table_end_block(); fat_block++)
        write_sd_block(&buffer[(fat_block - get_fat_table_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], fat_block);
}

//gets block correlating to byte, block has to be created already
static uint32_t get_block_of_byte_file(File file, unsigned long byte){
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks()];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks());

    //reading fat table into buffer
    for(unsigned int fat_block = get_fat_table_start_block(); fat_block <= get_fat_table_end_block(); fat_block++)
        read_sd_block(&buffer[(fat_block - get_fat_table_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], fat_block);

    uint32_t* fat_ptr = (uint32_t*)buffer;

    unsigned int block_offset = 0;
    if(byte)
        block_offset = round_up_division(byte, SOFTWARE_DISK_BLOCK_SIZE) - 1;
    if(!(byte % SOFTWARE_DISK_BLOCK_SIZE) && byte) { block_offset++; };

    unsigned int current_block = file->file_block.starting_block;
    for(int i = 0; i < block_offset; i++)
        current_block = fat_ptr[current_block];

    return current_block;
}


//defining all nonstatic functions used in formatfs.c and in filesystem.c

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

//sets next n bits of bitmap
void set_next_bits_of_bitmap(unsigned int bits){
    unsigned int next_unset_bit = find_first_unused_bit_bitmap();
    unsigned int start_byte = next_unset_bit / 8 ;

    //loop through bitmap
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE * get_bitmap_size_blocks()];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_bitmap_size_blocks());
    for(int i = get_bitmap_start_block(); i <= get_bitmap_end_block(); i++)
        read_sd_block(&buffer[i * SOFTWARE_DISK_BLOCK_SIZE], i);

    for(int i = next_unset_bit; i < next_unset_bit + bits; i++){
        SET_BIT(buffer[start_byte], i % 8);
        if(buffer[start_byte] == 0xFF && i)
            start_byte++;
    }

    for(int i = 0; i < SOFTWARE_DISK_BLOCK_SIZE * get_bitmap_size_blocks(); i += SOFTWARE_DISK_BLOCK_SIZE)
        write_sd_block(&buffer[i], i / SOFTWARE_DISK_BLOCK_SIZE);
}

//returns blocks used by fat table
unsigned int get_fat_table_size_blocks(){
    unsigned int fat_table_bytes = get_bitmap_size_bytes() * 8;
    unsigned int fat_table_blocks = fat_table_bytes / SOFTWARE_DISK_BLOCK_SIZE;
    if(fat_table_bytes % SOFTWARE_DISK_BLOCK_SIZE) { fat_table_blocks++; }

    return fat_table_blocks;
}

#define COLUMN_SIZE 25
//prints the contents of the fat table
void print_fat_table(){
    printf("print_fat_table\n");

    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks()];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks());

    //reading fat table into buffer
    for(unsigned int fat_block = get_fat_table_start_block(); fat_block <= get_fat_table_end_block(); fat_block++)
        read_sd_block(&buffer[(fat_block - get_fat_table_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], fat_block);

    uint32_t* fat_ptr = (uint32_t*)buffer;
    for(int i = 0; i < get_bitmap_size_bytes(); i++){
        printf("|0x%-4x|", fat_ptr[i]);

        if(!((i + 1) % COLUMN_SIZE) && i)
            printf("\n");
    }
}

//defining all functions declared in header

// open existing file with pathname 'name' and access mode 'mode'.  Current file
// position is set at byte 0.  Returns NULL on error. Always sets 'fserror' global.
File open_file(char *name, FileMode mode){
    printf("open file with name %s\n", name);

    //loop through file descriptor blocks
    uint8_t file_descriptor_buffer[SOFTWARE_DISK_BLOCK_SIZE * get_file_descriptors_size_blocks()];
    memset(file_descriptor_buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_file_descriptors_size_blocks());
    
    for(int file_descriptor_block = get_file_descriptors_start_block(); file_descriptor_block <= get_file_descriptors_end_block(); file_descriptor_block++)
        read_sd_block(&file_descriptor_buffer[(file_descriptor_block - get_file_descriptors_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], file_descriptor_block);

    struct FileBlock* file_block = (struct FileBlock*)file_descriptor_buffer;

    for(int i = 0; i < MAX_FILES; i++){
        //file descriptor with name exists
        if(!memcmp(file_block[i].file_name, name, strlen(name))){
            File file = malloc(sizeof(struct FileInternals));
            memcpy(&(file->file_block), &file_block[i], sizeof(struct FileBlock));
            file->fp = 0;
            file->mode = mode;

            return file;
        }
    }

    return NULL;
}

// create and open new file with pathname 'name' and (implied) access
// mode READ_WRITE. The current file position is set at byte 0.
// Returns NULL on error. Always sets 'fserror' global.
File create_file(char *name){
    printf("create_file with name: %s\n", name);

    File file = malloc(sizeof(struct FileInternals));
    memset(file, 0, sizeof(struct FileInternals));
    memcpy(file->file_block.file_name, name, strlen(name));

    //adding it to the bitmap
    unsigned int start_block = find_first_unused_bit_bitmap();
    set_next_bits_of_bitmap(1);

    //adding it the fat table
    set_fat_entry(start_block, LAST_BLOCK);

    //setting file attributes
    file->file_block.starting_block = start_block;
    file->file_block.file_size = 0;
    file->mode = READ_WRITE;
    file->fp = 0;

    //loop through file descriptor blocks
    uint8_t file_descriptor_buffer[SOFTWARE_DISK_BLOCK_SIZE * get_file_descriptors_size_blocks()];
    memset(file_descriptor_buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_file_descriptors_size_blocks());
    
    for(int file_descriptor_block = get_file_descriptors_start_block(); file_descriptor_block <= get_file_descriptors_end_block(); file_descriptor_block++)
        read_sd_block(&file_descriptor_buffer[(file_descriptor_block - get_file_descriptors_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], file_descriptor_block);

    struct FileBlock* file_block = (struct FileBlock*)file_descriptor_buffer;

    for(int i = 0; i < MAX_FILES; i++){
        //file descriptor with name exists
        if(file_block[i].file_name[0] == 0){
            memcpy(&file_block[i], &(file->file_block), sizeof(struct FileBlock));

            //updating file descriptors
            //write back the file descriptor
            for(int file_descriptor_block = get_file_descriptors_start_block(); file_descriptor_block <= get_file_descriptors_end_block(); file_descriptor_block++)
                write_sd_block(&file_descriptor_buffer[(file_descriptor_block - get_file_descriptors_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], file_descriptor_block);

            return file;
        }
    }

    return NULL;
}

// close 'file'.  Always sets 'fserror' global.
void close_file(File file){
    printf("close file with name: %s\n", file->file_block.file_name);

    //loop through file descriptor blocks
    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE);
    
    for(int file_descriptor_block = get_file_descriptors_start_block(); file_descriptor_block <= get_file_descriptors_end_block(); file_descriptor_block++){
        read_sd_block(buffer, file_descriptor_block);

        for(int file_block_ptr = 0; file_block_ptr < SOFTWARE_DISK_BLOCK_SIZE; file_block_ptr += sizeof(struct FileBlock)){
            struct FileBlock* file_block = (struct FileBlock*)&buffer[file_block_ptr];

            //file descriptor with name exists
            if(!memcmp(file_block->file_name, file->file_block.file_name, strlen((const char*)file->file_block.file_name))){
                memcpy(file_block, file->file_block.file_name, sizeof(struct FileBlock));
                free(file);

                //writing file descriptor back to disk
                write_sd_block(buffer, file_descriptor_block);

                return;
            }
        }
    }
}

// read at most 'numbytes' of data from 'file' into 'buf', starting at the 
// current file position.  Returns the number of bytes read. If end of file is reached,
// then a return value less than 'numbytes' signals this condition. Always sets
// 'fserror' global.
unsigned long read_file(File file, void *buf, unsigned long numbytes){
    //updating the size of the file
    if(file->fp + numbytes > file->file_block.file_size)
        file->file_block.file_size += (file->fp + numbytes) - file->file_block.file_size;

    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE);
    read_sd_block(buffer, get_block_of_byte_file(file, file->fp));

    //convert void pointer to 8 byte pointer
    uint8_t* dest_buf = (uint8_t*)buf;

    unsigned int current_block = get_block_of_byte_file(file, file->fp);
    unsigned long current_byte = 0;
    for(current_byte = 0; current_byte < numbytes; current_byte++){
        dest_buf[current_byte] = buffer[file->fp % SOFTWARE_DISK_BLOCK_SIZE];

        file->fp++;
        if(!(file->fp % SOFTWARE_DISK_BLOCK_SIZE) || current_byte + 1 == numbytes){
            write_sd_block(buffer, current_block);
            current_block = get_block_of_byte_file(file, file->fp);
            read_sd_block(buffer, current_block);
        }
    }

    return current_byte;
}

// write 'numbytes' of data from 'buf' into 'file' at the current file position. 
// Returns the number of bytes written. On an out of space error, the return value may be
// less than 'numbytes'.  Always sets 'fserror' global.
unsigned long write_file(File file, void *buf, unsigned long numbytes){
    //updating the size of the file
    if(file->fp + numbytes > file->file_block.file_size)
        file->file_block.file_size += (file->fp + numbytes) - file->file_block.file_size;

    uint8_t buffer[SOFTWARE_DISK_BLOCK_SIZE];
    memset(buffer, 0, SOFTWARE_DISK_BLOCK_SIZE);
    read_sd_block(buffer, get_block_of_byte_file(file, file->fp));

    //convert void pointer to 8 byte pointer
    uint8_t* src_buf = (uint8_t*)buf;

    unsigned int current_block = get_block_of_byte_file(file, file->fp);
    unsigned long current_byte = 0;
    for(current_byte = 0; current_byte < numbytes; current_byte++){
        buffer[file->fp % SOFTWARE_DISK_BLOCK_SIZE] = src_buf[current_byte];

        seek_file(file, file->fp + 1);
        if(!(file->fp % SOFTWARE_DISK_BLOCK_SIZE) || current_byte + 1 == numbytes){
            write_sd_block(buffer, current_block);
            current_block = get_block_of_byte_file(file, file->fp);
            read_sd_block(buffer, current_block);
        }
    }

    return current_byte;
}

// sets current position in file to 'bytepos', always relative to the
// beginning of file.  Seeks past the current end of file should
// extend the file. Returns 1 on success and 0 on failure.  Always
// sets 'fserror' global.
int seek_file(File file, unsigned long bytepos){
    unsigned int current_blocks = number_of_blocks_of_file(file->file_block.starting_block);
    unsigned int needed_blocks = 1;

    if(bytepos)
        needed_blocks = round_up_division(bytepos, SOFTWARE_DISK_BLOCK_SIZE);
    if(!(bytepos % SOFTWARE_DISK_BLOCK_SIZE) && bytepos) { needed_blocks++; };

    if(needed_blocks > current_blocks)
        add_blocks_to_file(file->file_block.starting_block, needed_blocks - current_blocks);

    file->fp = bytepos;

    return 1;
}

// returns the current length of the file in bytes. Always sets 'fserror' global.
unsigned long file_length(File file){
    return file->file_block.file_size;
}

// deletes the file named 'name', if it exists. Returns 1 on success, 0 on failure. 
// Always sets 'fserror' global.   
int delete_file(char *name){
    printf("delete file with name %s\n", name);

    //loop through file descriptor blocks
    uint8_t file_descriptor_buffer[SOFTWARE_DISK_BLOCK_SIZE * get_file_descriptors_size_blocks()];
    memset(file_descriptor_buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_file_descriptors_size_blocks());
    
    for(int file_descriptor_block = get_file_descriptors_start_block(); file_descriptor_block <= get_file_descriptors_end_block(); file_descriptor_block++)
        read_sd_block(&file_descriptor_buffer[(file_descriptor_block - get_file_descriptors_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], file_descriptor_block);

    struct FileBlock* file_block = (struct FileBlock*)file_descriptor_buffer;

    for(int i = 0; i < MAX_FILES; i++){
        //file descriptor with name exists
        if(!memcmp(file_block[i].file_name, name, strlen(name))){
            uint8_t fat_table[SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks()];
            memset(fat_table, 0, SOFTWARE_DISK_BLOCK_SIZE * get_fat_table_size_blocks());

            //reading fat table into buffer
            for(unsigned int fat_block = get_fat_table_start_block(); fat_block <= get_fat_table_end_block(); fat_block++)
                read_sd_block(&fat_table[(fat_block - get_fat_table_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], fat_block);

            uint32_t* fat_ptr = (uint32_t*)fat_table;

            uint8_t clear_buffer[SOFTWARE_DISK_BLOCK_SIZE];
            memset(clear_buffer, 0, SOFTWARE_DISK_BLOCK_SIZE);

            uint32_t end_block = file_block[i].starting_block;
            file_block[i].starting_block = 0;
            write_sd_block(clear_buffer, end_block);
            clear_bit_bitmap(end_block);

            while(fat_ptr[end_block] != LAST_BLOCK){
                int32_t temp = end_block;
                end_block = fat_ptr[temp];

                //clearing fat table
                fat_ptr[temp] = 0;
                //clearing block of data
                write_sd_block(clear_buffer, end_block);
                //clearing bitmap table
                clear_bit_bitmap(end_block);
            }
            fat_ptr[end_block] = 0;

            for(unsigned int fat_block = get_fat_table_start_block(); fat_block <= get_fat_table_end_block(); fat_block++)
                write_sd_block(&fat_table[(fat_block - get_fat_table_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], fat_block);

            memset(&file_block[i], 0, sizeof(struct FileBlock));

            //write back the file descriptor
            for(int file_descriptor_block = get_file_descriptors_start_block(); file_descriptor_block <= get_file_descriptors_end_block(); file_descriptor_block++)
                write_sd_block(&file_descriptor_buffer[(file_descriptor_block - get_file_descriptors_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], file_descriptor_block);

            return FILE_EXIST;
        }
    }

    return FILE_DOESNT_EXIST;
}

// determines if a file with 'name' exists and returns 1 if it exists, otherwise 0.
// Always sets 'fserror' global.
int file_exists(char *name){
    //loop through file descriptor blocks
    uint8_t file_descriptor_buffer[SOFTWARE_DISK_BLOCK_SIZE * get_file_descriptors_size_blocks()];
    memset(file_descriptor_buffer, 0, SOFTWARE_DISK_BLOCK_SIZE * get_file_descriptors_size_blocks());
    
    for(int file_descriptor_block = get_file_descriptors_start_block(); file_descriptor_block <= get_file_descriptors_end_block(); file_descriptor_block++)
        read_sd_block(&file_descriptor_buffer[(file_descriptor_block - get_file_descriptors_start_block()) * SOFTWARE_DISK_BLOCK_SIZE], file_descriptor_block);

    struct FileBlock* file_block = (struct FileBlock*)file_descriptor_buffer;

    for(int i = 0; i < MAX_FILES; i++){
        //file descriptor with name exists
        if(!memcmp(file_block[i].file_name, name, strlen(name))){
            return FILE_FOUND;
        }
    }

    return FILE_NOT_FOUND;
}

// describe current filesystem error code by printing a descriptive message to standard
// error.
void fs_print_error(void){
    switch(fserror){
        case FS_NONE: 
            printf("no error\n");                                     
            break;
        case FS_OUT_OF_SPACE: 
            printf("out of space\n");                         
            break;
        case FS_FILE_NOT_OPEN: 
            printf("file not open\n");                       
            break;
        case FS_FILE_OPEN: 
            printf("file open\n");                               
            break;
        case FS_FILE_NOT_FOUND: 
            printf("file not found\n");                     
            break;
        case FS_FILE_READ_ONLY: 
            printf("file read only\n");                     
            break;
        case FS_FILE_ALREADY_EXISTS: 
            printf("file already exist\n");            
            break;
        case FS_EXCEEDS_MAX_FILE_SIZE: 
            printf("file exceeded max file size\n"); 
            break;
        case FS_ILLEGAL_FILENAME: 
            printf("file has illegal filename\n");        
            break;
        case FS_IO_ERROR: 
            printf("io error\n");                                 
            break;
        default:
            printf("error does not exist\n");
            break;
    }
}