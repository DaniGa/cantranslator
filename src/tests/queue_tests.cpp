#include <check.h>
#include <stdint.h>
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct test_t {
    int i;
    char bytes[8];
};

QUEUE_DECLARE(uint8_t, 512);
QUEUE_DEFINE(uint8_t);
QUEUE_DECLARE(test_t, 10);
QUEUE_DEFINE(test_t);
QUEUE_DECLARE(int, 256);
QUEUE_DEFINE(int);

START_TEST (test_struct_element)
{
    QUEUE_TYPE(test_t) queue;
    QUEUE_INIT(test_t, &queue);
    int length = QUEUE_LENGTH(test_t, &queue);
    fail_unless(length == 0, "expected queue length of 0 but got %d", length);
    test_t original_value = {42, "abcdefg"};
    bool success = QUEUE_PUSH(test_t, &queue, original_value);
    fail_unless(success);
    fail_unless(QUEUE_LENGTH(test_t, &queue) == 1,
            "expected queue length of 1 but got %d", length);

    test_t value = QUEUE_POP(test_t, &queue);
    fail_unless(value.i == original_value.i);
    fail_unless(!strncmp(value.bytes, original_value.bytes, 8));
}
END_TEST

START_TEST (test_push)
{
    QUEUE_TYPE(uint8_t) queue;
    QUEUE_INIT(uint8_t, &queue);
    int length = QUEUE_LENGTH(uint8_t, &queue);
    fail_unless(length == 0, "expected queue length of 0 but got %d", length);
    bool success = QUEUE_PUSH(uint8_t, &queue, 0xEF);
    fail_unless(success);
    fail_unless(QUEUE_LENGTH(uint8_t, &queue) == 1,
            "expected queue length of 1 but got %d", length);
}
END_TEST

START_TEST (test_pop)
{
    QUEUE_TYPE(uint8_t) queue;
    QUEUE_INIT(uint8_t, &queue);
    uint8_t original_value = 0xEF;
    QUEUE_PUSH(uint8_t, &queue, original_value);
    uint8_t value = QUEUE_POP(uint8_t, &queue);
    fail_unless(value == original_value);
}
END_TEST

START_TEST (test_fill_er_up)
{
    QUEUE_TYPE(uint8_t) queue;
    QUEUE_INIT(uint8_t, &queue);
    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t); i++) {
        bool success = QUEUE_PUSH(uint8_t, &queue, (uint8_t) (i % 255));
        fail_unless(success, "wasn't able to add the %dth element", i + 1);
    }

    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t); i++) {
        uint8_t value = QUEUE_POP(uint8_t, &queue);
        if(i < QUEUE_MAX_LENGTH(uint8_t) - 1) {
            fail_unless(!QUEUE_EMPTY(uint8_t, &queue),
                    "didn't expect queue to be empty on %dth iteration", i + 1);
        }
        uint8_t expected = i % 255;
        fail_unless(value == expected,
                "expected %d but got %d out of the queue", expected, value);
    }
    fail_unless(QUEUE_EMPTY(uint8_t, &queue));
}
END_TEST

START_TEST (test_sliding_window)
{
    srand(42);
    QUEUE_TYPE(int) queue;
    QUEUE_INIT(int, &queue);
    for(int i = 0; i < 100; i++) {
        int elementsToAdd = 64 + rand() % 32;
        for(int i = 0; i < elementsToAdd; i++) {
            bool success = QUEUE_PUSH(int, &queue, i);
            fail_unless(success, "wasn't able to add the %dth element", i + 1);
        }

        int expectedValue = 0;
        while(!QUEUE_EMPTY(int, &queue)) {
            int value = QUEUE_POP(int, &queue);
            fail_unless(value == expectedValue);
            ++expectedValue;
        }
    }
    fail_unless(QUEUE_EMPTY(int, &queue));
}
END_TEST

START_TEST (test_length)
{
    QUEUE_TYPE(uint8_t) queue;
    QUEUE_INIT(uint8_t, &queue);
    fail_unless(QUEUE_LENGTH(uint8_t, &queue) == 0);
    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t); i++) {
        QUEUE_PUSH(uint8_t, &queue,  (uint8_t) (i % 255));
        if(i == QUEUE_MAX_LENGTH(uint8_t) - 1) {
            break;
        }
        fail_unless(QUEUE_LENGTH(uint8_t, &queue) == i + 1,
                "expected length of %d but found %d", i + 1,
                QUEUE_LENGTH(uint8_t, &queue));
    }

    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t); i++) {
        QUEUE_POP(uint8_t, &queue);
        fail_unless(QUEUE_LENGTH(uint8_t, &queue) == QUEUE_MAX_LENGTH(uint8_t) - i - 1);
    }
    fail_unless(QUEUE_LENGTH(uint8_t, &queue) == 0);

    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t); i++) {
        QUEUE_PUSH(uint8_t, &queue, (uint8_t) (i % 255));
        fail_unless(QUEUE_LENGTH(uint8_t, &queue) == i + 1,
                "expected length of %d but found %d", i + 1,
                QUEUE_LENGTH(uint8_t, &queue));
    }

    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t) / 2; i++) {
        QUEUE_POP(uint8_t, &queue);
        fail_unless(QUEUE_LENGTH(uint8_t, &queue) == QUEUE_MAX_LENGTH(uint8_t) - i - 1);
    }

    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t) / 2; i++) {
        QUEUE_PUSH(uint8_t, &queue, (uint8_t) (i % 255));
        int expectedLength =  i + (QUEUE_MAX_LENGTH(uint8_t) / 2) + 1;
        fail_unless(QUEUE_LENGTH(uint8_t, &queue) == expectedLength,
                "expected length of %d but found %d", expectedLength,
                QUEUE_LENGTH(uint8_t, &queue));
    }
}
END_TEST

START_TEST (test_available)
{
    QUEUE_TYPE(uint8_t) queue;
    QUEUE_INIT(uint8_t, &queue);
    fail_unless(QUEUE_AVAILABLE(uint8_t, &queue) == QUEUE_MAX_LENGTH(uint8_t));
    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t); i++) {
        QUEUE_PUSH(uint8_t, &queue,  (uint8_t) (i % 255));
        if(i == QUEUE_MAX_LENGTH(uint8_t) - 1) {
            break;
        }
        fail_unless(QUEUE_AVAILABLE(uint8_t, &queue) ==
                    QUEUE_MAX_LENGTH(uint8_t) - i - 1,
                "expected %d available but found %d",
                QUEUE_MAX_LENGTH(uint8_t) - i - 1,
                QUEUE_AVAILABLE(uint8_t, &queue));
    }

    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t); i++) {
        QUEUE_POP(uint8_t, &queue);
        fail_unless(QUEUE_AVAILABLE(uint8_t, &queue) == i + 1);
    }
    fail_unless(QUEUE_AVAILABLE(uint8_t, &queue) == QUEUE_MAX_LENGTH(uint8_t));
}
END_TEST

START_TEST (test_snapshot)
{
    QUEUE_TYPE(uint8_t) queue;
    QUEUE_INIT(uint8_t, &queue);
    uint8_t expected[QUEUE_MAX_LENGTH(uint8_t)];
    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t); i++) {
        uint8_t value = i % 255;
        QUEUE_PUSH(uint8_t, &queue, value);
        expected[i] = value;
    }

    uint8_t snapshot[QUEUE_MAX_LENGTH(uint8_t)];
    QUEUE_SNAPSHOT(uint8_t, &queue, snapshot);
    for(int i = 0; i < QUEUE_MAX_LENGTH(uint8_t); i++) {
        fail_unless(snapshot[i] == expected[i],
                "expected %d but found %d", expected[i], snapshot[i]);
    }
}
END_TEST

Suite* suite(void) {
    Suite* s = suite_create("queue");
    TCase *tc_core = tcase_create("core");
    tcase_add_test(tc_core, test_push);
    tcase_add_test(tc_core, test_pop);
    tcase_add_test(tc_core, test_fill_er_up);
    tcase_add_test(tc_core, test_sliding_window);
    tcase_add_test(tc_core, test_length);
    tcase_add_test(tc_core, test_available);
    tcase_add_test(tc_core, test_snapshot);
    tcase_add_test(tc_core, test_struct_element);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void) {
    int numberFailed;
    Suite* s = suite();
    SRunner *sr = srunner_create(s);
    // Don't fork so we can actually use gdb
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    numberFailed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (numberFailed == 0) ? 0 : 1;
}
