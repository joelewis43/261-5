CC = gcc
CFLAGS = -g -Wall -std=c99

all : prog spellChecker

prog : main.o hashMap.o
	$(CC) $(CFLAGS) -o $@ $^

spellChecker : spellChecker.o hashMap.o
	$(CC) $(CFLAGS) -o $@ $^

main.o : main.c hashMap.h

hashMap.o : hashMap.h hashMap.c

spellChecker.o : spellChecker.c hashMap.h

.PHONY : clean memCheckTests memCheckProg

memCheckProg :
	valgrind --tool=memcheck --leak-check=yes prog

memCheckSpellChecker :
	valgrind --tool=memcheck --leak-check=yes spellChecker

clean :
	-rm *.o
	-rm prog
	-rm spellChecker
