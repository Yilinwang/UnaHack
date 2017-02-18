#include <stdio.h>
#include <string.h>

int mymin(int a, int b) { return a < b? a: b; }

int myabs(int a) { return a > 0? a: -a; }

int dtw(int a[], int b[]) {
    int d[5][50] = {{0}};
    memset(d, 0x7f, sizeof(int)*5*50);
    d[0][0] = myabs(a[0] - b[0]);
    for(int i = 1; i < 50; i++) {
        d[0][i] = myabs(a[0] - b[i]);
        int min = 2147483647;
        for(int j = i-1; j >= 0 && j > i-5; j--) {
            min = min > d[0][j]? d[0][j]: min;
        }
        d[0][i] += min;
    }
    for(int i = 1; i < 50; i++) {
        for(int j = 0; j < 50; j++) {
            int min = 2147483647;
            for(int l = 0; l < 5; l++) {
                for(int k = j; k >= 0 && k > j-5; k--) {
                    if(l == i%5 && k == j) continue;
                    min = min > d[l][k]? d[l][k]: min;
                }
            }
            d[i%5][j] = myabs(a[i] - b[j]) + min;
        }
        /*
        for(int x = 0; x < 5; x++) {
            for(int y = 0; y < 50; y++) {
                printf("%d ", d[x][y]);
            }
            printf("\n");
        }
        */
    }
    return d[4][49];
}

int main() {
    FILE* fp = fopen("pattern.csv", "r");
    int a[8][50];
    
    for(int i = 0; i < 50; i++)
        fscanf(fp, "%d,%d,%d,%d,%d,%d,%d,%d\n", &a[0][i], &a[1][i], &a[2][i], &a[3][i], &a[4][i], &a[5][i], &a[6][i], &a[7][i]);
    printf("        ");
    for(int i = 0; i < 8; i++) {
        printf("%8d", i);
    }
    printf("\n");
    for(int i = 0; i < 8; i++) {
        printf("%8d", i);
        for(int j = 0; j < 8; j++) {
            printf("%8d", dtw(a[i], a[j]));
        }
        printf("\n");
    }
    dtw(a[6], a[5]);
    return 0;
}
