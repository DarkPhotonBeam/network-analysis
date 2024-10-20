CC := gcc
CARGS := -Wall

all: seq_analysis

seq_analysis: seq_analysis.c
	${CC} ${CARGS} seq_analysis.c -o seq_analysis

clean:
	rm seq_analysis
