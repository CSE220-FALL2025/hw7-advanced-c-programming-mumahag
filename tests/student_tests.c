#include "unit_tests.h"

TestSuite(student_tests, .timeout=TEST_TIMEOUT); 

Test(student_tests, eval_simple_add) {
    bst_sf *root = NULL;

    matrix_sf *A = copy_matrix(2, 2, (int[]){1,2,3,4});
    A->name = 'A';
    root = insert_bst_sf(A, root);

    matrix_sf *B = copy_matrix(2, 2, (int[]){5,6,7,8});
    B->name = 'B';
    root = insert_bst_sf(B, root);

    matrix_sf *R = evaluate_expr_sf('R', "A + B", root);
    cr_assert_not_null(R);

    int expected[4] = {6, 8, 10, 12};
    expect_matrices_equal(R, 2, 2, expected);

    free(R);
    free_bst_sf(root);
}

Test(student_tests, transpose_chain) {
    bst_sf *root = NULL;

    matrix_sf *T = copy_matrix(1, 3, (int[]){10,20,30});
    T->name = 'T';
    root = insert_bst_sf(T, root);

    matrix_sf *R = evaluate_expr_sf('R', "T'''''", root);
    cr_assert_not_null(R);

    expect_matrices_equal(R, 1, 3, (int[]){10,20,30});

    free(R);
    free_bst_sf(root);
}

Test(student_tests, script_simple) {
    FILE *f = fopen("stud1.txt", "w");
    fprintf(f, "A = 1 3 [1 2 3]\n");
    fprintf(f, "B = A'\n");
    fclose(f);

    matrix_sf *res = execute_script_sf("stud1.txt");
    cr_assert_not_null(res);

    int exp[3] = {1, 2, 3};
    expect_matrices_equal(res, 3, 1, exp);

    free(res);
}

