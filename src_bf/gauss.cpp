/* **********************************************

  File Name: gauss.cpp

  Author: zhengdongjian@tju.edu.cn

  Created Time: Sat, May 14, 2016 10:10:55 PM

*********************************************** */
#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;

/**
 * !!!
 * complex: O(n^n)
 */
enum ERROR {
    DATA_BUFFER_OVERFLOW = 1;
};

const int LINE_BUFFER_SIZE = 8192; /* each time read one line, with buffer 8k */
const double EPS = 1e-6;
const int MAX = 9; //max matrix size
const static AR[MAX + 1] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880/*, 3628800*/};

double det[MAX][MAX];
int size; //runtime matrix size

int seq[AR[MAX]][MAX];
int source[MAX];
bool row_vis[MAX];
int seq_size;

/**
 * [Func]input
 * parm filename: input file name
 */
void input(const char* filename);

/**
 * [Func]gauss
 * return: value of the det
 */
double gauss();

/**
 * depth first search, get the number sequences
 * @param depth the row to check currently
 */
void dfs(int depth) {
    if (depth == size) {
        for (int i = 0; i < size; ++i) {
            seq[seq_size][i] = source[i];
        }
        seq_size++;
        return;
    }
    for (int i = 0; i < size; ++i) {
        if (!row_size[i]) {
            row_size[i] = true;
            source[depth] = i;
            dfs(depth + 1);
            row_size[i] = false;
        }
    }
}

/**
 * generate sequences
 */
void generate() {
    seq_size = 0;
    for (int i = 0; i < size; ++i) {
        source[i] = i;
    }
    memset(row_vis, false, sizeof(row_vis));
    dfs(0);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file_name> <output_file_name>\nor\t%s <input_file_name>\n\n", argv[0], argv[0]);
        exit(1);
    }
    if (argc > 2) {
        freopen(argv[2], "a", stdout);
    }

    input(argv[1]);

    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    //generate sequences
    generate();
    printf("result = %f\n", gauss()); /* 高斯消元 */

    gettimeofday(&end, NULL);
    printf("time used: %.2f us\n", ((end.tv_sec - begin.tv_sec) * 1e6 + (end.tv_usec - begin.tv_usec)));

    return 0;
}

int input(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "open file %s failed!\n", filename);
        exit(1);
    }

    /* TODO: fix buffer-size */
    /* read one line each time */
    char buffer[LINE_BUFFER_SIZE];
    //Line line;
    size = 0;
    while (fgets(buffer, LINE_BUFFER_SIZE, file) != NULL) {
        if (size > MAX) {
            return -1;
            //throw DATA_BUFFER_OVERFLOW;
        }
        //printf("|%s|\n", buffer);
        int len = strlen(buffer);
        int line = 0;
        //line.clear();
        for (int i = 0, j = 0; i < len; ++i) {
            if (!isdigit(buffer[i]) && buffer[i] != '.' && buffer[i] != '-') {
                buffer[i] = '\0';
                det[line++][size] = atof(buffer + j);
                //line.push_back(atof(buffer + j));
                while (i < len && !isdigit(buffer[i]) && buffer[i] != '-' && buffer[i] != '-') {
                    ++i;
                }
                j = i;
            }
        }
        ++size;
        //det.push_back(line);
    }
#ifdef DEBUG
    puts("read:");
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            printf("%f ", det[i][j]);
        }
        puts("");
    }
#endif
    fclose(file);
}

bool vis_col[MAX]; //flag of if col[i] has been choosen.

//return: (-1)^n
inline int pow1(int n) {
    return (n & 1) ? -1 : 1;
}

double gauss() {
    double res = 0.0, tmp;
    for (int i = 0; i < seq_size; ++i) {
        tmp = pow1(i);
        for (int j = 0; j < size; ++j) {
            tmp *= det[j][seq[i][j]];
        }
        res += tmp;
    }
    return res;
}
