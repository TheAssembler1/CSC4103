#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "softwaredisk.h"

#define MAX_FILES 64
#define MAX_FILE_NAME 56
#define BITMAP_START_BLOCK 0

#define FILE_FOUND 1
#define FILE_NOT_FOUND 0

#define LAST_BLOCK 1

#define SET_BIT(byte,nbit)   ((byte) |=  (1 << (nbit)))
#define CLEAR_BIT(byte,nbit) ((byte) &= ~(1 << (nbit)))
#define CHECK_BIT(byte,nbit) ((byte) &   (1 << (nbit)))

//file type used by user code
typedef struct FileInternals* File;

//part of the file stored on disk
struct FileBlock{
    uint8_t file_name[MAX_FILE_NAME];
    uint32_t starting_block;
    uint32_t file_size;
};

//representation of a file
struct FileInternals {
    struct FileBlock file_block;
    int mode;
    int fp;
};

//representation of the bitmap
struct BitMap{
  uint32_t* blocks;
};

//representation of the fat table
struct FatTable{
  uint32_t* blocks;
};

// access mode for open_file() 
typedef enum {
	READ_ONLY, READ_WRITE
} FileMode;

// error codes set in global 'fserror' by filesystem functions
typedef enum  {
  FS_NONE, 
  FS_OUT_OF_SPACE,         // the operation caused the software disk to fill up
  FS_FILE_NOT_OPEN,  	   // attempted read/write/close/etc. on file that isn’t open
  FS_FILE_OPEN,      	   // file is already open. Concurrent opens are not
                           // supported and neither is deleting a file that is open.
  FS_FILE_NOT_FOUND, 	   // attempted open or delete of file that doesn’t exist
  FS_FILE_READ_ONLY, 	   // attempted write to file opened for READ_ONLY
  FS_FILE_ALREADY_EXISTS,  // attempted creation of file with existing name
  FS_EXCEEDS_MAX_FILE_SIZE,// seek or write would exceed max file size
  FS_ILLEGAL_FILENAME,     // filename begins with a null character
  FS_IO_ERROR              // something really bad happened
} FSError;

// function prototypes for filesystem API

// open existing file with pathname 'name' and access mode 'mode'.  Current file
// position is set at byte 0.  Returns NULL on error. Always sets 'fserror' global.
File open_file(char *name, FileMode mode);


// create and open new file with pathname 'name' and (implied) access
// mode READ_WRITE. The current file position is set at byte 0.
// Returns NULL on error. Always sets 'fserror' global.
File create_file(char *name);


// close 'file'.  Always sets 'fserror' global.
void close_file(File file);

// read at most 'numbytes' of data from 'file' into 'buf', starting at the 
// current file position.  Returns the number of bytes read. If end of file is reached,
// then a return value less than 'numbytes' signals this condition. Always sets
// 'fserror' global.
unsigned long read_file(File file, void *buf, unsigned long numbytes);

// write 'numbytes' of data from 'buf' into 'file' at the current file position. 
// Returns the number of bytes written. On an out of space error, the return value may be
// less than 'numbytes'.  Always sets 'fserror' global.
unsigned long write_file(File file, void *buf, unsigned long numbytes);

// sets current position in file to 'bytepos', always relative to the
// beginning of file.  Seeks past the current end of file should
// extend the file. Returns 1 on success and 0 on failure.  Always
// sets 'fserror' global.
int seek_file(File file, unsigned long bytepos);

// returns the current length of the file in bytes. Always sets 'fserror' global.
unsigned long file_length(File file);

// deletes the file named 'name', if it exists. Returns 1 on success, 0 on failure. 
// Always sets 'fserror' global.   
int delete_file(char *name); 

// determines if a file with 'name' exists and returns 1 if it exists, otherwise 0.
// Always sets 'fserror' global.
int file_exists(char *name);

// describe current filesystem error code by printing a descriptive message to standard
// error.
void fs_print_error(void);

//sets next n bits of bitmap
void set_next_bits_of_bitmap(unsigned int bits);

//print bits of byte
void print_bits_of_byte(uint8_t value);

//returns blocks used by bitmap
unsigned int get_bitmap_size_blocks();

//returns blocks used by file descriptors
unsigned int get_file_descriptors_size_blocks();

//returns blocks used by fat table
unsigned int get_fat_table_size_blocks();

//returns starting block of bitmap
unsigned int get_bitmap_start_block();

//returns starting block of file descriptors
unsigned int get_file_descriptors_start_block();

//returns starting block of fat table
unsigned int get_fat_table_start_block();

//returns end block of bitmap
unsigned int get_bitmap_end_block();

//returns end block of file descriptors
unsigned int get_file_descriptors_end_block();

//returns end block of fat table
unsigned int get_fat_table_end_block();

// filesystem error code set (set by each filesystem function)
extern FSError fserror;

//gets first unused bit in the bitmap
unsigned int find_first_unused_bit_bitmap();

//set then entry in the fat table to the value
void set_fat_entry(uint32_t entry, uint32_t value);

//returns the number of blocks used by file
uint32_t number_of_blocks_of_file(uint32_t start_block);

//adds blocks to file
void add_blocks_to_file(uint32_t start_block, uint32_t blocks);

//gets block correlating to byte, block has to be created already
uint32_t get_block_of_byte_file(File file, unsigned long byte);

void print_fat_table();

void clear_bit_bitmap(int bit);

#endif //FILE_SYSTEM_H