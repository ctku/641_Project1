
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
//struct vertex graph[MAX];
struct node articu[MAX];
struct node bridge[MAX];


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
					printf("Articulation point: %d\n", u->n); 
				}
			} else {
				if (v->low >= u->d) {
					articu[u->n].val[0] = 1;
					printf("Articulation point: %d\n", u->n); 
				}
			}
			if (u->low != v->low) {
				int small = (u->n <= v->n) ? u->n : v->n;
				int large = (u->n <= v->n) ? v->n : u->n;
				bridge[small].val[0] ++; // use idx 0 to indicate num of values
				bridge[small].val[large] = 1;
				printf("Bridge: %d,%d\n", u->n, v->n); 
			}
		}
		// find back edge
		if ((v != u->pi) && (v->color == GRAY)) {
			u->low = v->low;
			//printf("Back edge %d to %d found\n", u->n, v->n);
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

void main(int argc, char **argv) 
{
	struct vertex G[120];
	struct node **cur;
	FILE *inputFile;
	int N = 0, result, v, i, j;
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
	printf("\nArticulation Points:\n");
	for (i=0; i<MAX; i++) {
		if (articu[i].val[0])
			printf("%d ", i);
	}

	// print bridge
	printf("\nBridge:\n");
	for (i=0; i<MAX; i++) {
		if (bridge[i].val[0]) {
			for (j=1; j<MAX; j++) {
				if (bridge[i].val[j])
					printf("%d %d\n", i, j);
			}
		}
	}

	// print biconnected-component
	printf("\nBiconnected-Component:\n");



	{
		char cc;
		scanf("%c", &cc);
	}
}
