
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX     128

#define MIN(x,y) ((x<y)?x:y)

typedef struct node {
	int val[MAX];
	struct node *next;
};

typedef struct vertex {
	int n;               // vertex id
	int n_child;         // no of children
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
int two_elms_bucket[MAX][MAX];
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

int cnt_V[MAX];
int cnt_E[MAX][MAX];
int cnt_V_sum;
int cnt_E_sum;

void DFS_VISIT(struct vertex *G, struct vertex *u, int N) {
	struct node *a;
	struct vertex *v;                          // Vertext U  |  Vertext V   |  Edge U-V
	time ++;                                   cnt_V[u->n]++;
	u->low = time;                             cnt_V[u->n]++;
	u->d = time;                               cnt_V[u->n]++;
	u->color = GRAY;                           cnt_V[u->n]++;
	a = u->adj;                                cnt_V[u->n]++;
	while (a) {
		v = &G[a->val[0]];                                    cnt_V[v->n]++; 
		if (v->color == WHITE) {               cnt_V[u->n]++; cnt_V[v->n]++; cnt_E[u->n][v->n]++;
			push_edge(u->n, v->n);             // for Biconnected component
			u->n_child ++;                     cnt_V[u->n]++;
			v->pi = u;                                        cnt_V[v->n]++;
			DFS_VISIT(G, v, N);                cnt_V[u->n]++;
			u->low = MIN(u->low, v->low);      cnt_V[u->n]++; cnt_V[v->n]++; cnt_E[u->n][v->n]++;
			                                   cnt_V[u->n]++;
			if (u->d == 1) {                 
				                               cnt_V[u->n]++;
				if (u->n_child >= 2) {       
					add_articu(u->n);          // for printing Articulation point
					add_bridge(u->n, v->n);    // for printing Bridge
				}
				                               cnt_V[u->n]++;
				if (u->n_child <= 2) {       
					add_biconn(u->n, v->n);    // for printing Biconnected component
				}
			} else {
				                               cnt_V[u->n]++; cnt_V[v->n]++; cnt_E[u->n][v->n]++;
				if (v->low > u->d) {         
					add_articu(u->n);          // for printing Articulation point
					add_bridge(u->n, v->n);    // for printing Bridge
					add_biconn(u->n, v->n);    // for printing Biconnected component
				} else {
					                           cnt_V[u->n]++; cnt_V[v->n]++; cnt_E[u->n][v->n]++;
					if (v->low == u->d) {
						add_articu(u->n);
						add_biconn(u->n, v->n);// for printing Biconnected component
					}
				}
			}
			u->low = MIN(u->low, v->low);      cnt_V[u->n]++; cnt_V[v->n]++; cnt_E[u->n][v->n]++;
		} else {
			                                   cnt_V[u->n]++; cnt_V[v->n]++; cnt_E[u->n][v->n]++;
											   cnt_V[u->n]++; cnt_V[v->n]++; cnt_E[u->n][v->n]++;
			if ((v != u->pi) && (v->n < u->n)) {
				// back edge found
				push_edge(u->n, v->n);         // for printing Biconnected component
				u->low = MIN(u->low, v->d);    cnt_V[u->n]++; cnt_V[v->n]++; cnt_E[u->n][v->n]++;
			}
		}
		a = a->next;                                          cnt_V[v->n]++;
	}
	u->color = BLACK;                          cnt_V[u->n]++; 
	time ++;                                   cnt_V[u->n]++; 
	u->f = time;                               cnt_V[u->n]++; 
}

void DFS(struct vertex *G, int N) {
	int n;
	for (n=1; n<=N; n++) {
		G[n].color = WHITE;                    cnt_V[n]++; 
		G[n].pi = NULL;                        cnt_V[n]++; 
	}
	time = 0;
											   cnt_V[n]+=1; // n=1
	for (n=1; n<=N; n++) {                     cnt_V[n]+=2; // n<=N, n++
		if (G[n].color == WHITE)               cnt_V[n]++; 
			DFS_VISIT(G, &G[n], N);            cnt_V[n]++; 
	}
}

void main(int argc, char **argv) 
{
	struct vertex G[120];
	struct node **cur;
	FILE *inputFile;
	int N = 0, result, v, i, j, k, h, n, bi_no, id, *ptr;
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

	////////////////////////////////////////////// (above is for parsing input)

	// perform DFS to get everything ready
	DFS(G, N);

	////////////////////////////////////////////// (below is for printing ordered output)

	// print Articulation Points
	printf("Articulation Points:\n");
	for (i=0; i<MAX; i++) {
		if (articu[i].val[0])
			printf("%d ", i);
	}
	printf("\n");
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
	printf("\n");

	// print biconnected-component
	printf("Biconnected-Component:\n");
	for (i=0; i<MAX; i++) {
		for (j=0; j<MAX; j++) {
			if (bi_bucket[i][j] > 0) {
				int k = 0;
				while (biconn[j][k]) {
					k ++;
				}
				biconn[j][k] = i; // kth "biconnected comp with smallest vertex j" is at bi_bucket[i] 
				break;
			}
		}
	}
	for (i=0; i<MAX; i++) {
		for (j=0; j<MAX; j++) {
			if (bi_bucket[i][j] > 0) {
				bi_bucket[i][0] ++; // num of vertex
			}
		}
	}
	for (i=0; i<MAX; i++) {
		if (bi_bucket[i][0] == 2) {
			k = 1; while (!bi_bucket[i][k]) {k++;};
			h = k+1; while (!bi_bucket[i][h]) {h++;};
			two_elms_bucket[k][h] = 1;
			two_elms_bucket[k][0] ++; // number of biconnected comp shared same vertex
		}
	}
	for (i=0; i<MAX; i++) {
		for (k=0; k<MAX; k++) {
			if (biconn[i][k]) {
				if (bi_bucket[biconn[i][k]][0] == 2) {
					h = 1; while (!two_elms_bucket[i][h]) {h++;}
					two_elms_bucket[i][h] = 0;
					two_elms_bucket[i][0] --;
					printf("%d %d\n", i, h);
				} else {
					int found = 0;
					for (j=1; j<MAX; j++) {
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
	}
	printf("\n");

	// get file name id
	k = 0;
	while (argv[1][k] != '\0') {
        k++;
    }
	id = argv[1][k-4]-'0';

	// print Vertex & Edge charge information
	for (i=0; i<MAX; i++) {
		if (cnt_V[i]) {
			cnt_V_sum += cnt_V[i];
			if (id==1) printf("Vertex %d: total %d operations(C commands) charged to Vertex %d in unary.\n", i, cnt_V[i], i);
		}
	}
	for (i=0; i<MAX; i++) {
		for (j=0; j<MAX; j++) {
			if (i==j) continue;
			if (cnt_E[i][j]) {
				int sum = cnt_E[i][j] + cnt_E[j][i];
				int s = (i<j) ? i:j;
				int l = (i<j) ? j:i;
				if (id==1) printf("Edge (%d, %d): total %d operations(C commands) charged to Edge (%d, %d) in unary.\n", s, l, sum, s, l);
				cnt_E[i][j] = 0;
				cnt_E[j][i] = 0;
				cnt_E_sum += sum;
			}
		}
	}

	printf("Total number of operations charged to all vertices is: %d\n", cnt_V_sum);
	printf("Total number of operations charged to all edges is: %d\n", cnt_E_sum);
	printf("Total number of operations is: %d", cnt_V_sum + cnt_E_sum);

	{
		char cc;
		scanf("%c", &cc);
	}
}
