#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"

// RUN formatfs before conducting this test!
  
int main(int argc, char *argv[]) {
  int ret, i;
  File f;
  char buf[512];

  // tests block alignment, along with testfs4b
  
  f=create_file("line-em-up");
  printf("ret from create_file(\"line-em-up\") = %p\n",
	 f);
  fs_print_error();
  
  for (i=0; i < 50; i++) {
    memset(buf, 'A'+i, 512);
    ret = write_file(f, buf, 512);
    printf("ret from write_file(f, buf, 512) = %d\n",
	   ret);
    fs_print_error();
  }
  
  close_file(f);
  printf("Executed close_file(f).\n");
  fs_print_error();
}
