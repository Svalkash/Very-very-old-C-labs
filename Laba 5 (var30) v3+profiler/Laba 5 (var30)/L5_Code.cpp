#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define N 10 //Max. cache size
#define T 2 //Degree?? Okay, it's 2-3-4... Should be.
typedef struct smallitem {
	int key;
	char *info;
} Item;

typedef struct cacheitem {
	int f; //filled
	int d; //deleted
	Item I;
} CItem;

typedef struct treeitem {
	int n;
	Item arr[2 * T - 1];
	Node *ptr[2 * T];
	Node *parent;
} Node;

typedef struct fulltable {
	Node *root;
	CItem cache[N];
} Table;

const char *MENU[] = { "| 0 | Quit.", "| 1 | Add.", "| 2 | Find.", "| 3 | Delete.", "| 4 | Show.", "| 5 | Find max. key.", "| 6 | Load from file.", "| 7 | Show the tree.", "| 8 | Fill file." };
const int NOP = sizeof(MENU) / sizeof(MENU[0]);
const char *ERRMSG[] = { "Done!", "ERROR: Duplicate key!", "ERROR: Key wasn't found!" };
const char *fname = "loader.txt";

//get
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
int D_FF(Table *ptable);
//operators
int F_insert(Table *ptable, int key, char *info);
int F_search(Table *ptable, int key, int *cpos);
int F_remove(Table *ptable, int key);
int F_print(Table *ptable);
int F_maxkey(Table *ptable, int *cpos);
//operations
int C_insert(Table *ptable, Item nitem);
int T_insert(Table *ptable, Item nitem);
int C_search(Table *ptable, int key);
int T_search(Table *ptable, int key);
int C_remove(Table *ptable, int key);
int T_remove(Table *ptable, int key);
int C_print(Table *ptable);
int T_print(Table *ptable);
int C_maxkey(Table *ptable);
int T_maxkey(Table *ptable, int cmk, int *cpos);
int T_stree(Table *ptable);
void T_ff(Table *ptable, int num);
//loading
int fload(Table *ptable);
//filling
int fillfile(Table *ptable);
//hash
int hash(int key);


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
		puts("________________________________________________________________________________");
		puts("MENU:");
		for (i = 0; i < NOP; i++) puts(MENU[i]);
		puts("________________________________________________________________________________");
		printf("Choose an option: ");
		if (!getInt(&res)) res = 0;
		if (res > NOP || res < 0) printf("Please, choose from given options!\n");
	} while (res > NOP || res < 0);
	return res;
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
	src = F_search(ptable, key, &cpos);
	puts(ERRMSG[src]);
	if (cpos >= 0) printf("Key was found! Information:\n%s\n", ptable->cache[cpos].I.info);
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
	int rc = F_print(ptable);
	if (rc) printf("Table is empty!\n");
	return 1;
}

int D_Maxkey(Table *ptable)
{
	int cpos;
	int rc = F_maxkey(ptable, &cpos);
	if (rc) printf("Table is empty!\n");
	else printf("Key: %d, Info:\n%s\n", ptable->cache[cpos].I.key, ptable->cache[cpos].I.info);
	return 1;
}

int D_STree(Table *ptable)
{
	int rc = T_stree(ptable);
	if (rc) printf("Tree is empty!\n");
	return 1;
}

int D_FF(Table *ptable)
{
	int num = 0;
	while (num <= 0) {
		printf("Enter number of elements: ");
		if (!getInt(&num)) return 0;
		if (num <= 0) printf("ERROR: Enter natural number\n");
	}
	T_ff(ptable, num);
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
	Item nitem;
	fopen_s(&f, fname, "r+t");
	if (f == NULL) return 1;
	while (fgetInt(f, &nitem.key) && fgetStr(f, &nitem.info)) {
		T_insert(ptable, nitem);
		nitem.info = NULL;
	}
	return 0;
}

const int(*fptr[])(Table *) = { NULL, D_Add, D_Find, D_Delete, D_Show, D_Maxkey, D_Load, D_STree, D_FF };


int main()
{
	int cont = 1, dr;
	Table A;
	if (!D_Load(&A)) return 1; //EOF before 
	while (dr = dialog())
		if (!fptr[dr](&A)) break;
	printf("Program is stopped.\n");
	system("pause");
	return 0;
}