
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX     200

typedef struct node {
	int val[MAX];
	struct node *next;
};

typedef struct vertex {
	int n;               // vertex id (For debugging)
	int n_child;         // no of children
	int idx;             // biconnected componenet index
	int low;
#define WHITE  0
#define GRAY   1
#define BLACK  2
	int color;           // vertex color
	int d;               // discovered time stamp
	int f;               // finished time stamp
	struct vertex *pi;   // parent vertex
	struct node *adj;     // linked list of all adjacent vertexs 
};

int time;
struct node articu[MAX];
struct node bridge[MAX];
int bi_idx = 1;
int bi_bucket[MAX][MAX];
int biconn[MAX][MAX];
int biconn_order[MAX];
int edge[MAX][2];
int edge_top;

void push_edge(int x, int y) {
	int small = (x <= y) ? x : y;
	int large = (x <= y) ? y : x;
	edge[edge_top][0] = small;
	edge[edge_top][1] = large;
	edge_top ++;
}

void pop_edge(int *x, int *y) {
	edge_top --;
	*x = edge[edge_top][0];
	*y = edge[edge_top][1];
	edge[edge_top][0] = 0;
	edge[edge_top][1] = 0;
}

void add_bridge(int x, int y) {
	int small = (x <= y) ? x : y;
	int large = (x <= y) ? y : x;
	bridge[small].val[0] ++; // use idx 0 to indicate num of values
	bridge[small].val[large] = 1;
}

void add_articu(int x) {
	articu[x].val[0] = 1;
}

void add_bi_bucket(int idx, int x, int y) {
	bi_bucket[idx][x] = 1;
	bi_bucket[idx][y] = 1;
}

void add_biconn(int x, int y) {
	int s, l;
	int small = (x <= y) ? x : y;
	int large = (x <= y) ? y : x;

	do {
		pop_edge(&s, &l);
		add_bi_bucket(bi_idx, s, l);
	} while ((small != s) || (large != l));

	bi_idx ++;
}

void DFS_VISIT(struct vertex *G, struct vertex *u, int N) {
	struct node *a = u->adj;
	struct vertex *v;
	time ++;
	u->low = u->d = time;
	//printf("[%2d]:%2d/  \n",u->n,u->d);
	u->color = GRAY;
	while (a) {
		v = &G[a->val[0]];
		if (v->color == WHITE) {
			push_edge(u->n, v->n);           // for connected component
			u->n_child ++;
			v->pi = u;
			DFS_VISIT(G, v, N);
			u->low = min(u->low, v->low);
			if (u->d == 1) {
				if (u->n_child >= 2) {
					add_articu(u->n);
					add_bridge(u->n, v->n);
				}
				if (u->n_child <= 2) {
					add_biconn(u->n, v->n);  // for connected component
				}
			} else {
				if (v->low > u->d) {
					add_articu(u->n);
					add_bridge(u->n, v->n);
					add_biconn(u->n, v->n);  // for connected component
				} else if (v->low == u->d) {
					add_articu(u->n);
					add_biconn(u->n, v->n);  // for connected component
				}
			}
			u->low = min(u->low, v->low);
		} else if ((v != u->pi) && (v->n < u->n)) {
			// back edge found
			u->low = min(u->low, v->d);
			push_edge(u->n, v->n);           // for connected component
		}
		a = a->next;
	}
	u->color = BLACK;
	time ++;
	u->f = time;
	//printf("[%2d]:%2d/%2d low:%d\n",u->n,u->d,u->f,u->low);
}

void DFS(struct vertex *G, int N) {
	int n;
	for (n=1; n<=N; n++) {
		G[n].color = WHITE;
		G[n].pi = NULL;
	}
	time = 0;
	for (n=1; n<=N; n++) {
		if (G[n].color == WHITE)
			DFS_VISIT(G, &G[n], N); 
	}
}

#define ARRAY(a,r,c,p)    ((int *)a + p*r + c)
void col_quick_sort(int *arr, int c, int low, int high)
{
	int pivot,j,temp,i;

	if (low < high) {
		pivot = low;
		i = low;
		j = high;
 
		while (i < j) {
			while ((*ARRAY(arr,i,c,MAX) <= *ARRAY(arr,pivot,c,MAX)) && (i < high)) {
				i++;
			}
			while (*ARRAY(arr,j,c,MAX) > *ARRAY(arr,pivot,c,MAX)) {
				j--;
			}
			if (i<j) { 
				temp = *ARRAY(arr,pivot,c,MAX);
				*ARRAY(arr,i,c,MAX) = *ARRAY(arr,j,c,MAX);
				*ARRAY(arr,j,c,MAX) = temp;
			}
		}
		temp = *ARRAY(arr,pivot,c,MAX);
		*ARRAY(arr,pivot,c,MAX) = *ARRAY(arr,j,c,MAX);
		*ARRAY(arr,j,c,MAX) = temp;
		col_quick_sort(arr, c, low, j-1);
		col_quick_sort(arr, c, j+1, high);
	}
}

int bubble_sort(int *array, int n) {
	int c, d, swap;

	for (c = 0 ; c < ( n - 1 ); c++) {
		for (d = 0 ; d < n - c - 1; d++) {
			if (array[d] > array[d+1]) { /* For decreasing order use < */
				swap       = array[d];
				array[d]   = array[d+1];
				array[d+1] = swap;
			}
		}
	}
}

void main(int argc, char **argv) 
{
	struct vertex G[120];
	struct node **cur;
	FILE *inputFile;
	int N = 0, result, v, i, j, k, h, n, bi_no, *ptr;
	char c;

	if (argc < 2) {
		printf("usage:  main <input file>\n");
		exit(-1);
	}

	// read input file into G
	inputFile = fopen(argv[1], "r");
	while (1) {
		result = fscanf(inputFile, "%d%c", &N, &c);
		if (result == EOF)
			break;
		memset(&G[N], 0, sizeof(struct vertex));
		G[N].n = N;
		cur = &G[N].adj;
		while (1) {
			result = fscanf(inputFile, "%d%c", &v, &c);
			*cur = (struct node *)malloc(sizeof(struct node));
			(*cur)->next = NULL;
			(*cur)->val[0] = v;
			cur = &(*cur)->next;
			if ((result == EOF) || (c == '\n'))
				break;
		}
	}
	fclose(inputFile);

	// perform DFS to label "low" value in each node
	DFS(G, N);

	// print articulation points
	printf("Articulation Points:\n");
	for (i=0; i<MAX; i++) {
		if (articu[i].val[0])
			printf("%d ", i);
	}
	printf("\n");

	// print bridge
	printf("Bridge:\n");
	for (i=0; i<MAX; i++) {
		if (bridge[i].val[0]) {
			for (j=1; j<MAX; j++) {
				if (bridge[i].val[j]) {
					printf("%d %d\n", i, j);
				}
			}
		}
	}

	// print biconnected-component
	//DFS_LABEL_EDGE(G, N);
	printf("Biconnected-Component:\n");

	for (i=0; i<MAX; i++) {
		for (j=0; j<MAX; j++) {
			if (bi_bucket[i][j] > 0) {
				int k = 0;
				while (biconn[j][k]) {
					k ++;
				}
				biconn[j][k] = i;
				break;
			}
		}
	}

	for (i=0; i<MAX; i++) {
		for (k=0; k<MAX; k++) {
			if (biconn[i][k]) {
				int found = 0;
				for (j=0; j<MAX; j++) {
					if (bi_bucket[biconn[i][k]][j] > 0) {
						printf("%d ", j);
						found = 1;
					}
				}
				if (found)
					printf("\n");
			}
		}
	}

	{
		char cc;
		scanf("%c", &cc);
	}
}
