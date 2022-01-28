#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

int strSort(const void* str1, const void* str2);

int main(int argc, char* argv[]){
	char charArray[BUFFER_SIZE + 1];
	int strNum;
	
	//NOTE::Prompting user to enter number of strings
	printf("Type number of strings you want sorted:");
	fflush(stdout);
	
	//NOTE::Getting user input and casting to int
	fgets(charArray, BUFFER_SIZE, stdin);
	strNum = atoi(charArray);

	//NOTE::Creating array of char pointers for our future strings
	char** stringArray = malloc(strNum * sizeof(char*));	

	printf("Enter Strings:\n");

	for(int i = 0; i < strNum; i++){
		fgets(charArray, BUFFER_SIZE, stdin);
		int strLength = strlen(charArray);
		
		//NOTE::Getting rid of the newline
		charArray[strLength - 1] = 0;
		
		//NOTE::Allocating and copying the input string into the string array
		stringArray[i] = malloc(strLength * sizeof(char));
		strcpy(stringArray[i], charArray);
	}

	//NOTE::Sorting the strings
	qsort(stringArray, strNum, sizeof(char*), strSort);

	printf("Sorted strings:\n");	
	//NOTE::Printing strings
	for(int i = 0; i < strNum; i++)
		printf("%s\n", stringArray[i]);

	//NOTE::Deallocating memory
	for(int i = 0; i < strNum; i++)
		free(stringArray[i]);
	free(stringArray);

	return 0;
}

int strSort(const void* str1, const void* str2){
	return strcmp(*(const char**)str1, *(const char**)str2);
}
