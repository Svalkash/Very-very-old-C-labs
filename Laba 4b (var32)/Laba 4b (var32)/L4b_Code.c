#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define MSZ 10 //Max. table size

typedef struct tableitem {
	int key;
	int par;
	int offset;
	int len;
} Item;

typedef struct nontableitem {
	int key;
	int par;
	char *info;
} COI; //Copy Of Item

typedef struct tableitself {
	int size;
	Item tab[MSZ];
	FILE *f;
} Table;

int getInt(int *add);
int getStr(char **add);
int dialog();
int D_Add(Table *ptable);
int D_Find(Table *ptable);
int D_Delete(Table *ptable);
int D_Show(Table *ptable);
int search(Table *ptable, int key);
int collect(Table *ptable, int key, COI *dest);
int insert(Table *ptable, int key, int par, char *info);
int removeitem(Table *ptable, int key);
int checkpar(Table *ptable, int parkey);
void removechildren(Table *ptable, int parkey);
void cutfile(Table *ptable, int offset, int len);
void changeoffsets(Table *ptable, int offset, int len);
void printTable(Table *ptable);
//loading
int D_Load(Table *ptable);
int fload(Table *ptable, char *fname);
int fcreate(Table *ptable, char *fname);
//D_Save?
int save(Table *ptable);

const int(*fptr[])(Table *) = { NULL, D_Add, D_Find, D_Delete, D_Show };
const char *MENU[] = { "| 0 | Quit.", "| 1 | Add.", "| 2 | Find.", "| 3 | Delete.", "| 4 | Show." };
const int NOP = sizeof(MENU) / sizeof(MENU[0]);


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
	//Item removed
	int key, par;
	char *info = NULL;
	printf("Enter key: ");
	if (!getInt(&key)) return 0;
	printf("Enter parent key: ");
	if (!getInt(&par)) return 0;
	printf("Enter information:\n");
	if (!getStr(&info)) return 0;
	res = insert(ptable, key, par, info);
	free(info);
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
	COI copy = { 0, 0, NULL };
	printf("Enter key: ");
	if (!getInt(&key)) return 0;
	sres = collect(ptable, key, &copy);
	switch (sres) {
	case -1:
		printf("Done!\n");
		printf("Key wasn't found.\n");
		break;
	case -2:
		printf("ERROR: Wrong key!\n");
		break;
	default:
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

int collect(Table *ptable, int key, COI *dest)
{
	int sres = search(ptable, key);
	if (sres < 0) return sres;
	dest->key = (ptable->tab)[sres].key;
	dest->par = (ptable->tab)[sres].par;
	dest->info = (char*)malloc((ptable->tab)[sres].len + 1);
	fseek(ptable->f, (ptable->tab)[sres].offset, SEEK_SET);
	fread(dest->info, sizeof(char), (ptable->tab)[sres].len, ptable->f);
	return sres;
}

int insert(Table *ptable, int key, int par, char *info)
{
	int i;
	if (ptable->size == MSZ) return -1;
	if (key == 0) return -2;
	if (search(ptable, key) >= 0) return -3;
	if (!checkpar(ptable, par)) return -4;
	ptable->size++;
	fseek(ptable->f, 0, SEEK_END);
	//&&!done
	for (i = (ptable->size) - 1; i > 0; i--) if ((ptable->tab)[i - 1].par <= par) break;
	else (ptable->tab)[i] = (ptable->tab)[i - 1];
	//IS IT WORKING?
	(ptable->tab)[i].key = key;
	(ptable->tab)[i].par = par;
	(ptable->tab)[i].len = strlen(info) + 1;
	(ptable->tab)[i].offset = ftell(ptable->f);
	fwrite(info, sizeof(char), (ptable->tab)[i].len, ptable->f);
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
	cutfile(ptable, (ptable->tab)[sres].offset, (ptable->tab)[sres].len);
	//changeoffsets(ptable, (ptable->tab)[sres].offset, (ptable->tab)[sres].len);
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
	char *bufinfo = NULL;
	printf("Current size: %d.\n", ptable->size);
	printf("Format:\n");
	printf("| Item key | Parent key |\nInformation\n");
	printf("Table items:\n");
	for (i = 0; i < ptable->size; i++) {
		fseek(ptable->f, (ptable->tab)[i].offset, SEEK_SET);
		bufinfo = (char*)realloc(bufinfo, (ptable->tab)[i].len + 1);
		fread(bufinfo, sizeof(char), (ptable->tab)[i].len, ptable->f);
		printf("| %d | %d |\n%s\n", (ptable->tab)[i].key, (ptable->tab)[i].par, bufinfo);
	}
}

int D_Load(Table *ptable)
{
	char *fname = NULL;
	printf("Enter file name: ");
	if (!getStr(&fname)) return 0;
	if (fload(ptable, fname) == 0) fcreate(ptable, fname);
	free(fname);
	return 1;
}

//Â ÔÀÉËÅ ÍÅÒ ÐÀÇÌÅÐÀ, ÎÍ ÊÎÍÑÒÀÍÒÀ

int fload(Table *ptable, char *fname)
{
	fopen_s(&ptable->f, fname, "r+b");
	if (ptable->f == NULL) return 0;
	//ïðîïóùåíî âûäåëåíèå ïàìÿòè è ÷òåíèå ðàçìåðà SZ
	fread(&ptable->size, sizeof(int), 1, ptable->f);
	fread(ptable->tab, sizeof(Item), ptable->size, ptable->f);
	return 1;
}

int fcreate(Table *ptable, char *fname)
{
	ptable->size = 0;
	if (fopen_s(&ptable->f, fname, "w+b") != 0) return 0;
	fwrite(&ptable->size, sizeof(int), 1, ptable->f);
	fwrite(ptable->tab, sizeof(Item), MSZ, ptable->f);
	return 1;
	//created EMPTY table
}

int save(Table *ptable)
{
	fseek(ptable->f, 0, SEEK_SET);
	//ïðîïóñê äëèíû âåêòîðà ÏÐÎÏÓÙÅÍ
	fwrite(&ptable->size, sizeof(int), 1, ptable->f);
	fwrite(ptable->tab, sizeof(Item), ptable->size, ptable->f);
	fclose(ptable->f);
	ptable->f = NULL;
	return 1;
}

void cutfile(Table *ptable, int offset, int len)
{
	void *tmpbef = NULL, *tmpaf = NULL;
	int offeof;
	fseek(ptable->f, 0, SEEK_END);
	offeof = ftell(ptable->f);
	tmpbef = (void*)malloc(offset);
	if (offset + len < offeof) tmpaf = (void*)malloc(offeof - (offset + len));
	fseek(ptable->f, offset, SEEK_SET);
	fread(tmpbef, 1, offset, ptable->f);
	if (offset + len < offeof) {
		fseek(ptable->f, offset + len, SEEK_SET);
		fread(tmpaf, 1, offeof - (offset + len)+1, ptable->f);
	}
	fclose(ptable->f);
	fopen_s(&ptable->f, "file.f", "w+b");
	fseek(ptable->f, 0, SEEK_SET);
	fwrite(tmpbef, 1, offset, ptable->f);
	if (offset + len < offeof) {
		fseek(ptable->f, offset, SEEK_SET);
		fwrite(tmpaf, 1, offeof - (offset + len)+1, ptable->f);
	}
	free(tmpbef);
	if (tmpaf) free(tmpaf);
}

void changeoffsets(Table *ptable, int offset, int len)
{
	int i;
	for (i = 0; i < ptable->size; i++) if ((ptable->tab)[i].offset >= offset) (ptable->tab)[i].offset -= len;
}

int main()
{
	int cont = 1, dr;
	Table T;
	T.f = NULL;
	T.size = 0;
	if (!D_Load(&T)) return 1; //EOF before 
	while (dr = dialog())
		if (!fptr[dr](&T)) break;
	fseek(T.f, 0, SEEK_SET);
	printf("%d", feof(T.f));
	save(&T);
	printf("Program is stopped.\n");
	system("pause");
	return 0;
}