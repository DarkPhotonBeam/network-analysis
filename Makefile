CC := gcc
CCFLAGS := -Wall

all: seq_analysis

seq_analysis: seq_analysis.o node.o
	${CC} node.o seq_analysis.o -o seq_analysis

node.o: node.c node.h
	${CC} ${CCFLAGS} node.c -c

seq_analysis.o: seq_analysis.c node.h
	$(CC) $(CCFLAGS) seq_analysis.c -c

clean:
	rm seq_analysis
