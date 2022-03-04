#include <stdio.h>
#include <malloc.h>

typedef struct listitem {
	char c;
	struct listitem *next;
} Item;

int getlist(Item **plist);
void putlist(Item *list);
void deletelist(Item **plist);
void delsp(Item *eow);
void delfirst(Item **plist);
Item *addsp(Item *pos);
Item *skipword(Item *eosp);
Item *skippunc(Item *eow);
Item *newstr(Item *point);
void transformlist(Item **plist);


int getlist(Item **plist)
{
	char buf[81], *str;
	Item head = { '*', NULL };
	Item *last = &head;
	int n, rs = 1;
	printf("Enter your string:\n");
	do {
		n = scanf_s("%80[^\n]", buf, 81);
		if (n < 0) {
			deletelist(&head.next);
			head.next = NULL;
			rs = 0;
		}
		else if (n > 0) {
			for (str = buf; *str != '\0'; ++str) {
				last->next = (Item*)malloc(sizeof(Item));
				last = last->next;
				last->c = *str;
			}
			last->next = NULL;
		}
		else {
			scanf_s("%*c");
		};
	} while (n > 0);
	*plist = head.next;
	return rs;
}

void putlist(Item *list)
{
	Item *pos;
	printf("Resultant strings:\n");
	if (list != NULL)
	{
		pos = list;
		while ((pos->next) != NULL) {
			printf("%c", pos->c);
			pos = pos->next;
		}
		printf("%c", pos->c);
		if ((pos->c) != '\n') printf("\n");
	}
	else printf("\n");
}

void deletelist(Item **plist)
{
	if (*plist != NULL) {
		deletelist(&((*plist)->next));
		free(*plist);
		*plist = NULL;
	}
}

void delsp(Item *eow)
{
	Item *pos = eow;
	Item *buf;
	int cont = 1;
	do {
		buf = eow->next->next;
		free(eow->next);
		eow->next = buf;
		if (eow->next == NULL) cont = 0;
		else if ((eow->next->c != ' ') && (eow->next->c != '\t')) cont = 0;
	} while (cont);
}

void delfirst(Item **plist)
{
	Item *pbuf;
	int cont = 1;
	if (((*plist)->c != ' ') && ((*plist)->c != '\t')) cont = 0;
	while (cont) {
		pbuf = (*plist)->next;
		free(*plist);
		*plist = pbuf;
		if ((*plist) == NULL) cont = 0;
		else if (((*plist)->c != ' ') && ((*plist)->c != '\t')) cont = 0;
	}
}

Item *addsp(Item *pos)
{
	Item *buf = pos->next;
	pos->next = (Item*)malloc(sizeof(Item)); //ìîæåò, ñíà÷àëà null?
	pos->next->c = ' ';
	pos->next->next = buf;
	return(pos->next);
}

Item *skipword(Item *eosp)
{
	Item *pos = eosp;
	int cont = 1;
	do {
		pos = pos->next;
		if (pos->next == NULL) cont = 0;
		else if ((pos->next->c == ',') || (pos->next->c == ';') || (pos->next->c == '.') || (pos->next->c == ' ') || (pos->next->c == '\t')) cont = 0;
	} while (cont);
	return(pos);
}

Item *skippunc(Item *eow)
{
	Item *pos = eow;
	int cont = 1;
	do {
		pos = pos->next;
		if (pos->next == NULL) cont = 0;
		else if ((pos->next->c != ',') && (pos->next->c != ';')) cont = 0;
	} while (cont);
	return(pos);
}

Item *newstr(Item *point) //ÍÅ ÇÀÁÛÒÜ ÑÌÎÒÐÅÒÜ ÍÀ ÒÎ×ÊÓ!!!!!!
{

	Item *buf = point->next;
	point->next = (Item*)malloc(sizeof(Item)); //ìîæåò, ñíà÷àëà null?
	point->next->c = '\n';
	point->next->next = buf;
	return(point->next);
}

void transformlist(Item **plist)
{
	Item *pos;
	int needsp = 0, justn = 1;
	if (*plist != NULL) delfirst(plist);
	if (*plist != NULL)
	{
		pos = *plist;
		if (pos->c == '.') pos = newstr(pos);
		while (pos->next != NULL) {
			if (pos->next->c == '.')  {
				pos = pos->next;
				needsp = 0;
				justn = 1;
				if (pos->next != NULL) pos = newstr(pos);
			}
			else if ((pos->next->c == ',') || (pos->next->c == ';')) {
				pos = skippunc(pos);
				if (!justn) needsp = 1;
			}
			else if ((pos->next->c == ' ') || (pos->next->c == '\t')) {
				delsp(pos);
				if (!justn) needsp = 1;
			}
			else {
				if (needsp) pos = addsp(pos);
				needsp = 0;
				justn = 0;
				pos = skipword(pos);
			}
		}
	}
}

int main()
{
	Item *s;
	while (getlist(&s)) {
		transformlist(&s);
		putlist(s);
		deletelist(&s);
	}
	printf("End of file reached. Program stopped.\n");
	system("pause");
	return 0;
}