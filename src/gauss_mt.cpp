/* **********************************************

  File Name: gauss_mt.cpp

  Author: zhengdongjian@tju.edu.cn

  Created Time: Thu 12 May 2016 06:56:36 PM CST

*********************************************** */
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <bits/stdc++.h>
using namespace std;

//#define DEBUG

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

/**
 * 线程id数组
 */
const int MAX_THREAD_NUM = 1024; //最大线程数
pthread_t threads[MAX_THREAD_NUM];
int total_thread = MAX_THREAD_NUM; //使用的线程数
int rows_each_thread; //每个线程消多少行
int current_col; //当前消第几列
int total_col; //总列数

void* gao(void* tid) {
    int id = *(unsigned int*)tid; //id从0开始编号
    int rows_to_do = (id == total_col - 1) ? (total_col - current_col - 1) % rows_each_thread : rows_each_thread;
    if (rows_to_do == 0) rows_to_do = rows_each_thread;
    int begin = current_col + 1 + id * rows_each_thread;
    int end = begin + rows_to_do;
    double mother = det[current_col][current_col];
    for (int current_row = begin; current_row < end; ++current_row) {
        if (fabs(det[current_row][current_col]) > EPS) {
            double times = -det[current_row][current_col] / mother;
            for (int i = current_col; i < total_col; ++i) {
                det[current_row][i] += times * det[current_col][i];
            }
        }
    }
    return NULL;
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
    printf("result = %f\n", gauss()); /* 高斯消元 */
    gettimeofday(&end, NULL);
    printf("time used: %.2f us\n", ((end.tv_sec - begin.tv_sec) * 1e6 + (end.tv_usec - begin.tv_usec)));
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
    total_col = size;
    static int idx[MAX_THREAD_NUM];
    for (int j = 0; j < total_thread; ++j) {
        idx[j] = j;
    }
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
        current_col = i;
        total_thread = (total_col - i) >> 1;
        rows_each_thread = (total_col - i - 1 + total_thread - 1) / total_thread;
        for (int j = 0; j < total_thread; ++j) {
            pthread_create(threads + j, NULL, gao, (void*)(idx + j));
        }
        for (int j = 0; j < total_thread; ++j) {
            pthread_join(threads[j], NULL); //sync
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
