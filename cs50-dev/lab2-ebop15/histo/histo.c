// 16 bins
// each start as size 1
// do this by having a dynamic variable as range
// record the maximum
// when the maximum gets updated, double every bin range?

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>


int main(const int argc, const char *argv[]){
    int range = 1;
    int ceiling = (range * 16);
    int current_input;
    int bins[16][1];
    printf("16 bins of size %d for range [0: %d] \n", range, ceiling);
    while(scanf("%d", &current_input) != EOF){
        if(current_input >= 0){
            while(ceiling - 1 < current_input){
                ceiling = (ceiling * 2);
                range = range * 2;
                // put the things into new bins
                int i = 0;
                while(i <= 14){
                    bins[i / 2][0] = bins[i][0] + bins[i + 1][0];
                    i += 2;
                }
                int k = 8;
                while(k < 16){
                    bins[k][0] = 0;
                    k += 1;
                }
                printf("16 bins of size %d for range [0: %d] \n", range, ceiling);       
            }
            int bin_number = current_input / range;
            bins[bin_number][0] += 1;
        }
    }
    int j = 0;
    while(j < 16){
        int k = j * range;
        int l = (j + 1) * range;
        printf("Bin for range [%d: %d]: ", k, l - 1);
        printf("%.*s \n", bins[j][0], "*******************************************************************");
        j += 1;
    }
    return 0;
}