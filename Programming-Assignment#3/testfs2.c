#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"

// RUN formatfs before conducting this test!

int main(int argc, char *argv[]) {
  int ret, i, num_supported=0;
  unsigned long len;
  File f;
  char buf[1000], buf2[1000];
  
  // create a bunch of tiny files to see how many are supported
  
  for (i=0; i < 100000; i++) {
    sprintf(buf, "%1d.dat", i);
    f=create_file(buf);
    printf("ret from create_file(\"%s\") = %p\n",
	   buf, f);
    fs_print_error();
    if (! f) {
      break;
    }

    num_supported++;
    
    sprintf(buf, "hello, %1d", i);
    ret=write_file(f, buf, strlen(buf));
    printf("ret from write_file(f, \"%s\", strlen(\"%s\") = %d\n",
	   buf, buf, ret);
    fs_print_error();
    if (! ret) {
      break;
    }

    seek_file(f, 0);
    bzero(buf2, 1000);
    ret=read_file(f, buf2, 1000);
    printf("ret from read_file(f, buf2, 1000) = %d\n",
	   ret);
    printf("buf2=\"%s\"\n", buf2);
    fs_print_error();
    if (! ret) {
      break;
    }

    len=file_length(f);
    printf("ret from file_length(f) = %lu\n",
	   len);
    fs_print_error();
    if (! len) {
      break;
    }
    
    close_file(f);
    printf("Executed close_file(f).\n");
    fs_print_error();
  }

  // now check to see if they all exist

  for (i=0; i < num_supported; i++) {
    sprintf(buf, "%1d.dat", i);
    printf("File \"%s\" %s.\n", buf, file_exists(buf) ? "exists" : "does not exist");
  }
}
