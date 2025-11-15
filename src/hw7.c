#include "hw7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

static matrix_sf *alloc_matrix(char name, unsigned int rows, unsigned int cols) {
    matrix_sf *m = malloc(sizeof(matrix_sf) + rows * cols * sizeof(int));
    if (!m) return NULL;
    m->name = name;
    m->num_rows = rows;
    m->num_cols = cols;
    return m;
}

bst_sf* insert_bst_sf(matrix_sf *mat, bst_sf *root) {
    if (!mat) return root;
    if (!root) {
        bst_sf *node = malloc(sizeof(bst_sf));
        if (!node) return NULL;
        node->mat = mat;
        node->left_child = NULL;
        node->right_child = NULL;
        return node;
    }
    if (mat->name < root->mat->name) {
        root->left_child = insert_bst_sf(mat, root->left_child);
    } else {
        root->right_child = insert_bst_sf(mat, root->right_child);
    }
    return root;
}

matrix_sf* find_bst_sf(char name, bst_sf *root) {
    if (!root) return NULL;
    if (name == root->mat->name) return root->mat;
    if (name < root->mat->name) return find_bst_sf(name, root->left_child);
    return find_bst_sf(name, root->right_child);
}

void free_bst_sf(bst_sf *root) {
    if (!root) return;
    free_bst_sf(root->left_child);
    free_bst_sf(root->right_child);
    if (root->mat) free(root->mat);
    free(root);
}

matrix_sf* add_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) {
    assert(mat1 && mat2);
    unsigned int r = mat1->num_rows;
    unsigned int c = mat1->num_cols;
    matrix_sf *res = alloc_matrix('?', r, c);
    if (!res) return NULL;
    for (unsigned int i = 0; i < r * c; i++) res ->values[i] = mat1->values[i] + mat2->values[i];
    return res;
}

matrix_sf* mult_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) {
    assert(mat1 && mat2);
    unsigned int r = mat1->num_rows;
    unsigned int m = mat1->num_cols;
    unsigned int c = mat2->num_cols;
    matrix_sf *res = alloc_matrix('?', r, c);
    if (!res) return NULL;
    for (unsigned int i = 0; i < r; i++) {
        for (unsigned int j = 0; j < c; j++) {
            long sum = 0;
            for (unsigned int k = 0; k < m; k++) {
                sum += (long)mat1->values[i*m + k] * (long)mat2->values[k*c + j];
            }
            res ->values[i*c + j] = (int)sum;
        }
    }
    return res;
}

matrix_sf* transpose_mat_sf(const matrix_sf *mat) {
    assert(mat);
    unsigned int r = mat->num_rows;
    unsigned int c = mat->num_cols;
    matrix_sf *res = alloc_matrix('?', c, r);
    if (!res) return NULL;
    for (unsigned int i = 0; i < r; i++) {
        for (unsigned int j = 0; j < c; j++) {
            res->values[j*r + i] = mat->values[i*c + j];
        }
    }
    return res;
}

matrix_sf* create_matrix_sf(char name, const char *expr) {
    if (!expr) return NULL;
    const char *p = expr;
    while (isspace((unsigned char)*p)) p++;
    char *endptr;
    long nr = strtol(p, &endptr, 10);
    if (endptr == p || nr <= 0) return NULL;
    p = endptr;
    while (isspace((unsigned char)*p)) p++;
    long nc = strtol(p, &endptr, 10);
    if (endptr == p || nc <= 0) return NULL;
    p = endptr;
    while (*p && *p != '[') p++;
    if (!*p) return NULL;
    p++;

    matrix_sf *m = alloc_matrix(name, (unsigned int)nr, (unsigned int)nc);
    if (!m) return NULL;
    unsigned int total = (unsigned int)(nr * nc);
    unsigned int idx = 0;
    while (*p && idx < total) {
        while (isspace((unsigned int)*p)) p++;
        if (*p ==';') { p++; continue; }
        if (*p == ']') break;
        long val = strtol (p, &endptr, 10);
        if (endptr == p) { p++; continue; }
        m->values[idx++] = (int)val;
        p = endptr;
    }
    while (idx < total) m->values[idx++] = 0;
    return m;
}

char* infix2postfix_sf(char *infix) {
    return NULL;
}

matrix_sf* evaluate_expr_sf(char name, char *expr, bst_sf *root) {
    return NULL;
}

matrix_sf *execute_script_sf(char *filename) {
   return NULL;
}

// This is a utility function used during testing. Feel free to adapt the code to implement some of
// the assignment. Feel equally free to ignore it.
matrix_sf *copy_matrix(unsigned int num_rows, unsigned int num_cols, int values[]) {
    matrix_sf *m = malloc(sizeof(matrix_sf)+num_rows*num_cols*sizeof(int));
    m->name = '?';
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    memcpy(m->values, values, num_rows*num_cols*sizeof(int));
    return m;
}

// Don't touch this function. It's used by the testing framework.
// It's been left here in case it helps you debug and test your code.
void print_matrix_sf(matrix_sf *mat) {
    assert(mat != NULL);
    assert(mat->num_rows <= 1000);
    assert(mat->num_cols <= 1000);
    printf("%d %d ", mat->num_rows, mat->num_cols);
    for (unsigned int i = 0; i < mat->num_rows*mat->num_cols; i++) {
        printf("%d", mat->values[i]);
        if (i < mat->num_rows*mat->num_cols-1)
            printf(" ");
    }
    printf("\n");
}
