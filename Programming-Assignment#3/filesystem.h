#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "softwaredisk.h"

//sets boundaries of the file system
#define MAX_FILES 64
#define MAX_FILE_NAME 52
#define BITMAP_START_BLOCK 0

//used to set the status on a file
#define FILE_OPENED 1
#define FILE_CLOSED 0

//used to signal read to many bytes from file
#define FILE_READ_WRITE_OVER(x) (x - 1)

//signals wether file deleted successfully
#define DELETE_FILE_SUCESS 1
#define DELETE_FILE_FAIL 0

//used to signal wether file exists or not
#define FILE_EXISTS_SUCCESS 1
#define FILE_EXISTS_FAIL 0

//used to signal wether file length succeeded
#define FILE_LENGTH_FAIL 0

//used to signal wether seek_file succeeded
#define SEEK_FILE_SUCCESS 1
#define SEEK_FILE_FAIL 0

//blocks befored before end of disk
#define DISK_BLOCK_BUFFER 5

//signals last block of file
#define LAST_BLOCK 1

//size of a byte
#define BYTE_SIZE 8

//signals wether finding a bit succeeded
#define FIND_FIRST_UNUSED_BIT_BITMAP_FAIL 0

//signals wether we are out of space
#define WRITE_FILE_FAIL 0

//bitwise operations
#define SET_BIT(byte,nbit)   ((byte) |=  (1 << (nbit)))
#define CLEAR_BIT(byte,nbit) ((byte) &= ~(1 << (nbit)))
#define CHECK_BIT(byte,nbit) ((byte) &   (1 << (nbit)))

//file type used by user code
typedef struct FileInternals* File;

//part of the file stored on disk
struct FileBlock{
    uint8_t file_name[MAX_FILE_NAME];
    uint8_t status;
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

// filesystem error code set (set by each filesystem function)
extern FSError fserror;

//these functions need to be declared here for formatfs.c

//returns blocks used by bitmap
unsigned int get_bitmap_size_blocks();

//returns blocks used by file descriptors
unsigned int get_file_descriptors_size_blocks();

//returns blocks used by fat table
unsigned int get_fat_table_size_blocks();

//prints the contents of the fat table
void print_fat_table();

#endif //FILE_SYSTEM_H