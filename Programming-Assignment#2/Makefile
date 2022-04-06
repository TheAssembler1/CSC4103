GCC = gcc
OUTPUT_EXE = sheduler

C_FILES = scheduler.c prioque.c

INPUT_FOLDER = ./Tests/Input/
OUTPUT_FOLDER = ./Tests/Output/
MY_OUTPUT_FOLDER = ./Tests/MyOutput/

all_tests: test1 test2 test3 test4

.PHONY: test1 test2 test3 test4 clean

test1:
	@echo "Running Test 1--------------------------------------------------------------------------"
	@echo "Compiling Files..."
	gcc -Wall $(C_FILES) -o $(OUTPUT_EXE)
	@echo "Running Exe And Sending Output To File..."
	./$(OUTPUT_EXE) < $(INPUT_FOLDER)test1_input.txt > $(MY_OUTPUT_FOLDER)test1_output.txt
	@echo "Getting Diff Between My Output And Correct Output..."
	-diff $(MY_OUTPUT_FOLDER)test1_output.txt $(OUTPUT_FOLDER)test1_output.txt
	@echo "Finished Running Test 1-----------------------------------------------------------------\n"

test2:
	@echo "Running Test 2--------------------------------------------------------------------------"
	@echo "Compiling Files..."
	gcc -Wall $(C_FILES) -o $(OUTPUT_EXE)
	@echo "Running Exe And Sending Output To File..."
	./$(OUTPUT_EXE) < $(INPUT_FOLDER)test2_input.txt > $(MY_OUTPUT_FOLDER)test2_output.txt
	@echo "Getting Diff Between My Output And Correct Output..."
	-diff $(MY_OUTPUT_FOLDER)test2_output.txt $(OUTPUT_FOLDER)test2_output.txt
	@echo "Finished Running Test 2-----------------------------------------------------------------\n"

test3:
	@echo "Running Test 3--------------------------------------------------------------------------"
	@echo "Compiling Files..."
	gcc -Wall $(C_FILES) -o $(OUTPUT_EXE)
	@echo "Running Exe And Sending Output To File..."
	./$(OUTPUT_EXE) < $(INPUT_FOLDER)test3_input.txt > $(MY_OUTPUT_FOLDER)test3_output.txt
	@echo "Getting Diff Between My Output And Correct Output..."
	-diff $(MY_OUTPUT_FOLDER)test3_output.txt $(OUTPUT_FOLDER)test3_output.txt
	@echo "Finished Running Test 3-----------------------------------------------------------------\n"

test4:
	@echo "Running Test 4--------------------------------------------------------------------------"
	@echo "Compiling Files..."
	gcc -Wall $(C_FILES) -o $(OUTPUT_EXE)
	@echo "Running Exe And Sending Output To File..."
	./$(OUTPUT_EXE) < $(INPUT_FOLDER)test4_input.txt > $(MY_OUTPUT_FOLDER)test4_output.txt
	@echo "Getting Diff Between My Output And Correct Output..."
	-diff $(MY_OUTPUT_FOLDER)test4_output.txt $(OUTPUT_FOLDER)test4_output.txt
	@echo "Finished Running Test 4-----------------------------------------------------------------\n"

clean:
	-rm $(MY_OUTPUT_FOLDER)*
	-rm $(OUTPUT_EXE)
