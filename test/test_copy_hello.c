#include "test.h"

#include <check.h>
#include <string.h>

START_TEST(test_copy_hello)
{
    char buffer[30];
    copy_hello(buffer, sizeof(buffer));

    fail_if(strcmp(buffer, "Hello World") != 0, "Buffer does not equal 'Hello World'");
}
END_TEST

START_TEST(test_fail)
{
    ck_abort_msg("This should most definitely fail");
}
END_TEST

Suite* test_suite(void)
{
    Suite *suite = suite_create("echocounter");
    TCase *tcase = tcase_create("case");
    suite_add_tcase(suite, tcase);

    tcase_add_test(tcase, test_copy_hello);
    tcase_add_test(tcase, test_fail);

    return suite;
}

int main(int arc, char *argv[])
{
    int num_failed = 0;

    Suite *suite = test_suite();
    SRunner *runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);

    num_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return num_failed;
}
