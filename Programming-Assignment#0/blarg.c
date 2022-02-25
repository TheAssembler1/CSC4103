#include <stdio.h>

#define TOTAL_ELEMENTS ((sizeof(array) / sizeof(array[0])))
int array[] = {1, 3, 15, 19, 0, 43, 12};

int main(int argc, char *argv[]) {
  int i;  

  //NOTE::Need to cast TOTAL_ELEMENTS TO unsigned int for conditionals
  for (i=-1;i <= (signed int)TOTAL_ELEMENTS - 2; i++) {
    printf("Element %d has value %d\n", i+1, array[i+1]);
  }
  
  return 0;
}
