#include <stdio.h>
#include <string.h>

#define MAXN	12000
#define MAXM	32768

int adjref[MAXM], adjnext[MAXM], adjfree, n;

struct {
	int parent, adj, count;
	int use;	/* how many moves are to be made along vertex-parent edge
			   +1 for each parent->vertex move, -1 for the opposite */
} vert[MAXN];

static int insert(int list, int ref)
{
	int k;

	k = adjfree;
	adjfree = adjnext[adjfree];

	adjref[k] = ref;
	adjnext[k] = list;

	return k;
}

static void release(int list)
{
	int t;

	while (list != 0) {
		t = adjnext[list];
		adjnext[list] = adjfree;
		adjfree = list;
		list = t;
	}
}

/* "Distributes" excess marbles at vertex `v'. */
static void distribute(int v)
{
	static int queue[MAXN], pred[MAXN], qhead, qtail;
	static int found[MAXN], nfound, need;
	int i, j, k;

	/* BFS to find paths to free vertices */

	for (i = 1; i <= n; i++)
		pred[i] = 0;

	queue[0] = v;
	qhead = 0;
	qtail = 1;
	pred[v] = v;

	need = vert[v].count - 1;

	for (nfound = 0; qhead < qtail && nfound < need;) {
		k = queue[qhead++];

		/* check descendents */
		for (i = vert[k].adj; i != 0; i = adjnext[i]) {
			j = adjref[i];
			if (pred[j] != 0)
				continue;

			pred[j] = -1;		/* `go up' */
			queue[qtail++] = j;

			if (vert[j].count == 0) {
				found[nfound] = j;
				if (++nfound >= need) break;
			}
		}

		/* check ancestor */
		if ((j = vert[k].parent) != 0 && pred[j] == 0) {
			pred[j] = k;
			queue[qtail++] = j;

			if (vert[j].count == 0)
				found[nfound++] = j;
		}
	}

	if (nfound > need)
		nfound = need;

	/* Propagate excess marbles along the found paths */

	for (k = 0; k < nfound; k++) {
		i = found[k];
		vert[v].count--;
		vert[i].count++;

		while (i != v) {
			if (pred[i] < 0) {
				/* go up */
				vert[i].use--;     /* vertex->parent move */
				i = vert[i].parent;
			} else {
				/* go down */
				i = pred[i];
				vert[i].use++;     /* parent->vertex move */
			}
		}
	}
}

int main()
{
	int i, r, v, m, d;

	for (adjfree = 1, i = 1; i < MAXM; i++)
		adjnext[i] = i + 1;
	adjnext[MAXM - 1] = 0;

	while (scanf("%d", &n) == 1 && n > 0) {
		memset(vert, '\0', sizeof(vert[0]) * (n + 1));

		for (i = 0; i < n && scanf("%d %d %d", &v, &m, &d) == 3; i++) {
			vert[v].count = m;
			while (d-- > 0 && scanf("%d", &m) == 1) {
				vert[v].adj = insert(vert[v].adj, m);
				vert[m].parent = v;
			}
		}

		for (i = 1; i <= n; i++)
			if (vert[i].count >= 2) distribute(i);

		for (r = 0, i = 1; i <= n; i++)
			r += ((vert[i].use > 0) ? 1 : -1) * vert[i].use;

		printf("%d\n", r);

		for (i = 1; i <= n; i++)
			release(vert[i].adj);
	}

	return 0;
}
