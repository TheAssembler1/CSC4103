#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"

// RUN formatfs before conducting this test!
  
int main(int argc, char *argv[]) {
  int ret, i;
  File f;
  char c='A';
  unsigned long maxlen;
  char *buf, *buf2;

  // see how large a single file can be
  
  f=create_file("superfile");
  printf("ret from create_file(\"superfile\") = %p\n",
	 f);
  fs_print_error();
  
  while (1) {
    ret = write_file(f, &c, 1);
    printf("ret from write_file(f, \"%c\", 1) = %d\n",
	   c, ret);
    fs_print_error();
    c++;
    if (c > 'Z') {
      c='A';
    }

    maxlen=file_length(f);
    printf("File size is %ld\n", maxlen);
    
    if (! ret) {
      printf("breaking out of while loop\n");
      break;
    }
  }

  printf("passed while loop\n");

  close_file(f);
  printf("Executed close_file(f).\n");
  fs_print_error();

  // now create one file of maximum length, with a single write

  f=create_file("superfile-max");
  printf("ret from create_file(\"superfile-max\") = %p\n",
	 f);
  fs_print_error();
  buf=malloc(maxlen+1);
  buf2=malloc(maxlen+1);
  c='A';
  for (i=0; i < maxlen; i++) {
    buf[i]=c;
    c++;
    if (c > 'Z') {
      c='A';
    }
  }

  ret = write_file(f, buf, maxlen);
  printf("ret from write_file(f, buf, %ld) = %d\n",
	 maxlen, ret);
  printf("Seeking to beginning of file.\n");
  seek_file(f, 0);
  fs_print_error();
  ret = read_file(f, buf2, maxlen);
  printf("ret from read_file(f, buf2, %ld) = %d\n",
	 maxlen, ret);
  fs_print_error();
  buf[maxlen]=0;
  printf("buf2:\n");
  puts(buf2);
  
  close_file(f);
  printf("Executed close_file(f).\n");
  fs_print_error();

  printf("Read / write buffers %s.\n",
	 ! memcmp(buf, buf2, maxlen) ? "match" : "don't match");
}
