
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
struct node biconn[MAX];
int biconn_order[MAX][MAX];

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
			u->n_child ++;
			v->pi = u;
			DFS_VISIT(G, v, N);
			u->low = min(u->low, v->low);
			if (u->d == 1) {
				if (u->n_child >= 2) {
					articu[u->n].val[0] = 1;
					//printf("Articulation point: %d\n", u->n); 
				}
			} else {
				if (v->low >= u->d) {
					articu[u->n].val[0] = 1;
					//printf("Articulation point: %d\n", u->n); 
				}
			}
			if (u->low != v->low) {
				int small = (u->n <= v->n) ? u->n : v->n;
				int large = (u->n <= v->n) ? v->n : u->n;
				bridge[small].val[0] ++; // use idx 0 to indicate num of values
				bridge[small].val[large] = 1;
				//printf("Bridge: %d,%d\n", u->n, v->n); 
			}
		}
		// find back edge
		if ((v != u->pi) && (v->color == GRAY)) {
			u->low = min(u->low, v->low);
			//printf("Back edge %d to %d found\n", u->n, v->n);
		}
		a = a->next;
	}
	u->color = BLACK;
	time ++;
	u->f = time;
	biconn[u->low].val[0] ++;
	biconn[u->low].val[u->n] = 1;
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

void main(int argc, char **argv) 
{
	struct vertex G[120];
	struct node **cur;
	FILE *inputFile;
	int N = 0, result, v, i, j, k, n, bi_no;
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
	printf("Biconnected-Component:\n");
	bi_no = 0;
	for (i=0; i<MAX; i++) {
		if (biconn[i].val[0]) {
			n = 1; while (!biconn[i].val[n]) {n++;}
			biconn_order[n][0] = i;
			if (biconn[i].val[0] == 1) {
				// find and add shared node from bridge info
				struct node *a; int found = 0;
				a = G[n].adj;	
				while(a) {
					for (j=0; j<MAX; j++) {
						if ((articu[j].val[0]) && (j==a->val[0])) {
							if (found == 0) {
								if (j<n) {
									biconn_order[n][0] = 0;
									if (biconn_order[j][0] == 0) {
										biconn_order[j][0] = i;
									} else {
										k = 1; while (biconn[k].val[0]) {k++;}
										biconn[k].val[0] = 2;
										biconn[k].val[n] = 1;
										biconn[k].val[j] = 1;
										biconn_order[j][1] = k;
									}
								}
								biconn[i].val[0] ++;
								biconn[i].val[j] = 1;
							} else {
								int small = (n<=j) ? n : j;
								k = 1; while (biconn[k].val[0]) {k++;}
								biconn[k].val[0] = 2;
								biconn[k].val[n] = 1;
								biconn[k].val[j] = 1;
								biconn_order[small][found+1] = k;
							}
							found ++;
						}
					}
					a = a->next;
				}
			}
		}
	}
	for (i=0; i<MAX; i++) {
		for (j=0; j<MAX; j++) {
			if (biconn_order[i][j]) {
				for (k=1; k<MAX; k++) {
					if (biconn[biconn_order[i][j]].val[k] > 0)
						printf("%d ", k);
				}
				printf("\n");
			}
		}
	}

	{
		char cc;
		scanf("%c", &cc);
	}
}
