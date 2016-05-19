/* **********************************************

  File Name: generator.cpp

  Author: zhengdongjian@tju.edu.cn

  Created Time: Thu 12 May 2016 07:21:11 PM CST

*********************************************** */
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <data_max> <data_size>\n", argv[0]);
        exit(1);
    }
    int mod = atoi(argv[1]);
    int size = atoi(argv[2]);
    //fprintf(stderr, "generator -> size = %d\n", size);
    srand(time(0));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            printf("%d ", rand() % mod);
        }
        puts("");
    }
    return 0;
}
