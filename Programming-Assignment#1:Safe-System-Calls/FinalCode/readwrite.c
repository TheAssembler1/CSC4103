#include <stdio.h>
#include "fileio.h"

//NOTE::Wrapper around opening a file
File open_the_file();
//NOTE::Wrapper around closing the file
void close_the_file(File f);

//NOTE::List of the error test
void test_1();
void test_2();
void test_3();
void test_4();
void test_5();
void test_6();
void test_7();

//NOTE::List of non error test
void test_8();
void test_9();
void test_10();
void test_11();

int main(int argc, char *argv[]) {
  printf("INFO::Running test 1---------------------\n");
  test_1();
  printf("INFO::Finished test 1--------------------\n\n");

  printf("INFO::Running test 2---------------------\n");
  test_2();
  printf("INFO::Finished test 2--------------------\n\n");

  printf("INFO::Running test 3---------------------\n");
  test_3();
  printf("INFO::Finished test 3--------------------\n\n");

  printf("INFO::Running test 4---------------------\n");
  test_4();
  printf("INFO::Finished test 4--------------------\n\n");

  printf("INFO::Running test 5---------------------\n");
  test_5();
  printf("INFO::Finished test 5--------------------\n\n");

  printf("INFO::Running test 6---------------------\n");
  test_6();
  printf("INFO::Finished test 6--------------------\n\n");

  printf("INFO::Running test 7---------------------\n");
  test_7();
  printf("INFO::Finished test 7--------------------\n\n");

  printf("INFO::Running test 8---------------------\n");
  test_8();
  printf("INFO::Finished test 8--------------------\n\n");

  printf("INFO::Running test 9---------------------\n");
  test_9();
  printf("INFO::Finished test 9--------------------\n\n");

  printf("INFO::Running test 10---------------------\n");
  test_10();
  printf("INFO::Finished test 10--------------------\n\n");

  printf("INFO::Running test 11---------------------\n");
  test_11();
  printf("INFO::Finished test 11--------------------\n\n");
  return 0;
}

void test_1(){
  File f = open_the_file();
  write_file_at(f, "MZ", 2, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  printf("INFO::Should get an error\n");
  close_file(f);
}

void test_2(){
  File f = open_the_file();
  write_file_at(f, "M", 1, CURRENT_POSITION, 0L);
  write_file_at(f, "Z", 1, CURRENT_POSITION, 0L);
  fs_print_error();
  printf("INFO::Should get an error\n");
  close_file(f);
}

void test_3(){
  File f = open_the_file();
  write_file_at(f, "M", 1, BEGINNING_OF_FILE, 0L);
  write_file_at(f, "Z", 1, CURRENT_POSITION, 0L);
  fs_print_error();
  printf("INFO::Should get an error\n");
  close_file(f);
}

void test_4(){
  File f = open_the_file();
  write_file_at(f, "M", 1, BEGINNING_OF_FILE, 0L);
  write_file_at(f, "Z", 1, BEGINNING_OF_FILE, 1L);
  fs_print_error();
  printf("INFO::Should get an error\n");
  close_file(f);
}

void test_5(){
  File f = open_the_file();
  write_file_at(f, "M", 1, CURRENT_POSITION, 0L);
  write_file_at(f, "Z", 1, CURRENT_POSITION, 1L);
  write_file_at(f, "Z", 1, BEGINNING_OF_FILE, 1L);
  fs_print_error();
  printf("INFO::Should get an error\n");
  close_file(f);
}

void test_6(){
  File f = open_the_file();
  write_file_at(f, "   ", 3, BEGINNING_OF_FILE, 0L);
  write_file_at(f, "MZ", 2, CURRENT_POSITION, -3);
  fs_print_error();
  printf("INFO::Should get an error\n");
  close_file(f);
}

void test_7(){
  File f = open_the_file();
  write_file_at(f, "   ", 3, BEGINNING_OF_FILE, 0L);
  write_file_at(f, "MZ", 2, END_OF_FILE, -3);
  fs_print_error();
  printf("INFO::Should get an error\n");
  close_file(f);
}

void test_8(){
  File f = open_the_file();
  write_file_at(f, "M", 1, BEGINNING_OF_FILE, 0L);
  write_file_at(f, " ", 1, CURRENT_POSITION, 0L);
  write_file_at(f, "Z", 1, CURRENT_POSITION, 0L);
  fs_print_error();
  printf("INFO::Should NOT get an error\n");
  close_file(f);
}

void test_9(){
  File f = open_the_file();
  write_file_at(f, "M", 1, BEGINNING_OF_FILE, 0L);
  write_file_at(f, " ", 1, CURRENT_POSITION, 0L);
  write_file_at(f, "Z", 1, CURRENT_POSITION, -2L);
  fs_print_error();
  printf("INFO::Should NOT get an error\n");
  close_file(f);
}

void test_10(){
  File f = open_the_file();
  write_file_at(f, " MZ", 3, BEGINNING_OF_FILE, 0L);
  fs_print_error();
  printf("INFO::Should NOT get an error\n");
  close_file(f);
}

void test_11(){
  File f = open_the_file();
  write_file_at(f, "   ", 3, BEGINNING_OF_FILE, 0L);
  write_file_at(f, " MZ", 3, END_OF_FILE, -3);
  fs_print_error();
  printf("INFO::Should NOT get an error\n");
  close_file(f);
}

//NOTE::Wrapper around opening a file
File open_the_file(){
  File f;
  system("rm important.dat");
  printf("Creating new file called \"important.dat\"...\n");
  f=open_file("important.dat");
  fs_print_error();

  return f;
}

//NOTE::Wrapper around closing the file
void close_the_file(File f){
  printf("Closing file...\n");
  close_file(f);
  fs_print_error();
}
