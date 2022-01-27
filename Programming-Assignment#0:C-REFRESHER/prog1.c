
//FIXME::Check if the string is greater than the buffer size
//FIXME::Remove unnecessary comments

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024 + 1

int strSort(const void* str1, const void* str2);

int main(int argc, char* argv[]){
	char charArray[BUFFER_SIZE];
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
		if(strLength > BUFFER_SIZE)
			charArray[BUFFER_SIZE - 1] = 0;
		else
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

//NOTE::Function for sorting strings passed to qsort
//NOTE::Const void* str1 is a pointer to out char* 
//NOTE::Se have to cast it to a double pointer then 
//NOTE::Then Dereference to get the char*
int strSort(const void* str1, const void* str2){
	const char* castStr1 = *(const char**)str1;
	const char* castStr2 = *(const char**)str2;

	
	return strcmp(castStr1, castStr2);
}
