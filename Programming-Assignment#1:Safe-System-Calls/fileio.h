///////////////////////////////////////////////////////////////////////
// Intentionally flawed system call library that implements          //
// "safe" (not) file I/O, "preventing" writing "MZ" at the beginning //
// of a file.                                                        //
//                                                                   //
// Written by Golden G. Richard III (@nolaforensix), 7/2017          //
//                                                                   //
// Props to Brian Hay for a similar exercise he used in a recent     //
// training.                                                        //
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// filesystem error code
typedef enum  {
  NONE, 
  OPEN_FAILED,
  CLOSE_FAILED,
  READ_FAILED,
  WRITE_FAILED,
  ILLEGAL_MZ
} FSError;

typedef struct _FileInternal FileInternal;

//file handle type
typedef FileInternal* File;                

// seek anchors
typedef enum {
  BEGINNING_OF_FILE,
  CURRENT_POSITION, 
  END_OF_FILE
} SeekAnchor;

// function prototypes for system calls

// open file with pathname 'name'. Files are always opened for
// read/write access.  Always sets 'fserror' global.
File open_file(char *name);

// close file with handle 'file'.  Always sets 'fserror' global.
void close_file(File file);

// read 'num_bytes' into 'data' from 'file' at 'offset' bytes from a
// particular 'start'-ing position.  Returns the number of bytes
// read. Always sets 'fserror' global.
unsigned long read_file_from(File file, void *data, unsigned long num_bytes,
			     SeekAnchor start, long offset);

// write 'num_bytes' from 'data' into 'file' at 'offset' bytes from a 
// particular 'start'-ing position. Returns the number of bytes
// written.  Always sets 'fserror' global.
unsigned long write_file_at(File file, void *data, unsigned long num_bytes, 
			    SeekAnchor start, long offset);

// describe current filesystem error code 
void fs_print_error(void);

// GLOBALS //

// filesystem error code set (set by each function)
extern FSError fserror;

