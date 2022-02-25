#include <stdio.h>
#include "fileio.h"

int main(int argc, char *argv[]) {
  File f;
  system("rm important.dat");
  printf("Creating new file called \"important.dat\"...\n");
  f=open_file("important.dat");
  fs_print_error();
  printf("Writing MAZ to beginning of file...\n");
  write_file_at(f, "MAZ", 3, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  printf("Writing MZ to beginning of file...\n");
  write_file_at(f, "MZ", 2, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  printf("Closing file...\n");
  close_file(f);
  fs_print_error();
  return 0;
}
  
