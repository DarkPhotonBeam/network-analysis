#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a < b ? b : a)
#define BOOLTOCHAR(b) (b ? 'y' : 'n')

int is_end(char *c) {
	return (*c == '\n' || *c == EOF);
}

static int is_split_graph = 0;
static int is_threshold_graph = 0;
static int *erdos_eq_ptr = NULL;
static int erdos_eq_cnt = 0;
static int num_edges = 0;
static int *st_cnt = NULL;
static int *dyads = NULL;

int get_conjugate(const int j, const int *d, size_t n) {
	int cnt = 0;
	for (unsigned int i = 1; i <= j-1; ++i) {
		if (d[i-1] >= j-1) ++cnt;
	}
	for (unsigned int i = j+1; i <= n; ++i) {
		if (d[i-1] >= j) ++cnt;
	}
	return cnt;
}

unsigned int get_h(const int *base, size_t n) {
	unsigned int max = 0;
	for (unsigned int i = 0; i < n; ++i) {
		if (base[i] >= i) max = MAX(max, i+1);
	}
	return max;
}

int get_splittance(const int *base, size_t n) {
	unsigned int h = get_h(base, n);
	int sum = h * (h-1);
	for (unsigned int j = h + 1; j <= n; ++j) {
		sum += base[j-1];
	}
	for (unsigned int i = 1; i <= h; ++i) {
		sum -= base[i-1];
	}
	return 0.5 * sum;
}

int get_splittance_conj(const int *d, size_t n) {
	unsigned int h = get_h(d, n);
	int sum = 0;
	for (unsigned int j = 1; j <= h; ++j) {
		sum += get_conjugate(j, d, n) - d[j-1];
	}
	return 0.5 * sum;
}

void print_erdos_eq() {
	printf("k's with Erdos-Gallai equality (math. indexing): ");
	for (int i = 0; i < erdos_eq_cnt; ++i) {
		printf("%d", erdos_eq_ptr[i]);
		if (i < erdos_eq_cnt - 1) printf(", ");
	}
	printf("\n");
}

int is_graphic(const int *base, size_t n) {
	num_edges = 0;
	is_split_graph = 0;
	is_threshold_graph = 0;
	if (erdos_eq_ptr != NULL) free(erdos_eq_ptr);
	erdos_eq_ptr = calloc(n, sizeof(int));
	erdos_eq_cnt = 0;
	int equal = 1;
	for (unsigned int k = 1; k < n; ++k) {
		unsigned int lSum = 0;
		for (unsigned int i = 1; i <= k; ++i) {
			lSum += base[i-1];
		}
		unsigned int rSum = k * (k-1);
		for (unsigned int j = k + 1; j <= n; ++j) {
			rSum += MIN(k, base[j-1]);
		}
		if (k == get_h(base, n)) {
			is_threshold_graph = equal;
			is_split_graph = 1;
		}
		if (lSum != rSum) equal = 0;
		else {
			erdos_eq_ptr[erdos_eq_cnt] = k;
			++erdos_eq_cnt;
		}
		if (lSum > rSum) return 0;
	}
	unsigned int sum = 0;
	for (unsigned int i = 0; i < n; ++i) {
		sum += base[i];
	}
	num_edges = sum / 2;
	return (sum+1) % 2;
}

int comp_ints(const void *a, const void *b) {
	int ai = *((int *)a);
	int bi = *((int *)b);
	return bi - ai;
}

void generate_graphml(const int* d, const int n, const char *name) {
  FILE *f = fopen(name, "w");
  fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(f, "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"\n xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\n http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n");
  fprintf(f, "\t<graph id=\"G\" edgedefault=\"undirected\">\n");
  struct node *node_arr = malloc(n * sizeof(struct node));
  for (unsigned int i = 0; i < n; ++i) {
    node_arr[i].label = i;
    node_arr[i].degree = d[i];
    fprintf(f, "\t\t<node id=\"n%d\"/>\n", i);
  }
  // Algo of Havel, Hakimi
  while (node_arr[0].degree != 0) {
    for (unsigned int i = 1; i <= node_arr[0].degree; ++i) {
      fprintf(f, "\t\t<edge source=\"n%d\" target=\"n%d\"/>\n", node_arr[0].label, node_arr[i].label);
      --node_arr[i].degree;
    }
    node_arr[0].degree = 0;
    qsort(node_arr, n, sizeof(struct node), &nodecmp);
  }
  fprintf(f, "\t</graph>\n");
  fprintf(f, "</graphml>");
  fclose(f);
  free(node_arr);
}

int *calc_dists(const int* d, const int n) {
	int *dist = malloc(sizeof(int) * n * n);
	if (dyads != NULL) {
		free(dyads);
		dyads = NULL;
	}
	dyads = malloc(sizeof(int) * n * n);
	st_cnt = malloc(sizeof(int) * n * n);
	for (unsigned int i = 0; i < n*n; ++i) {
		dist[i] = n*(n-1);
		st_cnt[i] = 0;
		dyads[i] = 0;
	}
	struct node *node_arr = malloc(n * sizeof(struct node));
	for (unsigned int i = 0; i < n; ++i) {
		node_arr[i].label = i;
		node_arr[i].degree = d[i];
		dist[i*n + i] = 0;
		st_cnt[i*n + i] = 1;
	}
	while (node_arr[0].degree != 0) {
		for (unsigned int i = 1; i <= node_arr[0].degree; ++i) {
			int u = node_arr[0].label;
			int v = node_arr[i].label;
			dist[u * n + v] = 1;
			dist[v * n + u] = 1;
			st_cnt[u*n + v] = 1;
			st_cnt[v*n + u] = 1;
			dyads[u*n+v] = 1;
			dyads[v*n+u] = 1;
			--node_arr[i].degree;
		}
		node_arr[0].degree = 0;
		qsort(node_arr, n, sizeof(struct node), &nodecmp);
	}
	for (unsigned int k = 0; k < n; ++k) {
		for (unsigned int i = 0; i < n; ++i) {
			for (unsigned int j = 0; j < n; ++j) {
				if (dist[i*n+j]>(dist[i*n+k]+dist[k*n+j])) {
					dist[i*n+j] = dist[i*n+k] + dist[k*n+j];
					st_cnt[i*n+j] = st_cnt[i*n+k]*st_cnt[k*n+j];
				} else if (dist[i*n+j]==(dist[i*n+k]+dist[k*n+j])) {
					st_cnt[i*n+j]+=st_cnt[i*n+k]*st_cnt[k*n+j];
				}
			}
		}
	}
	free(node_arr);
	return dist;
}

void print_dist(const int* d, const int n, const int gen_adj) {
	int *dist = calc_dists(d, n);
	printf("Distance Matrix (from Havel-Hakimi-constructed graph):\n");
	printf("    ");
	for (int i = 0; i < n; ++i) {
		printf("%3d ", i);
	}
	printf("\n");
	for (int i = 0; i < n; ++i) {
		printf("%3d ", i);
		for (int j = 0; j < n; ++j) {
			int val = dist[i*n + j];
			if (val < 0) val = -1;
			printf("%3d ", val);
		}
		printf("\n");
	}
	printf("Closeness of vertices:\n");
	for (int i = 0; i < n; ++i) {
		int sum = 0;
		for (int j = 0; j < n; ++j) {
			sum += dist[i*n + j];
		}
		float c = (n-1) / ((float)sum);
		printf("C(%d) = %d / %d = %.3f\n", i, n-1, sum, c);
	}
	float *betweenness = malloc(sizeof(float) * n);
	for (int i = 0; i < n; ++i) betweenness[i] = 0.0;
	for (int s = 0; s < n; ++s) {
		for (int t = 0; t < n; ++t) {
			if (s == t) continue;
			for (int v = 0; v < n; ++v) {
				if (v == s || v == t) continue;
				if (dist[s*n+v]+dist[v*n+t]==dist[s*n+t]) {
					betweenness[v] += ((float)st_cnt[s*n+v]*st_cnt[v*n+t]) / st_cnt[s*n+t];
				}
			}
		}
	}
	printf("Betweenness of vertices:\n");
	for (int i = 0; i < n; ++i) {
		printf("C_B(%d) = %3.3f\n", i, betweenness[i]);
	}
	printf("Dyads:\n");
	printf("   ");
	for (int i = 0; i < n; ++i) {
		printf("%2d ", i);
	}
	printf("\n");
	FILE *f = gen_adj ? fopen("graph.adj", "w") : NULL;
	if (f != NULL) fprintf(f, "%d\n", n);
	for (int i = 0; i < n; ++i) {
		printf("%2d ", i);
		for (int j = 0; j < n; ++j) {
			printf("%2d ", dyads[i*n + j]);
			if (f != NULL) fprintf(f, "%d ", dyads[i*n + j]);
		}
		printf("\n");
	}
	if (f != NULL) fclose(f);
	free(dist);
	free(st_cnt);
	free(betweenness);
	free(dyads);
	dyads = NULL;
	st_cnt = NULL;
}

int main(int argc, char *argv[]) {
  	int gen_graphml = argc >= 2 && (!strcmp(argv[1], "-g") || (argc >= 3 && !strcmp(argv[2], "-g")));
	int gen_adj = argc >= 2 && (!strcmp(argv[1], "-adj") || (argc >= 3 && !strcmp(argv[2], "-adj")));

	int arr_size = 10;
	int *el = malloc(arr_size * sizeof(int));
	int num_el = 0;

	printf("Enter a sequence of integers, seperated by spaces.\n");
	int curr;
	while ( scanf("%d", &curr) != EOF ) {
		el[num_el] = curr;
		++num_el;
		if (num_el >= arr_size) {
			arr_size *= 2;
			el = (int *) realloc(el, arr_size * sizeof(int));
		}
		char c = getchar();
		if (is_end(&c)) break;
	}
	printf("You have entered %d numbers ([<label>]: <degree>):\n", num_el);
	qsort(el, num_el, sizeof(int), &comp_ints);
	for (unsigned int i = 0; i < num_el; ++i) {
		printf("[%d]: %d", i, el[i]);
		if (i < num_el - 1) printf(", ");
	}
	printf("\n");
	printf("The entered sequence of numbers is ");
	if (is_graphic(el, num_el)) {
		printf("graphic.\n");
		printf("#vertices: %d\n", num_el);
		printf("#edges: %d\n", num_edges);
		printf("h: %d\n", get_h(el, num_el));
		int splittance = get_splittance(el, num_el);
		printf("splittance: %d\n", splittance);
		printf("splittance (conj): %d\n", get_splittance_conj(el, num_el));
		printf("is split graph: %c\n", BOOLTOCHAR(splittance == 0));
		printf("is threshold graph: %c\n", BOOLTOCHAR(is_threshold_graph));
		print_erdos_eq();
		print_dist(el, num_el, gen_adj);
    if (gen_graphml) {
      generate_graphml(el, num_el, "out.graphml");
      printf("Graph generated and written to ./out.graphml\n");
    }
	}
	else printf("not graphic.\n");
	
	free(el);
	free(erdos_eq_ptr);
	erdos_eq_ptr = NULL;
	return 0;
}
