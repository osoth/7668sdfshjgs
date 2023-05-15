/*
 * Paralleles Rechnen, Uebung 1
 * Matrix-Multiplikation nach Schulmethode 
 *
 * vim: expandtab shiftwidth=4 tabstop=4
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <immintrin.h>

//#define DEBUG

#define TIME_DIFF(timer1, timer2) \
(( timer2.tv_sec * 1.0E+9 + timer2.tv_nsec ) - \
( timer1.tv_sec * 1.0E+9 + timer1.tv_nsec )) / 1.0E+9

typedef int32_t matrix_elem_t;

typedef struct {
    int rows, cols;
    matrix_elem_t *data;
} matrix_t;


bool read_matrix(char *filepath, matrix_t* matrix)
{
    int i, j, x;
    FILE *fp;
    if ((fp = fopen(filepath, "r")) == NULL) {
        perror("Can't open file!");
        exit(EXIT_SUCCESS);
    }

    fscanf(fp, "%d", &matrix->rows);
    fscanf(fp, "%d", &matrix->cols);

    matrix->data = aligned_alloc(32,matrix->rows * matrix->cols*sizeof(matrix_elem_t));
    if (matrix->data == NULL) {
        return false;
    }

    for (i = 0; i < matrix->rows; ++i) {
        for (j = 0; j < matrix->cols; ++j) {
            fscanf(fp, "%d\t", &x);
            matrix->data[(matrix->cols * i) + j] = x;
        }
        fscanf(fp, "\n");
    }

    fclose(fp);

    return true;
}

matrix_elem_t print_matrix(matrix_t m){
    int i, j;
    matrix_elem_t sum;

    sum = 0;
    for (i = 0; i < m.rows; ++i) {
        for (j = 0; j < m.cols; ++j) {
            sum += m.data[(i * m.cols) + j];
#ifdef DEBUG
            printf("%d\t ", m.data[(i * m.cols) + j]);
        }
        printf("\n");
#else
         }
#endif
    }
    return sum;
}

void matrix_mult(matrix_t* a, matrix_t* b, matrix_t* r)
{
    long i, j, k, y;

    for (i = 0; i < r->rows; ++i) {
        for (j = 0; j < r->cols; ++j) {
            y = 0;
            for (k = 0; k < a->cols; ++k) {
                y += a->data[i * a->cols + k] * b->data[k * b->cols + j];
            }   
            r->data[i * r->cols + j] = y;
        }
    }
}


bool matrix_mult_simple(matrix_t* a, matrix_t* b, matrix_t* r)
{
    if (a->cols != b->rows) {
        return false;
    }

    r->rows = a->rows;
    r->cols = b->cols;
    r->data = calloc(r->rows * r->cols, sizeof(matrix_elem_t));

    if (r->data == NULL) {
        return false;
    }

    struct timespec t1,t2;
    
    clock_gettime(CLOCK_MONOTONIC,&t1);
    matrix_mult(a, b, r);
    clock_gettime(CLOCK_MONOTONIC,&t2);

    printf("time %lf \n",TIME_DIFF(t1, t2));
    return true;
}


int main(int argc, char* argv[])
{
    matrix_t a = { 0, 0, NULL };
    matrix_t b = a;
    matrix_t r = a;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!read_matrix(argv[1], &a)) {
        fputs("could not read input matrix A", stderr);
        return EXIT_FAILURE;
    }

    if (!read_matrix(argv[2], &b)) {
        fputs("could not read input matrix B", stderr);
        return EXIT_FAILURE;
    }

    if (matrix_mult_simple(&a, &b, &r)) {
        printf("Pruefsumme: %d\n",print_matrix(r));
    } else {
        fputs("could not multiply: mismatch between number of rows and columns in input matricess.", stderr);
    }

    free(a.data);
    free(b.data);
    free(r.data);

    return EXIT_SUCCESS;
}

