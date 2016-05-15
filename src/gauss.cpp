/* **********************************************

  File Name: gauss.cpp

  Author: zhengdongjian@tju.edu.cn

  Created Time: Thu 12 May 2016 05:39:35 PM CST

*********************************************** */
#ifdef UNIX
#undef UNIX
#endif
#ifdef __unix__
#define UNIX
#include <sys/time.h>
#endif
#include <bits/stdc++.h>
using namespace std;

typedef vector<double> Line; /* one line */
typedef vector<Line> Matrix;

const int LINE_BUFFER_SIZE = 8192;
const double EPS = 1e-6;
const int MAX = 256; //max size

double det[MAX][MAX];
int size;

//Matrix det;

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

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file_name> <output_file_name>\nor\t%s <input_file_name>\n\n", argv[0], argv[0]);
        exit(1);
    }
    if (argc > 2) {
        freopen(argv[2], "a", stdout);
    }

    input(argv[1]);

#ifdef UNIX
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
#else
    unsigned int begin = time(0);
#endif
    printf("result = %f\n", gauss()); /* 高斯消元 */
#ifdef UNIX
    gettimeofday(&end, NULL);
    printf("time used: %.2f us\n", ((end.tv_sec - begin.tv_sec) * 1e6 + (end.tv_usec - begin.tv_usec)));
#else
    unsigned int end = time(0);
    printf("time used: %u ms\n", end - begin);
#endif
    return 0;
}

void input(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "open file %s failed!\n", filename);
        exit(1);
    }

    /* TODO: fix buffer-size */
    /* read one line each time */
    char buffer[LINE_BUFFER_SIZE];
    //Line line;
    int line = 0;
    while (fgets(buffer, LINE_BUFFER_SIZE, file) != NULL) {
        //printf("|%s|\n", buffer);
        int len = strlen(buffer);
        //line.clear();
        size = 0;
        for (int i = 0, j = 0; i < len; ++i) {
            if (!isdigit(buffer[i]) && buffer[i] != '.' && buffer[i] != '-') {
                buffer[i] = '\0';
                det[line][size++] = atof(buffer + j);
                //line.push_back(atof(buffer + j));
                ++size;
                while (i < len && !isdigit(buffer[i]) && buffer[i] != '-' && buffer[i] != '-') {
                    ++i;
                }
                j = i;
            }
        }
        ++line;
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

double gauss() {
    //assert(det.size() == det[0].size());

    bool negtive = false;
    for (int i = 0; i < size; ++i) {
        //选主元
        if (fabs(det[i][i]) < EPS) {
            for (int j = i + 1; j < size; ++j) {
                if (fabs(det[j][i]) > EPS) {
                    swap(det[i], det[j]);
                    negtive = !negtive;
                    break;
                }
            }
        }
        
        if (fabs(det[i][i]) < EPS) {
            return 0;
        }

        //消元
        for (int j = i + 1; j < size; ++j) { //row j
            if (fabs(det[j][i]) < EPS) {
                continue; //already finish
            }
            double times = -det[j][i] / det[i][i];
            for (int k = i; k < size; ++k) {
                det[j][k] += det[i][k] * times;
            }
        }
    }

#ifdef DEBUG
    puts("res:");
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            printf("%f ", det[i][j]);
        }
        puts("");
    }
#endif

    double res = negtive ? -1.0 : 1.0;
    for (int i = 0; i < size; ++i) {
        res *= det[i][i];
    }
    return res;
}
