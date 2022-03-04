#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define MSZ 10 //Max. table size
#define NOP 5 //Number of operations
#define MENU "________________________________________________________________________________\nMENU:\n| 0 | Quit;\n| 1 | Add;\n| 2 | Find;\n| 3 | Delete;\n| 4 | Show.\n| 5 | Parent->step-parent.\n________________________________________________________________________________\n"

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
int D_PSP(Table *ptable);
void psp(Table *ptable, Item parent);
int search(Table *ptable, int key);
void insert(Table *ptable, Item nitem);
void removeitem(Table *ptable, int pos);
int checkpar(Table *ptable, int parkey); //CHECK FOR 0
void removechildren(Table *ptable, int parkey);
void printTable(Table *ptable);


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
	int n;
	do {
		printf("%s", MENU);
		printf("Choose an option: ");
		n = getInt(&res);
		if (n == 0) {
			res = 0;
			printf("End of file is reached. Stopping the program...\n");
		}
		if (res > NOP || res < 0) printf("Please, choose from given options!\n");
	} while (res > NOP || res < 0);
	return res;
}

int D_Add(Table *ptable)
{
	int cont = 1; //cont = 0 - eof, cont = -1 - overflow, cont = -2 - wrong key, cont = -3 - duplicate, cont = -4 - parent, 
	Item nitem;
	if (ptable->size == MSZ) cont = -1;
	if (cont == 1) {
		printf("Enter key: ");
		cont = getInt(&nitem.key);
	}
	if (cont == 1) if (nitem.key == 0) cont = -2;
	if (cont == 1) if (search(ptable, nitem.key) >= 0) cont = -3;
	if (cont == 1) {
		printf("Enter parent key: ");
		cont = getInt(&nitem.par);
	}
	if (cont == 1 && nitem.par != 0) if (!checkpar(ptable, nitem.par)) cont = -4;
	if (cont == 1) {
		printf("Enter information:\n");
		cont = getStr(&nitem.info);
	}
	if (cont == 1) insert(ptable, nitem);
	switch (cont) {
	case 1:
		printf("Done!\n");
		break;
	case 0:
		printf("ERROR: End of file is reached. Stopping the program...\n");
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
	if (cont == 0) return 0;
	 else return 1;
}

int D_Find(Table *ptable)
{
	int cont = 1; //cont = 0 - eof, cont =-1 - wrong key
	int key = 0;
	int sres = -1;
	Item copy = { 0, 0, NULL };
	printf("Enter key: ");
	cont = getInt(&key);
	if (key == 0 && cont == 1) cont = -1;
	if (cont == 1) sres = search(ptable, key);
	if ((cont == 1) && (sres >= 0)) {
		copy.key = (ptable->tab)[sres].key;
		copy.par = (ptable->tab)[sres].par;
		copy.info = (char*)malloc(strlen((ptable->tab)[sres].info) + 1);
		*copy.info = '\0';
		strcpy_s(copy.info, strlen((ptable->tab)[sres].info) + 1, (ptable->tab)[sres].info);
	}
	//Copied!
	switch (cont) {
	case 1: 
		printf("Done!\n");
		if (sres >= 0) printf("Item was found!\n Key: %d\n Parent key: %d\n Information:\n%s\n", copy.key, copy.par, copy.info);
		else printf("Key wasn't found.\n");
		break;
	case 0:
		printf("ERROR: End of file is reached. Stopping the program...\n");
		break;
	case -1:
		printf("ERROR: Wrong key!\n");
		break;
	}
	if (copy.info != NULL) {
		free(copy.info);
		copy.info = NULL;
	}
	if (cont == 0) return 0;
	else return 1;
}

int D_Delete(Table *ptable) //delchildren
{
	int cont = 1; //cont = 0 - eof, cont = -1 - empty, cont =-2 - wrong key, cont = -3 - not found
	int key = 0;
	int sres = -1;
	if (ptable->size == 0) cont = -1;
	if (cont == 1) {
		printf("Enter key: ");
		cont = getInt(&key);
	}
	if (cont == 1 && key == 0) cont = -2;
	if (cont == 1) sres = search(ptable, key);
	if (cont == 1 && sres < 0) cont = -3;
	if (cont == 1) {
		psp(ptable, (ptable->tab)[sres]);
		removeitem(ptable, sres);
		//removechildren(ptable, key);
	}
	switch (cont) {
	case 1:
		printf("Done!\n");
		break;
	case 0:
		printf("ERROR: End of file is reached. Stopping the program...\n");
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
	if (cont == 0) return 0;
	else return 1;
}

int D_Show(Table *ptable)
{
	if (ptable->size == 0) printf("Table is empty!\n");
	else printTable(ptable);
	return 1;
}

int D_PSP(Table *ptable)
{
	int cont = 1; //cont = 0 - eof, cont =-1 - wrong key, -2 - not found
	int key = 0;
	int sres = -1;
	printf("Enter key: ");
	cont = getInt(&key);
	if (key == 0 && cont == 1) cont = -1;
	if (cont == 1) sres = search(ptable, key);
	if (cont == 1 && sres == -1) cont = -2;
	psp(ptable, (ptable->tab)[sres]);
	switch (cont) {
	case 1:
		printf("Done!\n");
		break;
	case 0:
		printf("ERROR: End of file is reached. Stopping the program...\n");
		break;
	case -1:
		printf("ERROR: Wrong key!\n");
		break;
	case -2:
		printf("ERROR: Key wasn't found!\n");
		break;
	}
	if (cont == 0) return 0;
	else return 1;
}


void psp(Table *ptable, Item parent)
{
	int i;
	Item t;
	int changes = 1;
	for (i = 0; (i < ptable->size) && ((ptable->tab)[i].par <= parent.key); i++) if ((ptable->tab)[i].par == parent.key) (ptable->tab)[i].par = parent.par; //!!!
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

int search(Table *ptable, int key)
{
	int i, res = -1;
	for (i = 0; i < ptable->size; i++) if ((ptable->tab)[i].key == key) res = i;
	return res;
}

void insert(Table *ptable, Item nitem)
{
	int i, done = 0;
	ptable->size++;
	for (i = (ptable->size)-1; (i > 0) && (!done); i--) if ((ptable->tab)[i - 1].par <= nitem.par) {
		(ptable->tab)[i] = nitem;
		done = 1;
	}
	else (ptable->tab)[i] = (ptable->tab)[i-1];
	if (!done) (ptable->tab)[i] = nitem;
}

void removeitem(Table *ptable, int pos)
{
	int i;
	free((ptable->tab)[pos].info);
	for (i = pos; i < (ptable->size-1); i++) (ptable->tab)[i] = (ptable->tab)[i + 1];
	ptable->size--;
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
	int(*fptr[])(Table *) = { NULL, D_Add, D_Find, D_Delete, D_Show, D_PSP };
	Table T;
	T.size = 0;
	while (cont) {
		dr = dialog();
		if (dr) cont = fptr[dr](&T);
		else cont = 0;
	}
	printf("Program is stopped.\n");
	system("pause");
	return 0;
}