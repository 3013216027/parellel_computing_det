/* **********************************************

  File Name: gauss_mt.cpp

  Author: zhengdongjian@tju.edu.cn

  Created Time: Tue, May 17, 2016  3:30:03 PM

*********************************************** */
#include <bits/stdc++.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

/**
 * !!!
 * complex: O(n^n)
 */
enum ERROR {
    DATA_BUFFER_OVERFLOW = 1
};

//#define TEST

const int LINE_BUFFER_SIZE = 8192; /* each time read one line, with buffer 8k */
const double EPS = 1e-6;
const int MAX = 11; //max matrix size
const static int AR[MAX + 1] = {
	1, //0!
	1, //1!
	2, //2!
	6, //3!
	24,
	120,
	720, //6!
	5040,
	40320,
	362880, //9!
	3628800, //10!
	39916800 //11!
};//全排列数

double det[MAX][MAX]; //行列式
int size; //runtime matrix size

int seq[39916800 * MAX]; //全排列
int source[MAX]; //排列数暂存数组
bool row_vis[MAX]; //排列生成时的标记
int seq_size;

/**
 * [Func]input
 * parm filename: input file name
 */
int input(const char* filename);

/**
 * [Func]gauss
 * return: value of the det
 */
double gauss(int num_thread = 16);

/**
 * depth first search, get the number sequences
 * @param depth the row to check currently
 */
void dfs(int depth) {
    if (depth == size) {
        for (int i = 0; i < size; ++i) {
            seq[seq_size * MAX + i] = source[i];
        }
        seq_size++;
        return;
    }
    for (int i = 0; i < size; ++i) {
        if (!row_vis[i]) {
            row_vis[i] = true;
            source[depth] = i;
            dfs(depth + 1);
            row_vis[i] = false;
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
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file_name> <thread_num> [<output_file_name>].\n\n", argv[0]);
        exit(1);
    }
    if (argc > 3) {
        freopen(argv[3], "a", stdout);
    }

    input(argv[1]);

    //generate sequences
    generate();
    struct timeval begin, end;
#ifdef TEST
	for (int i = 1; i <= 128; i <<= 1) {
		printf("num_thread = %d\n", i);
#endif
    gettimeofday(&begin, NULL);
#ifdef TEST
    printf("result = %f\n", gauss(i));
#else
    fprintf(stderr, "result = %f\n", gauss(atoi(argv[2])));
    //printf("result = %f\n", gauss(atoi(argv[2]));
#endif

    gettimeofday(&end, NULL);
    printf("%.2f\n", ((end.tv_sec - begin.tv_sec) * 1e6 + (end.tv_usec - begin.tv_usec)));
    //printf("time used: %.2f us\n", ((end.tv_sec - begin.tv_sec) * 1e6 + (end.tv_usec - begin.tv_usec)));
#ifdef TEST
	}
#endif

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

	return 0; //no error
}

bool vis_col[MAX]; //flag of if col[i] has been choosen.

//return: (-1)^n
inline int pow1(int n) {
    return (n & 1) ? -1 : 1;
}

/**
 * data struct for parellel computing
 */
const int MAX_THREADS = 32;
pthread_t threads[MAX_THREADS];
int ids[MAX_THREADS]; //tell the specific thread his ID
double res; //temporary parellel result array
sem_t lock; //lock for res
int total_threads = 1; // how much threads do we need?
int total_of_each; // how much work for each thread?

void* work(void* id) {
	int me = *(int*)id; //my id
	int start = me * total_of_each + 0;
	int end = start + total_of_each; // [start, end)
	if (end > seq_size) {
		end = seq_size;
	}

	double my_res = 0;
	while (start++ < end) {
		//do with seq_{start}
		double tmp = pow1(start);
		for (int j = 0; j < size; ++j) {
			tmp *= det[j][seq[start * MAX + j]];
		}
		my_res += tmp;
	}
	sem_wait(&lock); //P
	res += my_res;
	sem_post(&lock); //V

	return NULL;
}

double gauss(int num_thread) {
	/* decide how much threads to work */
	if (seq_size < total_threads) {
		seq_size = total_threads;
	}
	//fix threads_num
	total_threads = num_thread;
	total_threads = std::min(MAX_THREADS, total_threads);
	total_threads = std::min(seq_size, total_threads);
	total_of_each = (seq_size + total_threads - 1) / total_threads;
	res = 0.0;
	sem_init(&lock, 0, 1); //init with value of 1, and shared
    for (int i = 0; i < total_threads; ++i) {
		ids[i] = i;
		pthread_create(threads + i, NULL, work, (void*)(ids + i));
    }
	for (int i = 0; i < total_threads; ++i) {
		pthread_join(threads[i], NULL);
	}
    return res;
}
