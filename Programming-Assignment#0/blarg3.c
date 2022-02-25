#include <stdio.h>
#include <stdlib.h>

//NOTE::Main issue with the program was that p++ incremented by 4 because
//NOTE::p++ is like saying p = p + sizeof(int);

int main(int argc, char *argv[]) {

  unsigned long *p=malloc(sizeof(unsigned long));
  *p=0xdeadbeefcafeba00;
  int i;

  // output the individual bytes of the unsigned long value...or not?

  for (i=sizeof(unsigned long) - 1; i >= 0; i--)
    printf("%02x ", (unsigned char)(((char *)p)[i]));
  
  printf("\n");
  return 0;
}
