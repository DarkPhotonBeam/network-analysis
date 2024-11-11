CC := gcc
CCFLAGS := -Wall

all: seq_analysis count_walks

count_walks: count_walks.o
	${CC} count_walks.o -o count_walks

seq_analysis: seq_analysis.o node.o
	${CC} node.o seq_analysis.o -o seq_analysis

node.o: node.c node.h
	${CC} ${CCFLAGS} node.c -c

count_walks.o: count_walks.c
	$(CC) ${CCFLAGS} count_walks.c -c

seq_analysis.o: seq_analysis.c node.h
	$(CC) ${CCFLAGS} seq_analysis.c -c

clean:
	rm seq_analysis
