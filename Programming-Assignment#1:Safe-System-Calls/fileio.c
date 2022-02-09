//////////////////////////////////////////////////////////////////////
// Intentionally flawed system call library that implements          //
// (unfortunately, not) "safe" file I/O, "preventing" writing "MZ"   //
// at the beginning of a file.                                       //
//                                                                   //
// Written by Golden G. Richard III (@nolaforensix), 7/2017          //
//                                                                   //
// Props to Brian Hay for a similar exercise he used in a recent     //
// training.                                                        //
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"

//
// GLOBALS
//

FSError fserror;

typedef struct _FileInternal{
    FILE* fp;
    char mem[2];
} FileInternal;

//
// private functions
//

static int seek_file(File file, SeekAnchor start, long offset) {
  if (! file->fp || (start != BEGINNING_OF_FILE && 
	       start != CURRENT_POSITION && start != END_OF_FILE)) {
    return 0;
  }
  else {
    if (! fseek(file->fp, offset, start == BEGINNING_OF_FILE ? SEEK_SET : 
		(start == END_OF_FILE ? SEEK_END : SEEK_CUR))) {
      return 1;
    }
    else {
      return 0;
    }
  }
}

//
// public functions
//

// open or create a file with pathname 'name' and return a File
// handle.  The file is always opened with read/write access. If the
// open operation fails, the global 'fserror' is set to OPEN_FAILED,
// otherwise to NONE.
File open_file(char *name) {
  File file;

  fserror=NONE;
  // try to open existing file
  file->fp=fopen(name, "r+");
  if (! file->fp) {
    // fail, fall back to creation
    file->fp=fopen(name, "w+");
    if (! file->fp) {
      fserror=OPEN_FAILED;
      return NULL;
    }
  }

  // setting mem[] to 0
  file->mem[0] = 0;
  file->mem[1] = 0;

  return file;
}

// close a 'file'.  If the close operation fails, the global 'fserror'
// is set to CLOSE_FAILED, otherwise to NONE.
void close_file(File file) {
  if (file->fp && ! fclose(file->fp)) {
    fserror=NONE;
  }
  else {
    fserror=CLOSE_FAILED;
  }
}

// read at most 'num_bytes' bytes from 'file' into the buffer 'data',
// starting 'offset' bytes from the 'start' position.  The starting
// position is BEGINNING_OF_FILE, CURRENT_POSITION, or END_OF_FILE. If
// the read operation fails, the global 'fserror' is set to READ_FAILED,
// otherwise to NONE.
unsigned long read_file_from(File file, void *data, unsigned long num_bytes, 
			     SeekAnchor start, long offset) {

  unsigned long bytes_read=0L;

  fserror=NONE;
  if (! file->fp || ! seek_file(file, start, offset)) {
    fserror=READ_FAILED;
  }
  else {
    bytes_read=fread(data, 1, num_bytes, file->fp);
    if (ferror(file->fp)) {
      fserror=READ_FAILED;
    }
  }
  return bytes_read;
}
  
// write 'num_bytes' to 'file' from the buffer 'data', starting
// 'offset' bytes from the 'start' position.  The starting position is
// BEGINNING_OF_FILE, CURRENT_POSITION, or END_OF_FILE.  If an attempt
// is made to modify a file such that "MZ" appears in the first two
// bytes of the file, the write operation fails and ILLEGAL_MZ is
// stored in the global 'fserror'.  If the write fails for any other
// reason, the global 'fserror' is set to WRITE_FAILED, otherwise to
// NONE.
unsigned long write_file_at(File file, void *data, unsigned long num_bytes, 
			     SeekAnchor start, long offset) {
  unsigned long bytes_written=0L;
  long int file_abs_pos=offset;

  //setting the absolution position of the write
  if(start == CURRENT_POSITION){
      file_abs_pos += ftell(file->fp);
      printf("CURRENT_POSITION\n");
    }
  else if(start == END_OF_FILE){
      int temp = ftell(file->fp);
      fseek(file->fp, 0L, SEEK_END);
      file_abs_pos += ftell(file->fp); 
      fseek(file->fp, temp, SEEK_SET);
  }

  //setting the char buffer in the file internal structure
  int i = file_abs_pos;
  for(int j = 0; i < 2 && j < num_bytes; j++, i++)
    file->mem[file_abs_pos + j] = *((char*)(data + j));

  printf("___________________\n");
  printf("File abs pos = %lu\n", file_abs_pos);
  printf("Num bytes = %ld\n", num_bytes);
  printf("mem[0] : %c\n", file->mem[0]);
  printf("mem[1] : %c\n", file->mem[1]);
  printf("-------------------\n");

  fserror=NONE;
  if (! file->fp || ! seek_file(file, start, offset)) {
    fserror=WRITE_FAILED;
  } else if(file->mem[0] == 'M' && file->mem[1] == 'Z'){
    fserror = ILLEGAL_MZ;
  }else {
    bytes_written=fwrite(data, 1, num_bytes, file->fp);
    if (bytes_written < num_bytes) {
      fserror=WRITE_FAILED;
    }
  }

  return bytes_written;
}


// print a string representation of the error indicated by the global
// 'fserror'.
void fs_print_error(void) {
  printf("FS ERROR: ");
  switch (fserror) {
  case NONE:
    puts("NONE");
    break;
  case OPEN_FAILED:
    puts("OPEN_FAILED");
    break;
  case CLOSE_FAILED:
    puts("CLOSE_FAILED");
    break;
  case READ_FAILED:
    puts("READ_FAILED");
    break;
  case WRITE_FAILED:
    puts("WRITE_FAILED");
    break;
  case ILLEGAL_MZ:
    puts("ILLEGAL_MZ: SHAME ON YOU!");
  break;
  default:
    puts("** UNKNOWN ERROR **");
  }
}
