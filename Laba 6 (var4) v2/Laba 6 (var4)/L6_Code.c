#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

//types
const int D = 0; //deadend
const int E = 1; //exit
const int C = 2; //cross

//colors
const int W = 10;
const int G = 11;
const int B = 12;
//infinity?
const int INF = INT_MAX;
//possibility of exit (%)
const int PE = 5;
//profiler: corridors/rooms
const int PRC = 2;
//profiler: number of exits
const int PNE = 1;

typedef struct position {
	int x;
	int y;
} Pos;

typedef struct adjitem {
	Pos pos; //to
	struct adjitem *next;
} AList;

typedef struct point {
	Pos pos;
	char *name;
	int type;
	AList *Adj;
	struct point *prev;
	struct point *next;
	//for search
	int color;
	int d;
	struct point *pred;
	//addition
	int marks; //canbeexit
	int ne;
	int counted;
} Room;

const char *SMENU[] = { "| 0 | Create empty labirint.", "| 1 | Load from file.", "| 2 | Autofill." };
const int SNOP = sizeof(SMENU) / sizeof(SMENU[0]);
const char *MENU[] = { "| 0 | Quit.", "| 1 | Add room.", "| 2 | Add corridor.", "| 3 | Delete room.", "| 4 | Find exit.", "| 5 | Show labirint.", "| 6 | Launch profiler.", "| 7 | 3-to-exit." };
const int NOP = sizeof(MENU) / sizeof(MENU[0]);
const char *ERRMSG[] = { "Done!", "ERROR: Duplicate name!", "ERROR: Duplicate position!", "ERROR: Connected room not found!", "ERROR: Room not found!", "ERROR: Cannot create corridor to itself"};
const char *fname = "labirint.f";
const char *LINE = "________________________________________________________________________________";

int getInt(int *add);//
int getStr(char **add);//

int dialog();//

int D_Start(Room *L);//
int fload(Room *L);//
int fsave(Room *L);//
int clearmem(Room *L);//
int generate(Room *L, int nr, int nc, int lx, int rx, int dy, int uy);

int D_AddR(Room *L);//
Room *searchP(Room *L, Pos p);//
Room *searchN(Room *L, char *s);//
int insertR(Room *L, Pos p, char *name, char **snames, int snlen, int isexit);//

int D_AddC(Room *L);//
int insertC(Room *L, char *name1, char *name2);//

int D_DelR(Room *L);//
int deleteR(Room *L, char *name);//
int clearlist(AList *al);//

int D_FindE(Room *L);//
int findexit(Room *L, char *name);//
int  Pfindexit(Room *L, char *name);
int printpath(Room *s, Room *r);//

int D_Show(Room *L);
void show(Room *L);

int D_Profiler(Room *L);
float profiler(int nr, int iter);
//help
int rcount(Room *r);
int alcount(AList *al);
//addition
int D_3TE(Room *L);
int transform_3te(Room *L);
int checkexit(Room *L, Room *r, int dist);
int mark0(Room *L, Room *r, int dist);
int marker(Room *L, Room *r, int dist);
int count3ne(Room *L, Room *r, int dist);
int maxmark(Room *L, Room *r, int dist);
int createexit(Room *L, Room *r, int mm, int dist);
int counter0(Room *L, Room *r, int dist);

const int(*fptr[])(Room *) = { NULL, D_AddR, D_AddC, D_DelR, D_FindE, D_Show, D_Profiler, D_3TE };

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

int D_Start(Room *L)
{
	int res, rc;
	int i;
	int nr = 0, nc = -1, lx = 0, rx = 0, dy = 0, uy = 0;
	do {
		puts(LINE);
		puts("START:");
		for (i = 0; i < SNOP; i++) puts(SMENU[i]);
		puts(LINE);
		printf("Choose an option: ");
		if (!getInt(&res)) res = 0;
		if (res > NOP || res < 0) printf("Please, choose from given options!\n");
	} while (res > NOP || res < 0);
	puts(LINE);
	if (res == 0) {
		puts(ERRMSG[0]);
		return 1;
	}
	if (res == 1) {
		rc = fload(L);
		if (rc) puts(ERRMSG[0]);
		else puts("ERROR: File not found!");
		return rc;
	}
	if (res == 2) {
		while (nr <= 0) {
			printf("Enter number of rooms: ");
			if (!getInt(&nr)) return 0;
			if (nr <= 0) puts("ERROR: Enter natural number!");
		}
		while (nc < 0) {
			printf("Enter number of corridors: ");
			if (!getInt(&nc)) return 0;
			if (nc < 0) puts("ERROR: Enter natural number or 0!");
		}
		while ((rx - lx + 1)*(uy - dy + 1) < nc) {
			printf("Enter minimum X: ");
			if (!getInt(&lx)) return 0;
			printf("Enter maximum X: ");
			if (!getInt(&rx)) return 0;
			printf("Enter minimum Y: ");
			if (!getInt(&dy)) return 0;
			printf("Enter maximum Y: ");
			if (!getInt(&uy)) return 0;
			if ((rx - lx + 1)*(uy - dy + 1) < nc) puts("ERROR: Labirint size is too small for entered number of rooms!");
		}
		generate(L, nr, nc, lx, rx, dy, uy);
		puts(ERRMSG[0]);
		return 1;
	}
	return 0;
}
int fload(Room *L)
{
	FILE *f;
	int i, j, nr, slen, llen;
	Room *r = NULL;
	AList *al = NULL;
	fopen_s(&f, fname, "r+b");
	if (f == NULL) return 0;
	fread(&nr, sizeof(int), 1, f);
	for (i = 0; i < nr; i++) {
		r = (Room*)malloc(sizeof(Room));
		r->Adj = NULL;
		r->name = NULL;
		fread(&(r->pos.x), sizeof(int), 1, f);
		fread(&(r->pos.y), sizeof(int), 1, f);
		fread(&slen, sizeof(int), 1, f);
		r->name = (char*)malloc(slen + 1);
		fread(r->name, sizeof(char), slen + 1, f);
		fread(&(r->type), sizeof(int), 1, f);
		fread(&llen, sizeof(int), 1, f);
		for (j = 0; j < llen; j++) {
			al = (AList*)malloc(sizeof(AList));
			fread(&(al->pos.x), sizeof(int), 1, f);
			fread(&(al->pos.y), sizeof(int), 1, f);
			al->next = r->Adj;
			r->Adj = al;
			al = NULL;
		}
		r->prev = L;
		r->next = L->next;
		if (L->next) L->next->prev = r;
		L->next = r;
		r = NULL;
	}
	fclose(f);
	return 1;
}
int rcount(Room *r)
{
	int c = 0;
	while (r != NULL) {
		c++;
		r = r->next;
	}
	return c;
}
int alcount(AList *al)
{
	int c = 0;
	while (al != NULL) {
		c++;
		al = al->next;
	}
	return c;
}
int fsave(Room *L)
{
	int i, j, nr, slen, llen;
	Room *r = NULL;
	AList *al = NULL;
	FILE *f;
	fopen_s(&f, fname, "w+b");
	if (f == NULL) return 0;
	fseek(f, 0, SEEK_SET);
	nr = rcount(L->next);
	fwrite(&nr, sizeof(int), 1, f);
	r = L->next;
	for (i = 0; i < nr; i++) {
		fwrite(&(r->pos.x), sizeof(int), 1, f);
		fwrite(&(r->pos.y), sizeof(int), 1, f);
		slen = strlen(r->name);
		fwrite(&slen, sizeof(int), 1, f);
		fwrite(r->name, sizeof(char), slen + 1, f);
		fwrite(&(r->type), sizeof(int), 1, f);
		llen = alcount(r->Adj);
		fwrite(&llen, sizeof(int), 1, f);
		al = r->Adj;
		for (j = 0; j < llen; j++) {
			fwrite(&(al->pos.x), sizeof(int), 1, f);
			fwrite(&(al->pos.y), sizeof(int), 1, f);
			al = al->next;
		}
		r = r->next;
	}
	fclose(f);
	return 1;
}
//generation!

int D_AddR(Room *L)
{
	int rc = 1;
	int i, j;
	Pos p;
	char *name = NULL;
	char **snames = NULL;
	int snl = -1;
	int flag = 0;
	printf("Enter coordinate X: ");
	if (!getInt(&(p.x))) return 0;
	printf("Enter coordinate Y: ");
	if (!getInt(&(p.y))) return 0;
	printf("Enter name: ");
	if (!getStr(&name)) return 0;
	while (snl < 0) {
		printf("Enter number of connections: ");
		if (!getInt(&snl)) {
			free(name);
			return 0;
		}
		if (snl < 0) puts("ERROR: I can't imagine this! Enter 0 or natural.");
	}
	if (snl != 0) snames = (char**)malloc(sizeof(char*) * snl);
	for (i = 0; i < snl; i++) {
		printf("Enter name of connected room #%d: ", i);
		if (!getStr(&(snames[i]))) {
			for (j = 0; j < i; j++) free(snames[j]);
			free(snames);
			free(name);
			return 0;
		}
	}
	printf("If it is exit, enter '1', else enter any int: ");
	if (!getInt(&flag)) {
		for (j = 0; j < snl; j++) free(snames[j]);
		free(snames);
		free(name);
		return 0;
	}
	if (flag != 1) flag = 0;
	rc = insertR(L, p, name, snames, snl, flag);
	for (j = 0; j < snl; j++) free(snames[j]);
	free(snames);
	if (rc) free(name);
	puts(ERRMSG[rc]);
	return 1;
}
Room *searchP(Room *L, Pos p)
{
	Room *r = L->next;
	while (r != NULL) {
		if ((r->pos.x == p.x) && (r->pos.y == p.y)) return r;
		r = r->next;
	}
	return NULL;
}
Room *searchN(Room *L, char *s)
{
	Room *r = L->next;
	while (r != NULL) {
		if (!strcmp(r->name, s)) return r;
		r = r->next;
	}
	return NULL;
}
int insertR(Room *L, Pos p, char *name, char **snames, int snlen, int isexit)
{
	int i, allen = 0;
	AList *al = NULL;
	Room *r = NULL;
	Room *sr = NULL;
	if (searchN(L, name)) return 1;
	if (searchP(L, p)) return 2;
	for (i = 0; i < snlen; i++) if (!searchN(L, snames[i])) return 3;
	r = (Room*)malloc(sizeof(Room));
	r->Adj = NULL;
	r->pos = p;
	r->name = name;
	if (isexit) r->type = E;
	else if (snlen <= 1) r->type = D;
	else r->type = C;
	for (i = 0; i < snlen; i++) {
		sr = searchN(L, snames[i]);
		al = (AList*)malloc(sizeof(AList));
		al->pos = sr->pos;
		al->next = r->Adj;
		r->Adj = al;
		al = NULL;
		//addition to connected
		al = (AList*)malloc(sizeof(AList));
		al->pos = p;
		al->next = sr->Adj;
		sr->Adj = al;
		al = NULL;
		//needtochangetypeofconnected
		allen = alcount(sr->Adj);
		if (sr->type == E) sr->type = E;
		else if (allen <= 1) sr->type = D;
		else sr->type = C;
	}
	r->prev = L;
	r->next = L->next;
	if (L->next) L->next->prev = r;
	L->next = r;
	return 0;
}

int D_AddC(Room *L)
{
	char *name1 = NULL, *name2 = NULL;
	int rc;
	printf("Enter name of room 1: ");
	if (!getStr(&name1)) return 0;
	printf("Enter name of room 2: ");
	if (!getStr(&name2)) {
		free(name1);
		return 0;
	}
	rc = insertC(L, name1, name2);
	puts(ERRMSG[rc]);
	free(name1);
	free(name2);
	return 1;
}
int insertC(Room *L, char *name1, char *name2)
{
	Room *r1 = searchN(L, name1), *r2 = searchN(L, name2);
	AList *al = NULL;
	int allen = 0;
	if ((!r1) || (!r2)) return 4;
	if (r1 == r2) return 5;
	al = (AList*)malloc(sizeof(AList));
	al->pos = r2->pos;
	al->next = r1->Adj;
	r1->Adj = al;
	al = NULL;
	//r2
	al = (AList*)malloc(sizeof(AList));
	al->pos = r1->pos;
	al->next = r2->Adj;
	r2->Adj = al;
	al = NULL;
	//r1
	allen = alcount(r1->Adj);
	if (r1->type == E) r1->type = E;
	else if (allen <= 1) r1->type = D;
	else r1->type = C;
	//r2
	allen = alcount(r2->Adj);
	if (r2->type == E) r2->type = E;
	else if (allen <= 1) r2->type = D;
	else r2->type = C;
	return 0;
}

int D_DelR(Room *L)
{
	char *name = NULL;
	int rc;
	printf("Enter name:\n");
	if (!getStr(&name)) return 0;
	rc = deleteR(L, name);
	puts(ERRMSG[rc]);
	free(name);
	return 1;
}
int deleteR(Room *L, char *name)
{
	Room *r = searchN(L, name);
	Room *sr = NULL;
	AList *ral = NULL, *sal = NULL, *t = NULL;
	int allen;
	if (!r) return 4;
	ral = r->Adj;
	while (ral) {
		sr = searchP(L, ral->pos);
		while ((sr->Adj) && ((sr->Adj->pos.x == r->pos.x) && (sr->Adj->pos.y == r->pos.y))) {
			t = sr->Adj;
			sr->Adj = sr->Adj->next;
			free(t);
		}
		sal = sr->Adj;
		while (sal->next) {
			if ((sal->next->pos.x == r->pos.x) && (sal->next->pos.y == r->pos.y))
			{
				t = sal->next;
				sal->next = sal->next->next;
				free(t);
			}
			else sal = sal->next;
		}
		allen = alcount(sr->Adj);
		if (sr->type == E) sr->type = E;
		else if (allen <= 1) sr->type = D;
		else sr->type = C;
		ral = ral->next;
	}
	//now delete r
	r->prev->next = r->next;
	if (r->next) r->next->prev = r->prev;
	clearlist(r->Adj);
	free(r->name);
	free(r);
	return 0;
}
int clearlist(AList *al)
{
	if (!al) return 0;
	clearlist(al->next);
	free(al);
	return 1;
}

int clearmem(Room *r)
{
	if (!r) return 0;
	clearmem(r->next);
	free(r->name);
	clearlist(r->Adj);
	free(r);
	return 1;
}

int D_FindE(Room *L)
{
	char* name = NULL;
	int rc;
	printf("Enter name: ");
	if (!getStr(&name)) return 0;
	rc = findexit(L, name);
	puts(ERRMSG[rc]);
	free(name);
	return 1;
}
int findexit(Room *L, char *name)
{
	Room *s = searchN(L, name);
	AList *al = NULL;
	Room *r = NULL;
	Room **Q = NULL;
	int qh, qt, qsz;
	int minpath = INF;
	Room *mpe = NULL; //min. path exit
	if (!s) return 4;
	//init
	r = L->next;
	while (r) {
		r->color = W;
		r->d = INF;
		r->pred = NULL;
		r = r->next;
	}
	s->color = G;
	s->d = 0;
	qsz = rcount(L->next) + 1;
	Q = (Room**)malloc(sizeof(Room*) * qsz); 
	qh = 0;
	Q[0] = s;
	qt = 1;
	while (qh != qt) { //must work?
		if (Q[qh]->type == E) {
			minpath = Q[qh]->d;
			mpe = Q[qh];
			//Sure?
			break;
		}
		al = Q[qh]->Adj;
		while (al) {
			r = searchP(L, al->pos);
			if (r->color == W) {
				r->color = G;
				r->d = Q[qh]->d + 1;
				r->pred = Q[qh];
				Q[qt] = r;
				qt = (qt + 1) % qsz;
			}
			al = al->next;
		}
		Q[qh]->color = B;
		qh = (qh + 1) % qsz;
	}
	if (mpe) {
		printf("Found path: ");
		printpath(s, mpe);
		printf("\nPath distance: %d.\n", minpath);
	}
	else puts("Path not found");
	return 0;
}
int Pfindexit(Room *L, char *name)
{
	Room *s = searchN(L, name);
	AList *al = NULL;
	Room *r = NULL;
	Room **Q = NULL;
	int qh, qt, qsz;
	int minpath = INF;
	Room *mpe = NULL; //min. path exit
	if (!s) return 4;
	//init
	r = L->next;
	while (r) {
		r->color = W;
		r->d = INF;
		r->pred = NULL;
		r = r->next;
	}
	s->color = G;
	s->d = 0;
	qsz = rcount(L->next) + 1;
	Q = (Room**)malloc(sizeof(Room*) * qsz);
	qh = 0;
	Q[0] = s;
	qt = 1;
	while (qh != qt) { //must work?
		if (Q[qh]->type == E) {
			minpath = Q[qh]->d;
			mpe = Q[qh];
			//Sure?
			break;
		}
		al = Q[qh]->Adj;
		while (al) {
			r = searchP(L, al->pos);
			if (r->color == W) {
				r->color = G;
				r->d = Q[qh]->d + 1;
				r->pred = Q[qh];
				Q[qt] = r;
				qt = (qt + 1) % qsz;
			}
			al = al->next;
		}
		Q[qh]->color = B;
		qh = (qh + 1) % qsz;
	}
	return 0;
}

int printpath(Room *s, Room *r)
{
	int rc;
	if (r == s) printf("%s ", s->name);
	else if (r->pred == NULL) {
		printf("Path not found.\n");
		return 0;
	}
	else {
		rc = printpath(s, r->pred);
		if (rc) printf("%s ", r->name);
	}
	return 1;
}

int D_Show(Room *L)
{
	if (!L->next) puts("Labirint is empty!");
	else show(L);
	return 1;
}

void show(Room *L)
{
	char s[9];
	AList *al = NULL;
	Room *rel = NULL;
	Room *r = L->next;
	*s = '\0';
	while (r) {
		printf("Name: \"%s\".\n", r->name);
		printf("Position: { %d ; %d }.\n", r->pos.x, r->pos.y);
		if (r->type == D) sprintf_s(s, 9, "Dead end");
		if (r->type == C) sprintf_s(s, 9, "Crossout");
		if (r->type == E) sprintf_s(s, 9, "Exit");
		printf("Type: %s.\n", s);
		*s = '0';
		printf("Connected rooms: ", s);
		al = r->Adj;
		while (al) {
			rel = searchP(L, al->pos);
			printf("%s, ", rel->name);
			al = al->next;
		}
		printf(".\n__________\n");
		r = r->next;
	}
}

int generate(Room *L, int nr, int nc, int lx, int rx, int dy, int uy)
{
	int i, r1, r2;
	char *s = NULL;
	char n1[20], n2[20];
	Pos p;
	int ie = 0;
	for (i = 0; i < nr;) {
		s = (char*)malloc(20);
		*s = '\0';
		sprintf_s(s, 20, "auto_%d", i);
		p.x = rand() * rand() % (rx - lx + 1) + lx;
		p.y = rand() * rand() % (uy - dy + 1) + dy;
		if (rand() * rand() % 100 <= PE) ie = 1;
		else ie = 0;
		if (insertR(L, p, s, NULL, 0, ie) == 0) i++;
		s = NULL;
	}
	for (i = 0; i < nc;) {
		*n1 = '\0';
		*n2 = '\0';
		r1 = rand() * rand() % nr;
		r2 = rand() * rand() % nr;
		sprintf_s(n1, 20, "auto_%d", r1);
		sprintf_s(n2, 20, "auto_%d", r2);
		if (insertC(L, n1, n2) == 0) i++;
	}
	return 1;
}

int D_Profiler(Room *L)
{
	int nr = 0, nit = -1;
	float T = 0;
	while (nr <= 0) {
		printf("Enter number of rooms: ");
		if (!getInt(&nr)) return 0;
		if (nr <= 0) puts("ERROR: Enter natural number!");
	}
	while (nit < 0) {
		printf("Enter number of iterations: ");
		if (!getInt(&nit)) return 0;
		if (nit < 0) puts("ERROR: Enter natural number or 0!");
	}
	T = profiler(nr, nit);
	printf("Time(ticks): %f\n", T);
	return 1;
}

float profiler(int nr, int iter)
{
	int i, r1, r2, el, iit;
	char *s = NULL;
	Room PLh = { { 0, 0 }, NULL, -1, NULL, NULL, NULL, -1, 0 };
	char n1[20], n2[20];
	Pos p;
	time_t ts, te;
	float tsum = 0;
	tsum = 0;
	int ie = 0;
	for (iit = 0; iit < iter; iit++) {
		el = PNE;
		for (i = 0; i < nr;) {
			s = (char*)malloc(20);
			*s = '\0';
			sprintf_s(s, 20, "auto_%d", i);
			p.x = i;
			p.y = 0;
			if (el) {
				ie = 1;
				el--; //if not (insert()), it'll be a bug!
			}
			else ie = 0;
			if (insertR(&PLh, p, s, NULL, 0, ie) == 0) i++;
			s = NULL;
		}
		for (i = 0; i < nr * PRC;) {
			*n1 = '\0';
			*n2 = '\0';
			r1 = rand() * rand() % nr;
			r2 = rand() * rand() % nr;
			sprintf_s(n1, 20, "auto_%d", r1);
			sprintf_s(n2, 20, "auto_%d", r2);
			if (insertC(&PLh, n1, n2) == 0) i++;
		}
		ts = clock();
		for (i = 0; i < nr; i++) {
			*n1 = '\0';
			sprintf_s(n1, 20, "auto_%d", i);
			Pfindexit(&PLh, n1);
		}
		te = clock();
		tsum += (float)(te - ts)/nr;
		clearmem(PLh.next);
		PLh.next = NULL;
	}
	return tsum;
}

int D_3TE(Room *L)
{
	int rc;
	if (!L->next) puts("Labirint is empty!");
	else {
		rc = transform_3te(L);
		if (rc) puts("Done!");
		else puts("Don't need changes.");
	}
	return 1;
}
/*
int checkexit(Room *L, Room *r, int dist);
int mark0(Room *L, Room *r, int dist);
int marker(Room *L, Room *r, int dist);
int count3ne(Room *L, Room *pre, Room *r, int dist);
int maxmark(Room *L, Room *r, int dist);
int createexit(Room *L, Room *r, int mm, int dist);
int counter0(Room *L, Room *r, int dist);
*/
int transform_3te(Room *L)
{
	Room *r = L->next;
	int allright = 1;
	int m;
	while (r) {
		if (!checkexit(L, r, 3)) {
			mark0(L, r, 3);
			marker(L, r, 3);
			m = maxmark(L, r, 3);
			createexit(L, r, m, 3);
			allright = 0;
		}
		r = r->next;
	}
	if (allright) return 0; //stopiter
	return 1;
}

int checkexit(Room *L, Room *r, int dist)
{
	AList *al = r->Adj;
	Room *rs = NULL;
	if (r->type == E) return 1;
	if (dist == 0) return 0;
	while (al) {
		rs = searchP(L, al->pos);
		if (checkexit(L, rs, dist - 1)) return 1;
		al = al->next;
	}
	return 0;
}

int mark0(Room *L, Room *r, int dist)
{
	AList *al = r->Adj;
	Room *rs = NULL;
	r->marks = 0;
	if (dist == 0) return 0;
	while (al) {
		rs = searchP(L, al->pos);
		mark0(L, rs, dist - 1);
		al = al->next;
	}
	return 1;
}

int counter0(Room *L, Room *r, int dist)
{
	AList *al = r->Adj;
	Room *rs = NULL;
	r->counted = 0;
	if (dist == 0) return 0;
	while (al) {
		rs = searchP(L, al->pos);
		counter0(L, rs, dist - 1);
		al = al->next;
	}
	return 1;
}

int marker(Room *L, Room *r, int dist)
{
	AList *al = r->Adj;
	Room *rs = NULL;
	counter0(L, r, 3);
	r->marks = count3ne(L, r, 3);
	if (dist == 0) return 0;
	while (al) {
		rs = searchP(L, al->pos);
		if (rs->marks == 0) marker(L, rs, dist - 1);
		al = al->next;
	}
	return 1;
}

int count3ne(Room *L, Room *r, int dist)
{
	int k = 0;
	int f = 0;
	AList *al = r->Adj;
	Room *rs = NULL;
	if (r->counted == 0) {
		r->counted = 1;
		if (!checkexit(L, r, 3)) k++;
	}
	if (dist == 0) return k;
	while (al) {
		rs = searchP(L, al->pos);
		k += count3ne(L, rs, dist - 1);
		al = al->next;
	}
	return k;
}

int maxmark(Room *L, Room *r, int dist)
{
	int m = 0, n;
	AList *al = r->Adj;
	Room *rs = NULL;
	m = r->marks;
	if (dist == 0) return m;
	while (al) {
		rs = searchP(L, al->pos);
		n = maxmark(L, rs, dist - 1);
		if (n > m) m = n;
		al = al->next;
	}
	return m;
}

int createexit(Room *L, Room *r, int mm, int dist)
{
	AList *al = r->Adj;
	Room *rs = NULL;
	if (r->marks == mm) {
		r->type = E;
		return 1;
	}
	if (dist == 0) return 0;
	while (al) {
		rs = searchP(L, al->pos);
		if (createexit(L, rs, mm, dist - 1)) return 1;
		al = al->next;
	}
	return 0;
}

int main()
{
	int cont = 1, dr;
	Room Lhead = { { 0, 0 }, NULL, -1, NULL, NULL, NULL, -1, 0 };
	srand(time(NULL));
	if (!D_Start(&Lhead)) {
		system("pause");
		return 1;
	}
	while (dr = dialog())
		if (!fptr[dr](&Lhead)) break;
	printf("Program is stopped.\n");
	fsave(&Lhead);
	clearmem(Lhead.next);
	Lhead.next = NULL;
	system("pause");
	return 0;
}

/*
FILE STRUCTURE

number of rooms
rooms:
{
	x
	y
	name length
	name (length+1)
	type
	Alist length
	Alist:
	{
		x
		y
	}
}
*/