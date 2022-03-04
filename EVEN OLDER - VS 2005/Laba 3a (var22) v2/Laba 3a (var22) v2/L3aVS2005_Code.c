#include <stdio.h>
#include <malloc.h>
#include <string.h>

char *getstr();
char *skipspace(char *s);
char *copyword(char *from, int maxlen, char *to);
char *skipword(char *s);
char *transformer(char *s);
//char *ENLARGER(char *sstr);


char *getstr()
{
	char *res = (char*)malloc(1);
	char buf[81];
	int n, len = 0;
	*res = '\0';
	do {
		n = scanf_s("%80[^\n]", buf, 81);
		if (n < 0) {
			free(res);
			res = NULL;
		};
		if (n == 0) scanf_s("%*c");
		if (n > 0) {
			len += strlen(buf);
			res = (char*)realloc(res, len + 1);
			strcat_s(res, len + 1, buf);
		}
		
	} while (n > 0);
	return res;
}

char *skipspace(char *s)
{
	int k = strspn(s, " \t");
	return s + k;
}

char *copyword(char *from, int maxlen, char *to)
{
	int k = strcspn(from, " \t");
	strncat_s(to, maxlen, from, k);
	to += k;
	*to++ = ' ';
	*to = '\0';
	return to;
}

char *skipword(char *s)
{
	int k = strcspn(s, " \t");
	return s + k;
}

char *transformer(char *s)
{
	//creating short string
	char *nstr = (char*)malloc(strlen(s) + 2);
	char *lstr = NULL;
	char *tmp = nstr;
	char *tmp2 = lstr;
	int newl = 0;
	int i = 0;
	*tmp = '\0';
	s = skipspace(s);
	while ((*s) != '\0') {
		tmp = copyword(s, strlen(s) + 2, tmp);
		s = skipword(s);
		s = skipspace(s);
	}
	if (tmp != nstr)
	{
		tmp--;
		*tmp = '\0';
	}
	nstr = (char*)realloc(nstr, strlen(nstr) + 1);
	if (strlen(nstr) % 80 == 0) newl = strlen(nstr)+1;
	else newl = (strlen(nstr)/80 + 1)*80+1;
	lstr = (char*)malloc(newl);
	tmp2 = lstr;
	tmp=nstr;
	*tmp2='\0';
	while (strlen(tmp)>=80)
	{
		strncat_s(lstr, newl, tmp, 80);
		tmp += 80;
		tmp2 += 80;
	}
	if (strlen(tmp) > 0)
	{
		i = 0;
		for (i = 0; i < ((80 - strlen(tmp)) / 2); i++) *(tmp2++) = ' ';
		*tmp2 = '\0';
		printf("%s\n", tmp);
		printf("%s\n", lstr);
		strcat_s(lstr, newl, tmp);
		tmp2 += strlen(tmp);
		for (i = 0; i < ((80 - strlen(tmp)) / 2 + ((80 - strlen(tmp)) % 2)); i++) *(tmp2++) = ' ';
		*tmp2 = '\0';
	}
	free(nstr);
	nstr = NULL;
	return lstr;
}
/*
char *ENLARGER(char *sstr)
{
	char *lstr = NULL;
	int i = 0;
	lstr = (char*)malloc(81);
	char *tmp = lstr;
	*tmp = '\0';
	for (i = 0; i < ((80 - strlen(sstr)) / 2); i++) *(tmp++) = ' ';
	*tmp = '\0';
	strcat_s(tmp, 81, sstr);
	tmp += strlen(sstr);
	for (i = 0; i < ((80 - strlen(sstr)) / 2 + ((80 - strlen(sstr)) % 2)); i++) *(tmp++) = ' ';
	*tmp = '\0';
	return lstr;
}
*/

int main()
{
	int ender = 1;
	int cont = 1;
	char *s = NULL;
	char *trs = NULL;
	do {
		printf("Enter your string:\n");
		s = getstr();
		if (s != NULL) {
			trs = transformer(s);
			printf("Resultant string:\n%s\n", trs);
			free(s);
			free(trs);
		}
		else {
			printf("End of file reached. Program is stopped.\n");
			cont = 0;
		}
	}
	while (cont == 1);
	//system("pause");
	//END
	printf("Close the program, please.\n");
	scanf_s("%d", ender);
	return 0;
}