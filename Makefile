BUILD_DIR=build
CC=gcc 
CFLAGS= -Wall -Wextra -Werror -Wno-missing-field-initializers -o $(BUILD_DIR)/$@ -I include/

default: cpuinfo_parser

cpuinfo_parser: parser.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/parser.o $(BUILD_DIR)/cpuinfo.o

parser.o: src/main.c cpuinfo.o
	$(CC) -c $(CFLAGS) src/main.c

cpuinfo.o: src/cpuinfo.c
	$(CC) -c $(CFLAGS) src/cpuinfo.c

clean:
	rm -f build/*.o build/parser
