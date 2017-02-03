#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { C = 5, A = 26, SZ = A * A * A * A, N = 1000 };
double freq[SZ];
int posx[256], posy[256];
char t[C][C];
char ciphert[N];
char buf[N];

double min(double x, double y) { return x < y ? x : y; }
void swap(char *p, char *q) { char t = *p; *p = *q; *q = t; }

int getidx(const char *s) {
	int ans = 0;
	for (int i = 0; s[i]; ++i) ans = 26 * ans + s[i] - 'A';
	return ans;
}

void load(void) {
	FILE *fp = fopen("freq4", "r");
	assert(fp != NULL);

	for (int i = 0; i < SZ; ++i) freq[i] = 1;

	char line[64];	
	int tot = 0;
	while (fgets(line, sizeof line, fp)) {
		char *p = strchr(line, ' ');
		assert(p != NULL);
		int count = atoi(p + 1);
		*p = 0;
		tot += count;
		freq[getidx(line)] += count;
	}

	for (int i = 0; i < SZ; ++i) freq[i] = log(freq[i]) - log(tot);

	fclose(fp);
}

void decode1(int c1, int c2, char *p1, char *p2) {
	int x1 = posx[c1], y1 = posy[c1];
	int x2 = posx[c2], y2 = posy[c2];
	if (x1 == x2) *p1 = t[(y1 - 1 + C) % C][x1], *p2 = t[(y2 - 1 + C) % C][x2];
	else if (y1 == y2) *p1 = t[y1][(x1 - 1 + C) % C], *p2 = t[y2][(x2 - 1 + C) % C];
	else *p1 = t[y1][x2], *p2 = t[y2][x1];
}

void decode(void) {
	int k = 0;
	for (int i = 0; ciphert[i]; i += 2) {
		char p1, p2;
		decode1(ciphert[i], ciphert[i + 1], &p1, &p2);
		buf[k++] = p1;
		buf[k++] = p2;
	}
	buf[k] = 0;
}

double getscore(void) {
	decode();
	double ans = 0;
	int i = 4;

	while (true) {
		char tmp = buf[i];
		buf[i] = 0;
		ans += freq[getidx(buf + i - 4)];
		buf[i] = tmp;
		if (!buf[i]) break;
		i++;
	}

	return ans;
}

void makemove(int x1, int y1, int x2, int y2) {
	posx[t[y1][x1]] = x2;
	posy[t[y1][x1]] = y2;
	posx[t[y2][x2]] = x1;
	posy[t[y2][x2]] = y1;
	swap(&t[y1][x1], &t[y2][x2]);
}

int main(void) {
	srand(time(NULL));
	load();
	scanf("%s", ciphert);
	int it = 0;
	
	while (true) {
		char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVXYZ";

		for (int i = 0; i < C * C; ++i) {
			int nb = C * C - i;
			int key = rand() % nb;
			//swap(alphabet + key, alphabet + nb - 1);
			t[i / C][i % C] = alphabet[nb - 1];
			posx[alphabet[nb - 1]] = i % C;
			posy[alphabet[nb - 1]] = i / C;
		}
		for (int i = 0; i < C; ++i, puts(""))
			for (int j = 0; j < C; ++j)
				printf("%c ", t[i][j]);

		decode();
		puts(buf);

		double score = getscore();
		printf("%f\n",getscore());
		for (double T = 500; T > 0; T -= 1) {
			for (int c = 0; c < 50000; ++c) {
				int y1 = rand() % C, x1 = rand() % C, y2 = rand() % C, x2 = rand() % C;
				makemove(x1, y1, x2, y2);
				double newscore = getscore();
				double df = newscore - score;
				if (df > 0 || (df < 0 && 1. * rand() / RAND_MAX < exp(df / T))) {
					score = newscore;
				}
				else {
					makemove(x1, y1, x2, y2);
				}
			}

			decode();
			printf("Iteration %d, T = %f, score = %.12f, message = %s\n", it, T, score, buf);
		}

		it++;
	}

	return 0;
}
