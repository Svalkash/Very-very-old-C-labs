#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define MSZ 10 //Max. table size

typedef struct tableitem {
	int key;
	int par;
	char *info;
} Item;

typedef struct tableitself {
	int size;
	Item tab[MSZ];
} Table;

int getInt(int *add);
int getStr(char **add);
int dialog();
int D_Add(Table *ptable);
int D_Find(Table *ptable);
int D_Delete(Table *ptable);
int D_Show(Table *ptable);
int search(Table *ptable, int key);
int insert(Table *ptable, Item nitem);
int removeitem(Table *ptable, int key);
int checkpar(Table *ptable, int parkey); //CHECK FOR 0
void removechildren(Table *ptable, int parkey);
void printTable(Table *ptable);

const int(*fptr[])(Table *) = { NULL, D_Add, D_Find, D_Delete, D_Show };
const char *MENU[] = { "| 0 | Quit.", "| 1 | Add.", "| 2 | Find.", "| 3 | Delete.", "| 4 | Show." };
const int NOP = sizeof(MENU)/sizeof(MENU[0]);


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
	int res = 1; 
	Item nitem;
	printf("Enter key: ");
	if (!getInt(&nitem.key)) return 0;
	printf("Enter parent key: ");
	if (!getInt(&nitem.par)) return 0;
	printf("Enter information:\n");
	if (!getStr(&nitem.info)) return 0;
	res = insert(ptable, nitem);
	switch (res) {
	case 1:
		printf("Done!\n");
		break;
	case -1:
		printf("ERROR: Table overflow!\n");
		break;
	case -2:
		printf("ERROR: Wrong key!\n");
		break;
	case -3:
		printf("ERROR: Duplicate key!\n");
		break;
	case -4:
		printf("ERROR: Parent doesn't exist!\n");
		break;
	}
	return 1;
}

int D_Find(Table *ptable)
{
	int key = 0;
	int sres = -1;
	Item copy = { 0, 0, NULL };
	printf("Enter key: ");
	if (!getInt(&key)) return 0;
	sres = search(ptable, key);
	switch (sres) {
	case -1: 
		printf("Done!\n");
		printf("Key wasn't found.\n");
		break;
	case -2:
		printf("ERROR: Wrong key!\n");
		break;
	default:
		copy.key = (ptable->tab)[sres].key;
		copy.par = (ptable->tab)[sres].par;
		copy.info = (char*)malloc(strlen((ptable->tab)[sres].info) + 1);
		*copy.info = '\0';
		strcpy_s(copy.info, strlen((ptable->tab)[sres].info) + 1, (ptable->tab)[sres].info);
		printf("Done!\n");
		printf("Item was found!\n Key: %d\n Parent key: %d\n Information:\n%s\n", copy.key, copy.par, copy.info);
		break;
	}
	if (copy.info != NULL) {
		free(copy.info);
		copy.info = NULL;
	}
	return 1;
}

int D_Delete(Table *ptable)
{
	int res = 1;
	int key = 0;
	printf("Enter key: ");
	if (!getInt(&key)) return 0;
	res = removeitem(ptable, key);
	switch (res) {
	case 1:
		printf("Done!\n");
		break;
	case -1:
		printf("ERROR: Table is empty!\n");
		break;
	case -2:
		printf("ERROR: Wrong key!\n");
		break;
	case -3:
		printf("ERROR: Key wasn't found!\n");
		break;
	}
	return 1;
}

int D_Show(Table *ptable)
{
	if (ptable->size == 0) printf("Table is empty!\n");
	else printTable(ptable);
	return 1;
}

int search(Table *ptable, int key)
{
	int i, res = -1;
	if (key == 0) return -2;
	for (i = 0; i < ptable->size; i++) if ((ptable->tab)[i].key == key) res = i;
	return res;
}

int insert(Table *ptable, Item nitem)
{
	int i, done = 0;
	if (ptable->size == MSZ) return -1;
	if (nitem.key == 0) return -2;
	if (search(ptable, nitem.key) >= 0) return -3;
	if (!checkpar(ptable, nitem.par)) return -4;
	ptable->size++;
	for (i = (ptable->size)-1; (i > 0) && (!done); i--) if ((ptable->tab)[i - 1].par <= nitem.par) {
		(ptable->tab)[i] = nitem;
		done = 1;
	}
	else (ptable->tab)[i] = (ptable->tab)[i-1];
	if (!done) (ptable->tab)[i] = nitem;
	return 1;
}

int removeitem(Table *ptable, int key)
{
	int sres = -1;
	int i;
	if (ptable->size == 0) return -1;
	if (key == 0) return -2;
	sres = search(ptable, key);
	if (sres < 0) return -3;
	removechildren(ptable, key);
	free((ptable->tab)[sres].info);
	for (i = sres; i < (ptable->size - 1); i++) (ptable->tab)[i] = (ptable->tab)[i + 1];
	ptable->size--;
	return 1;
}

int checkpar(Table *ptable, int parkey)
{
	int i, res = 0;
	if (parkey == 0) res = 1;
	else for (i = 0; i < ptable->size; i++) if ((ptable->tab)[i].key == parkey) res = 1;
	return res;
}

void removechildren(Table *ptable, int parkey)
{
	int i;
	Item t;
	int changes = 1;
	for (i = 0; (i < ptable->size) && ((ptable->tab)[i].par <= parkey); i++) if ((ptable->tab)[i].par == parkey) (ptable->tab)[i].par = 0;
	while (changes) {
		changes = 0;
		for (i = 0; (i < ptable->size - 1); i++) if ((ptable->tab)[i].par >(ptable->tab)[i + 1].par) {
			t = (ptable->tab)[i + 1];
			(ptable->tab)[i + 1] = (ptable->tab)[i];
			(ptable->tab)[i] = t;
			changes = 1;
		}
	}
}

void printTable(Table *ptable)
{
	int i;
	printf("Current size: %d.\n", ptable->size);
	printf("Format:\n");
	printf("| Item key | Parent key |\nInformation\n");
	printf("Table items:\n");
	for (i = 0; i < ptable->size; i++) printf("| %d | %d |\n%s\n", (ptable->tab)[i].key, (ptable->tab)[i].par, (ptable->tab)[i].info);
}

int main()
{
	int cont = 1, dr;
	Table T;
	T.size = 0;
	while (dr = dialog())
		if (!fptr[dr](&T)) break;
	printf("Program is stopped.\n");
	system("pause");
	return 0;
}