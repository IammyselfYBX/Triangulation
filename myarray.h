#ifndef MYARRAY_H
#define MYARRAY_H

#include "xmalloc.h"

/* 
 * 向量的动态分配和释放
 *  v 是向量名称，n 是向量长度
 * 
 * 用法：
 *     make_vector(v, n);
 *     free_vector(v);
 *     print_vector(fmt, v, n);
 */
#define make_vector(v, n) ((v) = xmalloc((n) * sizeof *(v))) 
#define free_vector(v)  do { free(v); v = NULL; } while (0)
#define print_vactor(fmt, v, n) do {                    \
    size_t print_vactor_loop_counter;                   \
    for (print_vactor_loop_counter = 0;                 \
         print_vactor_loop_counter < (n);               \
         print_vactor_loop_counter++)                   \
        printf(fmt, (v)[print_vactor_loop_counter]);    \
    printf("\n");                                       \
} while (0)

/*
 * 矩阵的动态分配和释放
 *  a 是矩阵名称，m 是行数，n 是列数
 * 
 * 用法：
 *    make_matrix(a, m, n);
 *    free_matrix(a);
 *    print_matrix(fmt, a, m, n);
 */
#define make_matrix(a, m, n) do {                        \
    size_t make_matrix_loop_counter;                     \
    make_vector(a, (m) + 1);                             \
    for (make_matrix_loop_counter = 0;                   \
         make_matrix_loop_counter < (m);                 \
         make_matrix_loop_counter++)                     \
        make_vector((a)[make_matrix_loop_counter], (n)); \
    (a)[m] = NULL;                                       \
} while (0)
#define free_matrix(a) do {                             \
    if (a != NULL){                                     \
        size_t make_matrix_loop_counter;                \
        for (make_matrix_loop_counter =0;               \
                (a)[make_matrix_loop_counter] != NULL;  \
                make_matrix_loop_counter++)             \
            free_vector((a)[make_matrix_loop_counter]); \
        free_vector(a);                                 \
        a = NULL;                                       \
    }                                                   \
} while (0)
#define print_matrix(fmt, a, m, n) do {                            \
    size_t print_matrix_loop_row_counter;                          \
    for (print_matrix_loop_row_counter = 0;                        \
         print_matrix_loop_row_counter < (m);                      \
         print_matrix_loop_row_counter++)                          \
         print_vactor(fmt, (a)[print_matrix_loop_row_counter], n); \
} while (0)
#endif