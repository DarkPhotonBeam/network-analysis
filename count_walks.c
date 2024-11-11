#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILE_SRC "graph.adj"

#define I(n, row, col) (n*row + col) 

// assuming square matrix
void mat_mult(unsigned int n, unsigned int *a, unsigned int *b, unsigned int *target) {
	for (unsigned int row = 0; row < n; ++row) {
		for (unsigned int col = 0; col < n; ++col) {
			target[I(n, row, col)] = 0;
			for (unsigned int i = 0; i < n; ++i) {
				target[I(n, row, col)] += a[I(n, row, i)] * b[I(n, i, col)];
			}
		}
	}
}

void cpy_arr(unsigned int n, unsigned int *src, unsigned int *dest) {
	for (unsigned int i = 0; i < n; ++i) {
		dest[i] = src[i];
	}
}

void print_mat(unsigned int n, unsigned int *mat) {
	for (unsigned int row = 0; row < n; ++row) {
		for (unsigned int col = 0; col < n; ++col) {
			printf("%3u ", mat[I(n, row, col)]);
		}
		printf("\n");
	}
}

int main(int argc, char* argv[]) {
	unsigned int k, n;
	FILE *f = NULL;

	if (argc < 2) {
		fprintf(stderr, "Usage: ./count_walks <number>\n");
		exit(1);
	}

	int inp;

	if ( (inp = atoi(argv[1])) <= 0 ) {
		fprintf(stderr, "Invalid Input.\nUsage: ./count_walks <number>\n");
		exit(1);
	}

	k = (unsigned int) inp;
	
	if ( ( f = fopen(INPUT_FILE_SRC, "r")) == NULL ) {
		fprintf(stderr, "Could not open file.\n");
		exit(1);
	}

	if ( fscanf(f, "%d\n", &n) == EOF ) {
		fprintf(stderr, "Input file malformed.\n");	
		exit(1);
	}

	printf("n: %u, k: %u\n", n, k);
	unsigned int* arr = (unsigned int *) malloc(n * n * sizeof(unsigned int));

	for (int i = 0; i < n*n; ++i) {
		if ( fscanf(f, "%u ", arr+i) == EOF ) {
			fprintf(stderr, "Input file malformed.\n");
			exit(1);
		}
	} 

	//printf("Read dyad matrix:\n");
	//print_mat(n, arr);
	
	unsigned int* mat = (unsigned int *) malloc(n * n * sizeof(unsigned int));
	unsigned int* mat_cpy = (unsigned int *) malloc(n * n * sizeof(unsigned int));

	cpy_arr(n*n, arr, mat);
	for (unsigned int i = 1; i < k; ++i) {
		cpy_arr(n*n, mat, mat_cpy);
		mat_mult(n, mat_cpy, arr, mat);
	}

	print_mat(n, mat);
	
	free(mat_cpy);
	free(mat);
	free(arr);

	return 0;
}
