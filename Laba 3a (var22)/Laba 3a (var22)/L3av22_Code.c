#include <stdio.h>
#include <malloc.h>
#include <string.h>

char *getstr();
char *skipspace(char *s);
char *copyword(char *from, int maxlen, char *to);
char *skipword(char *s);
char *transformatrix(char *s);
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

char *transformatrix(char *s)
{
	//creating short string
	char *nstr = (char*)malloc(strlen(s) + 2);
	char *lstr = NULL;
	char *tmp = nstr;
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
	if (strlen(nstr) < 80)
	{
		int i = 0;
		lstr = (char*)malloc(81);
		tmp = lstr;
		*tmp = '\0';
		for (i = 0; i < ((80 - strlen(nstr)) / 2); i++) *(tmp++) = ' ';
		*tmp = '\0';
		strcat_s(lstr, 81, nstr);
		tmp += strlen(nstr);
		for (i = 0; i < ((80 - strlen(nstr)) / 2 + ((80 - strlen(nstr)) % 2)); i++) *(tmp++) = ' ';
		*tmp = '\0';
		free(nstr);
		nstr = NULL;
		return lstr;
	}
	else return nstr;
}

int main()
{
	int cont = 1;
	char *s = NULL;
	char *trs = NULL;
	do {
		printf("Enter your string:\n");
		s = getstr();
		if (s != NULL) {
			trs = transformatrix(s);
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
	system("pause");
}