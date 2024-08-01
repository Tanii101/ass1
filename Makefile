# Makefile

# Compiler and flags
CC = gcc
CFLAGS = -O2 -lm
OMPFLAGS = -fopenmp

# Targets
all: ssi mmi ssomp mmomp

ssi: ssInsertion.c coordReader.c
	$(CC) $(CFLAGS) ssInsertion.c coordReader.c -o ssi.exe

mmi: mmInsertion.c coordReader.c
	$(CC) $(CFLAGS) mmInsertion.c coordReader.c -o mmi.exe

ssomp: ompcInsertion.c coordReader.c
	$(CC) $(CFLAGS) $(OMPFLAGS) ompcInsertion.c coordReader.c -o ssomp.exe

mmomp: ompfInsertion.c coordReader.c
	$(CC) $(CFLAGS) $(OMPFLAGS) ompfInsertion.c coordReader.c -o mmomp.exe

# Clean up
clean:
	rm -f ssi.exe mmi.exe ssomp.exe mmomp.exe
