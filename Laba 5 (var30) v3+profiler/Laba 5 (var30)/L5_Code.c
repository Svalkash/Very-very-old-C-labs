#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define N 3 //Max. cache size
#define T 2 //Degree?? Okay, it's 2-3-4... Should be.
typedef struct smallitem {
	int key;
	char *info;
} Item;

typedef struct cacheitem {
	int f; //free
	int d; //deleted
	Item I;
} CItem;

struct treeitem {
	int n;
	Item arr[2 * T - 1];
	struct treeitem *ptr[2 * T];
	//int list;
};

typedef struct treeitem Node;

typedef struct fulltable {
	Node *root;
	CItem cache[N];
} Table;

const char *MENU[] = { "| 0 | Quit.", "| 1 | Add.", "| 2 | Find.", "| 3 | Delete.", "| 4 | Show.", "| 5 | Find max. key.", "| 6 | Load from file.", "| 7 | Show the tree.", "| 8 | Autofill.", "| 9 | Delete ALL.", "| 10 | Launch profiler." };
const int NOP = sizeof(MENU) / sizeof(MENU[0]);
const char *ERRMSG[] = { "Done!", "ERROR: Duplicate key!", "ERROR: Key wasn't found!" };
const char *fname = "loader.txt";
const char *LINE = "________________________________________________________________________________";

//get
void init(Table *ptable);
int getInt(int *add);
int getStr(char **add);
int fgetInt(FILE *f, int *add);
int fgetStr(FILE *f, char **add);
//dialog
int dialog(); 
int D_Add(Table *ptable);
int D_Find(Table *ptable);
int D_Delete(Table *ptable);
int D_Show(Table *ptable);
int D_Maxkey(Table *ptable);
int D_Load(Table *ptable);
int D_STree(Table *ptable);
int D_AF(Table *ptable);
int D_deleteall(Table *ptable);
int D_profiler(Table *ptable);
//operators
int F_insert(Table *ptable, int key, char *info);
int F_search(Table *ptable, int key);
int F_get(Table *ptable, int key, int *cpos);
int F_remove(Table *ptable, int key);
int F_maxkey(Table *ptable, int *cpos);
//operations
int C_insert(Table *ptable, Item nitem);
int T_insert(Table *ptable, Item nitem);
int C_search(Table *ptable, int key);
int T_search(Node *x, int key);
int C_get(Table *ptable, int key, int *cpos);
int T_get(Table *ptable, int key, int *cpos);
int C_remove(Table *ptable, int key);
int T_remove(Table *ptable, int key, int delflag);
int C_print(Table *ptable);
int T_print(Node *x);
int C_maxkey(Table *ptable, int *cpos);
int T_maxkey(Table *ptable, int cmkp, int *cpos);
int T_stree(Node *x, int level);
int F_deleteall(Table *ptable);
int C_deleteall(Table *ptable);
//loading
int fload(Table *ptable);
//filling
void F_af(Table *ptable, int num);
//hash
int hash(int key);
//moving
int tocache(Table *ptable, Item mitem);
int totree(Table *ptable, int pos);
//helpersfortree
void T_split(Node *x, int i);
void T_insert_nonfull(Node *x, Item nitem);
//profiling
int T_deleteall(Node *x);
int T_profiler(int num_el, int num_s);

int hash(int key)
{
	return (key % N);
}

int getInt(int *add) //MODDED. READS \n
{
	int n = 0;
	do
	{
		n = scanf_s("%d", add);
		if (n == 0)
		{
			printf("ERROR: Enter integer.\n");
			scanf_s("%*c[^\n]");
		}
	} while (n == 0);
	if (n > 0) scanf_s("%*c");
	return n < 0 ? 0 : 1;
}

int getStr(char **add) //modified
{
	*add = (char*)malloc(1);
	char buf[81];
	int n, len = 0;
	**add = '\0';
	do {
		n = scanf_s("%80[^\n]", buf, 81);
		if (n < 0) {
			free(*add);
			*add = NULL;
		};
		if (n == 0) scanf_s("%*c");
		if (n > 0) {
			len += strlen(buf);
			*add = (char*)realloc(*add, len + 1);
			strcat_s(*add, len + 1, buf);
		}

	} while (n > 0);
	return n < 0 ? 0 : 1;
}

int fgetInt(FILE *f, int *add) //MODDED. READS \n
{
	int n = 0;
	do
	{
		n = fscanf_s(f, "%d", add);
		if (n == 0) fscanf_s(f, "%*c[^\n]");
	} while (n == 0);
	if (n > 0) fscanf_s(f, "%*c");
	return n < 0 ? 0 : 1;
}

int fgetStr(FILE *f, char **add) //modified
{
	*add = (char*)malloc(1);
	char buf[81];
	int n, len = 0;
	**add = '\0';
	do {
		n = fscanf_s(f, "%80[^\n]", buf, 81);
		if (n < 0) {
			free(*add);
			*add = NULL;
		};
		if (n == 0) fscanf_s(f, "%*c");
		if (n > 0) {
			len += strlen(buf);
			*add = (char*)realloc(*add, len + 1);
			strcat_s(*add, len + 1, buf);
		}

	} while (n > 0);
	return n < 0 ? 0 : 1;
}

int dialog()
{
	int res;
	int i;
	do {
		puts(LINE);
		puts("MENU:");
		for (i = 0; i < NOP; i++) puts(MENU[i]);
		puts(LINE);
		printf("Choose an option: ");
		if (!getInt(&res)) res = 0;
		if (res > NOP || res < 0) printf("Please, choose from given options!\n");
	} while (res > NOP || res < 0);
	puts(LINE);
	return res;
}

int D_deleteall(Table *ptable)
{
	if (!F_deleteall(ptable)) puts("Table is already empty!");
	else puts("Done!");
	return 1;
}

int F_deleteall(Table *ptable)
{
	int cdr, tdr;
	cdr = C_deleteall(ptable);
	tdr = T_deleteall(ptable->root);
	if (!cdr & !tdr) return 0;
	return 1;
}

int C_deleteall(Table *ptable)
{
	int i;
	int d = 0;
	for (i = 0; i < N; i++) {
		if ((!ptable->cache[i].d) && (!ptable->cache[i].f)) {
			free(ptable->cache[i].I.info);
			ptable->cache[i].I.info = NULL;
			d = 1;
		}
		ptable->cache[i].f = 1;
		ptable->cache[i].d = 0;
	}
	return(d);
}

int T_deleteall(Node *x)
{
	int i;
	if (x == NULL) return 0;
	if (x->n == 0) return 0;
	T_deleteall(x->ptr[0]);
	free(x->ptr[0]);
	x->ptr[0] = NULL;
	for (i = 0; i < x->n; i++) {
		free(x->arr[i].info);
		x->arr[i].info = NULL;
		T_deleteall(x->ptr[i + 1]);
		free(x->ptr[i + 1]);
		x->ptr[i + 1] = NULL;
	}
	x->n = 0;
	return 1;
}

int D_profiler(Table *ptable) //I don't need table, it's just for defintion
{
	int el = 0, s = 0, t = 0;
	while (el <= 0) {
		printf("Enter number of elements: ");
		if (!getInt(&el)) return 0;
		if (el <= 0) printf("ERROR: Enter natural number\n");
	}
	while (s <= 0) {
		printf("Enter number of searches: ");
		if (!getInt(&s)) return 0;
		if (s <= 0) printf("ERROR: Enter natural number\n");
	}
	t = T_profiler(el, s);
	printf("Time (in ticks): %d.\n", t);
	return 1;
}

int T_profiler(int num_el, int num_s)
{
	Table B;
	int i, suc = 0;
	int *kfs; //keys for search
	clock_t t1, t2;
	init(&B);
	F_af(&B, num_el + 3);
	kfs = (int*)malloc(num_s * sizeof(int));
	srand(time(NULL) + 60); //experimental
	for (i = 0; i < num_s; i++) kfs[i] = rand() * rand();
	t1 = clock();
	for (i = 0; i < num_s; i++) if (T_search(B.root, kfs[i]) == 1) suc++;
	t2 = clock();
	//printf("Result: %d elements were founded in TREE.\n", suc);
	//printf("Time (in ticks): %d.\n", t2 - t1);
	F_deleteall(&B);
	free(B.root);
	return (t2 - t1);
}

int F_maxkey(Table *ptable, int *cpos)
{
	int rc = 0;
	int cmkp = -1, tmkp = -1;
	rc = C_maxkey(ptable, &cmkp);
	rc += T_maxkey(ptable, cmkp, &tmkp);
	if (tmkp >= 0) *cpos = tmkp;
	else *cpos = cmkp;
	return(rc);
}

int C_maxkey(Table *ptable, int *cpos)
{
	int i;
	*cpos = -1;
	for (i = 0; i < N; i++)
		if ((!ptable->cache[i].f) && (!ptable->cache[i].d))
			if ((*cpos == -1) || (ptable->cache[i].I.key > ptable->cache[*cpos].I.key)) *cpos = i;
	if (*cpos == -1) return 0;
	return 1;
}

int T_maxkey(Table *ptable, int cmkp, int *cpos)
{
	*cpos = -1;
	Node *x = ptable->root;
	if (x->n == 0) return 0;
	while (x->ptr[0] != NULL) x = x->ptr[x->n];
	if ((cmkp == -1) || (x->arr[x->n - 1].key > ptable->cache[cmkp].I.key)) {
		*cpos = tocache(ptable, x->arr[x->n - 1]);
	}
	return 1;
}

int D_Add(Table *ptable)
{
	int rc = 1;
	int key;
	char *info = NULL;
	printf("Enter key: ");
	if (!getInt(&key)) return 0;
	printf("Enter information:\n");
	if (!getStr(&info)) return 0;
	rc = F_insert(ptable, key, info);
	free(info);
	puts(ERRMSG[rc]);
	return 1;
}

int D_Find(Table *ptable)
{
	int key = 0;
	int src = -1;
	int cpos = -1;
	Item copy = { 0, NULL };
	printf("Enter key: ");
	if (!getInt(&key)) return 0;
	src = F_get(ptable, key, &cpos);
	if (src) printf("Key was found! Information:\n%s\n", ptable->cache[cpos].I.info);
	else printf("Key wasn't found!\n");
	return 1;
}

int D_Delete(Table *ptable)
{
	int rc = 1;
	int key = 0;
	printf("Enter key: ");
	if (!getInt(&key)) return 0;
	rc = F_remove(ptable, key);
	puts(ERRMSG[rc]);
	return 1;
}

int D_Show(Table *ptable)
{
	int rc;
	puts("Format: | <Key> | <Info> |");
	puts("Cache table:");
	rc = C_print(ptable);
	if (!rc) printf("Cache is empty!\n");
	puts("Tree:");
	rc = T_print(ptable->root);
	if (!rc) printf("Tree is empty!\n");
	return 1;
}

int C_print(Table *ptable)
{
	int i;
	int fempty = 1;
	for (i = 0; i < N; i++)
		if ((!ptable->cache[i].f) && (!ptable->cache[i].d)) {
			printf("| %d | %s |\n", ptable->cache[i].I.key, ptable->cache[i].I.info);
			fempty = 0;
		}
	if (fempty) return 0;
	return 1;
}

int T_print(Node *x)
{
	int i = 0;
	if (x == NULL) return 0;
	if (x->n == 0) return 0;
	T_print(x->ptr[0]);
	for (i = 0; i < x->n; i++) {
		printf("| %d | %s |\n", x->arr[i].key, x->arr[i].info);
		T_print(x->ptr[i + 1]);
	}
	return 1;
}

int D_Maxkey(Table *ptable)
{
	int cpos;
	int rc = F_maxkey(ptable, &cpos);
	if (!rc) printf("Table is empty!\n");
	else printf("Key: %d, Info:\n%s\n", ptable->cache[cpos].I.key, ptable->cache[cpos].I.info);
	return 1;
}

int D_STree(Table *ptable)
{
	int rc = T_stree(ptable->root, 0);
	if (rc) printf("Tree is empty!\n");
	return 1;
}

int T_stree(Node *x, int level)
{
	int i = 0;
	int t = 0;
	if (x == NULL) return 1;
	if (x->n == 0) return 1;
	for (t = 0; t < level; t++) printf("  ");
	printf("{\n");
	T_stree(x->ptr[0], level + 1);
	for (i = 0; i < x->n; i++) {
		for (t = 0; t < level + 1; t++) printf("  ");
		printf("%d\n", x->arr[i].key);
		T_stree(x->ptr[i + 1], level + 1);
	}
	for (t = 0; t < level; t++) printf("  ");
	printf("}\n");
	return 0;
}

int D_AF(Table *ptable)
{
	int num = 0;
	while (num <= 0) {
		printf("Enter number of elements: ");
		if (!getInt(&num)) return 0;
		if (num <= 0) printf("ERROR: Enter natural number\n");
	}
	F_af(ptable, num);
	puts(ERRMSG[0]);
	return 1;
}

int D_Load(Table *ptable)
{
	if (fload(ptable)) printf("ERROR: File wasn't found\n");
	else printf("Done!\n");
	return 1;
}

int fload(Table *ptable)
{
	FILE *f;
	int key;
	char *info;
	fopen_s(&f, fname, "r+t");
	if (f == NULL) return 1;
	while (fgetInt(f, &key) && fgetStr(f, &info)) {
		F_insert(ptable, key, info);
		info = NULL;
	}
	fclose(f);
	return 0;
}

void F_af(Table *ptable, int num)
{
	int i, k;
	char s[21];
	*s = '\0';
	srand(time(NULL));
	for (i = 0; i < num; ) {
		k = rand() * rand();
		sprintf_s(s, 21, "auto_%d", k);
		if (F_insert(ptable, k, s) == 0) i++;;
		*s = '\0';
	}
}

int T_search(Node *x, int key)
{
	int i = 0;
	if (x == NULL) return 0;
	for (i = 0; (i < x->n) && (key > x->arr[i].key); ++i);
	if (i < x->n) 
		if (x->arr[i].key == key) return 1;
	return(T_search(x->ptr[i], key));
}

int C_search(Table *ptable, int key)
{
	int h = hash(key);
	if ((ptable->cache[h].I.key == key) && (!ptable->cache[h].d)) return 1;
	return 0;
}

int F_search(Table *ptable, int key)
{
	if (C_search(ptable, key)) return 1;
	if (T_search(ptable->root, key)) return 1;
	return 0;
}

int C_get(Table *ptable, int key, int *cpos)
{
	int h = hash(key);
	if ((ptable->cache[h].I.key == key) && (!ptable->cache[h].d)) {
		*cpos = h;
		return 1;
	}
	return 0;
}

int T_get(Table *ptable, int key, int *cpos)
{
	int i = 0;
	int sres = 0;
	Item got;
	Node *x = ptable->root;
	while ((x != NULL) && (!sres)) {
		for (i = 0; (i < x->n) && (key > x->arr[i].key); ++i);
		if (x->arr[i].key == key) {
			got = x->arr[i];
			sres = 1;
		}
		x = x->ptr[i];
	}
	if (!sres) return 0;
	*cpos = tocache(ptable, got);
	return 1;
}

int F_get(Table *ptable, int key, int *cpos)
{
	int src = 0;
	src = C_get(ptable, key, cpos);
	if (src) return 1;
	src = T_get(ptable, key, cpos);
	if (src) return 1;
	return 0;
}

int tocache(Table *ptable, Item mitem)
{
	int inspos = C_insert(ptable, mitem);
	T_remove(ptable, mitem.key, 0);
	return inspos;
}

int totree(Table *ptable, int pos)
{
	T_insert(ptable, ptable->cache[pos].I);
	ptable->cache[pos].d = 1;
	return 0;
}

int C_insert(Table *ptable, Item nitem)
{
	int h = hash(nitem.key);
	if ((!ptable->cache[h].f) && (!ptable->cache[h].d)) totree(ptable, h);
	ptable->cache[h].I = nitem;
	ptable->cache[h].f = 0;
	ptable->cache[h].d = 0;
	return h; //for tocache. if changed, set tocache for hash(key)
}

void T_split(Node *x, int i)
{
	int j;
	Node *y = x->ptr[i];
	Node *z = NULL;
	for (j = x->n - 1; j >= i; j--) {
		x->arr[j + 1] = x->arr[j];
		x->ptr[j + 2] = x->ptr[j + 1];
	}
	z = (Node*)malloc(sizeof(Node));
	//init
	for (j = 0; j < 2 * T; j++) z->ptr[j] = NULL;
	z->ptr[0] = y->ptr[y->n / 2 + 1];
	for (j = y->n / 2 + 1; j < y->n; j++) {
		z->arr[j - (y->n / 2 + 1)] = y->arr[j];
		z->ptr[j - (y->n / 2 + 1) + 1] = y->ptr[j + 1];
	}
	x->ptr[i + 1] = z;
	x->arr[i] = y->arr[y->n / 2];
	x->n++;
	z->n = y->n - (y->n / 2 + 1);
	y->n = y->n / 2;
}

int T_insert(Table *ptable, Item nitem)
{
	Node *r = ptable->root;
	Node *s = NULL;
	int j;
	if (r->n == 0) {
		r->n = 1;
		r->arr[0] = nitem;
		return 0;
	}
	if (r->n == 2 * T - 1)  {
		s = (Node*)malloc(sizeof(Node));
		s->n = 0;
		//init
		for (j = 0; j < 2 * T; j++) s->ptr[j] = NULL;
		s->ptr[0] = r;
		ptable->root = s;
		T_split(s, 0);
		r = s;
	}
	T_insert_nonfull(r, nitem);
	return 0;
}

void T_insert_nonfull(Node *x, Item nitem)
{
	Node *s;
	int i;
	while (x->ptr[0] != NULL) {
		for (i = 0; (i < x->n) && (nitem.key > x->arr[i].key); i++);
		//if (i == x->n) i--;
		s = x->ptr[i];
		if (s->n == 2 * T - 1) {
			T_split(x, i);
			if (nitem.key > x->arr[i].key) s = x->ptr[i + 1];
		}
		x = s;
	}
	for (i = x->n - 1; (i >= 0) && (nitem.key < x->arr[i].key); i--) x->arr[i + 1] = x->arr[i];
	x->arr[i+1] = nitem;
	x->n++;
}

int F_insert(Table *ptable, int key, char *info)
{
	//errmsg 1
	Item nitem;
	nitem.key = key;
	nitem.info = (char*)malloc(strlen(info) + 1);
	*nitem.info = '\0';
	strcpy_s(nitem.info, strlen(info) + 1, info);
	//if (C_search(ptable, key) || T_search(ptable->root, key)) return 1;
	if (F_search(ptable, key)) return 1;
	C_insert(ptable, nitem);
	return 0;
}

int C_remove(Table *ptable, int key)
{
	int h = hash(key);
	if ((!ptable->cache[h].f) && (!ptable->cache[h].d) && (ptable->cache[h].I.key == key)) {
		ptable->cache[h].d = 1;
		free(ptable->cache[h].I.info);
		ptable->cache[h].I.info = NULL;
		return 0;
	}
	return 2;
}

int T_remove(Table *ptable, int key, int delflag)
{
	int i, j;
	char *t;
	Node *x = ptable->root;
	Node *y = x->ptr[0], *z = x->ptr[1];
	Node *p;
	if (y && z) if ((x->n == 1) && (y->n == T - 1) && (z->n == T - 1)) {
		y->arr[T - 1] = x->arr[0];
		y->ptr[T] = z->ptr[0];
		for (i = 0; i < T - 1; i++) {
			y->arr[i + T] = z->arr[i];
			y->ptr[i + T + 1] = z->ptr[i + 1];
		}
		y->n = 2 * T - 1;
		ptable->root = y;
		free(x);
		free(z);
		x = ptable->root;
	}
	if (x->ptr[0]) while (x->ptr[0] != NULL) {
		for (i = 0; (i < x->n) && (key > x->arr[i].key); ++i);
		if ((i == x->n) || (x->arr[i].key != key)) { //can crash
			y = x->ptr[i];
			z = NULL;
			if ((i > 0) && ((y->n) == T - 1) && (x->ptr[i - 1]->n >= T)) {
				z = x->ptr[i - 1];
				for (j = y->n - 1; j >= 0; j--) {
					y->arr[j + 1] = y->arr[j];
					y->ptr[j + 2] = y->ptr[j + 1];
				}
				y->ptr[1] = y->ptr[0];
				y->arr[0] = x->arr[i - 1];
				x->arr[i - 1] = z->arr[z->n - 1];
				y->ptr[0] = z->ptr[z->n];
				y->n++;
				z->n--;
			}
			else if ((i < x->n) && ((y->n) == T - 1) && (x->ptr[i + 1]->n >= T)) {
				z = x->ptr[i + 1];
				y->arr[y->n] = x->arr[i];
				x->arr[i] = z->arr[0];
				y->ptr[y->n + 1] = z->ptr[0];
				z->ptr[0] = z->ptr[1];
				for (j = 0; j < z->n - 1; j++) {
					z->arr[j] = z->arr[j + 1];
					z->ptr[j + 1] = z->ptr[j + 2];
				}
				y->n++;
				z->n--;
			}
			else if ((y->n == T - 1) && (((i > 0) && (x->ptr[i - 1]->n == T - 1)) || ((i < x->n) && (x->ptr[i + 1]->n == T - 1))))
			{
				if (i == x->n)  i = i - 1;
				y = x->ptr[i];
				z = x->ptr[i + 1];
				y->arr[y->n] = x->arr[i];
				y->ptr[y->n + 1] = z->ptr[0];
				for (j = 0; j < z->n; j++) {
					y->arr[j + y->n + 1] = z->arr[j];
					y->ptr[j + y->n + 2] = z->ptr[j + 1];
				}
				free(z);
				for (j = i; j < x->n - 1; j++) {
					x->arr[j] = x->arr[j + 1];
					x->ptr[j + 1] = x->ptr[j + 2];
				}
				x->n--;
				y->n = 2 * T - 1;
			}
			x = y;
		}
		else if (x->arr[i].key == key) {
			y = x->ptr[i];
			z = x->ptr[i + 1];
			if (y->n >= T) {
				//red2
				p = y;
				while (p->ptr[0] != NULL) p = p->ptr[p->n];
				t = x->arr[i].info;
				x->arr[i] = p->arr[p->n - 1];
				p->arr[p->n - 1].info = t;
				key = p->arr[p->n - 1].key;
				x = y;
			}
			else if (z->n >= T) {
				//red
				p = z;
				while (p->ptr[0] != NULL) p = p->ptr[0];
				t = x->arr[i].info;
				x->arr[i] = p->arr[0];
				p->arr[0].info = t;
				key = p->arr[0].key;
				x = z;
			}
			else if ((y->n == T - 1) && (z->n == T - 1)) {
				y->arr[y->n] = x->arr[i];
				y->ptr[y->n + 1] = z->ptr[0];
				for (j = 0; j < z->n; j++) {
					y->arr[j + y->n + 1] = z->arr[j];
					y->ptr[j + y->n + 2] = z->ptr[j + 1];
				}
				free(z);
				for (j = i; j < x->n - 1; j++) {
					x->arr[i] = x->arr[i + 1];
					x->ptr[i + 1] = x->ptr[i + 2];
				}
				x->n--;
				y->n = 2 * T - 1;
				x = y;
			}
		}
	}
	for (i = 0; (i < x->n) && (key > x->arr[i].key); ++i);
	if ((i == x->n) || (key != x->arr[i].key)) return 2;
	if (delflag) free(x->arr[i].info);
	x->arr[i].info = NULL;
	for (j = i; j < x->n - 1; j++) x->arr[j] = x->arr[j + 1];
	x->n--;
	return 0;
}

int F_remove(Table *ptable, int key)
{
	int rc;
	rc = C_remove(ptable, key);
	if (rc == 2) rc = T_remove(ptable, key, 1);
	return(rc);
}

void init(Table *ptable)
{
	int i;
	ptable->root = (Node*)malloc(sizeof(Node));
	ptable->root->n = 0;
	for (i = 0; i < 2 * T; i++) ptable->root->ptr[i] = NULL;
	//table
	for (i = 0; i < N; i++) {
		ptable->cache[i].d = 0;
		ptable->cache[i].f = 1;
	}
}

const int(*fptr[])(Table *) = { NULL, D_Add, D_Find, D_Delete, D_Show, D_Maxkey, D_Load, D_STree, D_AF, D_deleteall, D_profiler };


int main()
{
	int cont = 1, dr;
	Table A;
	init(&A);
	while (dr = dialog())
		if (!fptr[dr](&A)) break;
	printf("Program is stopped.\n");
	F_deleteall(&A);
	free(A.root);
	system("pause");
	return 0;
}