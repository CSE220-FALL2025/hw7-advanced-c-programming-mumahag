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

static int prec(char op) {
    if (op == '+') return 1;
    if (op == '*') return 2;
    return 0;
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
    for (unsigned int i = 0; i < r * c; i++) {
        res ->values[i] = mat1->values[i] + mat2->values[i];
    }
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
        if (*p ==';') { 
            p++; 
            continue; 
        }
        if (*p == ']') break;
        long val = strtol (p, &endptr, 10);
        if (endptr == p) { 
            p++; 
            continue; 
        }
        m->values[idx++] = (int)val;
        p = endptr;
    }
    while (idx < total) m->values[idx++] = 0;
    return m;
}

char* infix2postfix_sf(char *infix) {
    if (!infix) return NULL;
    size_t len = strlen(infix);
    char *out = malloc(len + 2);
    if (!out) return NULL;
    int out_i = 0;
    char opstack[256];
    int top = -1;

    for (size_t i = 0; i < len; i++) {
        char ch = infix[i];
        if(isspace((unsigned char)ch)) continue;
        if (ch >= 'A' && ch <= 'Z') {
            out[out_i++] = ch;
            continue;
        }
        if (ch == '\'') {
            out[out_i++] = '\'';
            continue;
        }
        if (ch == '(') {
            opstack[++top] = ch;
            continue;
        }
        if (ch == ')') {
            while (top >= 0 && opstack[top] != '(') {
                out[out_i++] = opstack[top--];
            }
            if (top >= 0 && opstack[top] == '(') top--;
            continue;
        }
        if (ch == '+' || ch == '*') {
            while (top >= 0 && opstack[top] != '(' && prec(opstack[top]) >= prec(ch)) {
                out[out_i++] = opstack[top--];
            }
            opstack[++top] = ch;
            continue;
        }
    }
    while (top >= 0) out[out_i++] = opstack[top--];
    out[out_i] = '\0';
    return out;
}

matrix_sf* evaluate_expr_sf(char name, char *expr, bst_sf *root) {
    if (!expr) return NULL;
    char *post = infix2postfix_sf(expr);
    if (!post) return NULL;

    size_t cap = 16;
    size_t sp = 0;
    matrix_sf **stack = malloc(cap * sizeof(matrix_sf*));
    if (!stack) { 
        free(post); 
        return NULL; 
    }

    for (size_t i = 0; post[i] != '\0'; i++) {
        char ch = post[i];
        if (ch >= 'A' && ch <= 'Z') {
            matrix_sf *m = find_bst_sf(ch, root);
            if (sp >= cap) {
                cap *= 2;
                matrix_sf **tmp = realloc(stack, cap * sizeof(matrix_sf*));
                if (!tmp) {
                    free(post);
                    free(stack);
                    return NULL;
                }
                stack = tmp;
            }

            stack[sp++] = m;
            continue;
        }

        if (ch == '\'') {
            if (sp < 1) continue;
            matrix_sf *a = stack[--sp];
            matrix_sf *res = transpose_mat_sf(a);
            if (!res) {
                free(post);
                free(stack);
                return NULL;
            }

            if (a && (a->name < 'A' || a->name > 'Z')) free(a);
            if (sp >= cap) {
                cap *= 2;
                matrix_sf **tmp = realloc(stack, cap * sizeof(matrix_sf*));
                if (!tmp) {
                    free(post);
                    free(stack);
                    return NULL;
                }
                stack = tmp;
            }

            stack[sp++] = res;
            continue;
        }

        if (ch == '+' || ch == '*') {
            if (sp < 2) continue;

            matrix_sf *right = stack[--sp];
            matrix_sf *left = stack[--sp];

            matrix_sf *res = NULL;
            if (ch == '+') res = add_mats_sf(left,right);
            else res = mult_mats_sf(left, right);

            if (!res) {
                free(post);
                free(stack);
                return NULL;
            }

            if (left && (left->name < 'A' || left->name > 'Z')) free(left);
            if (right && (right->name < 'A' || right->name > 'Z')) free(right);
            if (sp >= cap) {
                cap *= 2;
                matrix_sf **tmp = realloc(stack, cap * sizeof(matrix_sf*));
                if (!tmp) {
                    free(post);
                    free(stack);
                    return NULL;
                }
                stack = tmp;
            }

            stack[sp++] = res;
            continue;
        }
    }

    matrix_sf *final = NULL;
    if (sp == 1) {
        final = stack[0];
        if (final) final->name = name;
    }
    free(stack);
    free(post);

    return final;
}

static void free_bst_except(bst_sf *root, matrix_sf *keep) {
    if (!root) return;

    free_bst_except(root->left_child, keep);
    free_bst_except(root->right_child, keep);

    if (root->mat != keep) {
        free(root->mat);
    }
    free(root);
}

matrix_sf *execute_script_sf(char *filename) {
    if (!filename) return NULL;
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    char *line = NULL;
    size_t n = 0;
    ssize_t read;
    bst_sf *root = NULL;
    matrix_sf *last = NULL;

    while ((read = getline(&line, &n, f)) != -1) {
        if (read == 0) continue;
        if (read > 0 && line[read-1] == '\n') line[read-1] = '\0';
        char *p = line;
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') continue;
        char name = *p;
        char *eq = strchr(p, '=');
        if (!eq) {
            free(line);
            line = NULL;
            continue;
        }
        char *rhs = eq + 1;
        while (isspace((unsigned char)*rhs)) rhs++;
        matrix_sf *m = NULL;

        char *trim = rhs;
        while (isspace(*trim)) trim++;

        if (isdigit(*trim)) {
            m = create_matrix_sf(name, trim);
        } else {
            m = evaluate_expr_sf(name, trim, root);
        }
        if (m) {
            root = insert_bst_sf(m, root);
            last = m;
        }
   }

   if (root) {
    free_bst_except(root,last);
   }
   
   free(line);
   fclose(f);
   return last;
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
