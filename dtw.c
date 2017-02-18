#include <stdio.h>

int mymin(int a, int b) { return a < b? a: b; }

int myabs(int a) { return a > 0? a: -a; }

int dtw(int a[], int b[]) {
    int d[2][50] = {{0}};
    int cur = 0;
    for(int i = 0; i < 50; i++) {
        for(int j = 0; j < 50; j++) {
            d[cur][j] = myabs(a[i] - b[j]);
            if(j > 0) d[cur][j] += mymin(d[cur][j-1], mymin(d[!cur][j], d[!cur][j-1]));
            else d[cur][j] += d[!cur][j];
        }
        cur = !cur;
    }
    return d[!cur][49];
}

int main() {
    FILE* fp = fopen("pattern.csv", "r");
    int a[8][50];
    
    for(int i = 0; i < 50; i++)
        fscanf(fp, "%d,%d,%d,%d,%d,%d,%d,%d\n", &a[0][i], &a[1][i], &a[2][i], &a[3][i], &a[4][i], &a[5][i], &a[6][i], &a[7][i]);
    for(int i = 0; i < 7; i++) {
        for(int j = i+1; j < 8; j++) {
            printf("%d, %d dtw: %d\n", i, j, dtw(a[i], a[j]));
        }
    }
    return 0;
}
