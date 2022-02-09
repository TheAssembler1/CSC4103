#include <stdio.h>
#include "fileio.h"

int main(int argc, char *argv[]) {
  File f;
  system("rm important.dat");
  printf("Creating new file called \"important.dat\"...\n");
  f=open_file("important.dat");
  fs_print_error();

  /*
  //______________
  printf("||||||||||||\n");
  printf("M BEG, A END\n");
  write_file_at(f, "M", 1, BEGINNING_OF_FILE, 0L);
  write_file_at(f, "A", 1, END_OF_FILE, 0L);
  fs_print_error();
  printf("||||||||||||\n");
  //______________
  */

  /*
  //______________
  printf("||||||||||||\n");
  printf("M CURRENT, Z CURRENT\n");
  write_file_at(f, "M", 1, CURRENT_POSITION, 0L);
  write_file_at(f, "Z", 1, CURRENT_POSITION, 0L);
  fs_print_error();
  printf("||||||||||||\n");
  //______________
  */

  /*
  printf("||||||||||||\n");
  printf("M BEG, A CURR, Z CURR -1\n");
  write_file_at(f, "M", 1, BEGINNING_OF_FILE, 0L);
  write_file_at(f, "A", 1, CURRENT_POSITION, 0L);
  write_file_at(f, "Z", 1, CURRENT_POSITION, -1L);
  fs_print_error();
  printf("||||||||||||\n");
  */

  /*
  printf("||||||||||||\n");
  printf("M BEG, A CURR, Z CURR -1\n");
  write_file_at(f, "M", 1, BEGINNING_OF_FILE, 0L);
  write_file_at(f, "A", 1, CURRENT_POSITION, 0L);
  write_file_at(f, "Z", 1, END_OF_FILE, 0L);
  fs_print_error();
  printf("||||||||||||\n");
  */

  printf("Closing file...\n");
  close_file(f);
  fs_print_error();
  return 0;
}
