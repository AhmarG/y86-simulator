CC = gcc 
CFLAGS = -g -Wall -std=c99

yess: main.o dump.o memory.o registers.o tools.o fetchStage.o decodeStage.o \
 executeStage.o memoryStage.o writebackStage.o loader.o
	$(CC) $(CFLAGS) -o yess main.o memory.o dump.o fetchStage.o decodeStage.o \
	 executeStage.o memoryStage.o writebackStage.o loader.o registers.o tools.o 

main.o: main.c tools.h memory.h loader.h dump.h forwarding.h status.h bubbling.h

dump.o: dump.h fetchStage.h decodeStage.h executeStage.h memoryStage.h writebackStage.h registers.h memory.h \
		loader.h status.h 

registers.o: registers.h tools.h

memory.o: memory.h tools.h registers.h

tools.o: tools.h
 
fetchStage.o: fetchStage.h tools.h instructions.h forwarding.h decodeStage.h bubbling.h

decodeStage.o: decodeStage.h tools.h instructions.h forwarding.h status.h bubbling.h

executeStage.o: executeStage.h tools.h instructions.h status.h bubbling.h

memoryStage.o: memoryStage.h tools.h forwarding.h status.h bubbling.h

writebackStage.o: writebackStage.h tools.h forwarding.h dump.h status.h

loader.o: loader.h memory.h tools.h

clean: 
	rm -f *.o 
