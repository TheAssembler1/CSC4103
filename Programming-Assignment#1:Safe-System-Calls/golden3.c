#include <stdio.h>
#include "fileio.h"

int main(int argc, char *argv[]) {
  File f;
  f=open_file("another.dat");
  fs_print_error();
  write_file_at(f, "HELLOTHERE", strlen("HELLOTHERE"), BEGINNING_OF_FILE, 0L);
  fs_print_error();
  write_file_at(f, "Z", 1, END_OF_FILE, -9L);
  fs_print_error();
  write_file_at(f, "M", 1, END_OF_FILE, -10L);
  fs_print_error();
  close_file(f);
  fs_print_error();
  return 0;
}
  

  


