#include <check.h>
#include <stdint.h>
#include "queue.h"

START_TEST (test_push)
{
    ByteQueue queue;
    queue_init(&queue);
    int length = queue_length(&queue);
    fail_unless(length == 0, "expected queue length of 0 but got %d", length);
    bool success = queue_push(&queue, 0xEF);
    fail_unless(success);
    fail_unless(queue_length(&queue) == 1,
            "expected queue length of 1 but got %d", length);
}
END_TEST

START_TEST (test_pop)
{
    ByteQueue queue;
    queue_init(&queue);
    uint8_t original_value = 0xEF;
    queue_push(&queue, original_value);
    uint8_t value = queue_pop(&queue);
    fail_unless(value == original_value);
}
END_TEST

START_TEST (test_fill_er_up)
{
    ByteQueue queue;
    queue_init(&queue);
    for(int i = 0; i < MAX_QUEUE_LENGTH; i++) {
        bool success = queue_push(&queue, (uint8_t) (i % 255));
        fail_unless(success, "wasn't able to add the %dth element", i + 1);
    }

    for(int i = 0; i < MAX_QUEUE_LENGTH; i++) {
        uint8_t value = queue_pop(&queue);
        if(i < MAX_QUEUE_LENGTH - 1) {
            fail_unless(!queue_empty(&queue),
                    "didn't expect queue to be empty on %dth iteration", i + 1);
        }
        uint8_t expected = i % 255;
        fail_unless(value == expected,
                "expected %d but got %d out of the queue", expected, value);
    }
    fail_unless(queue_empty(&queue));
}
END_TEST

START_TEST (test_length)
{
    ByteQueue queue;
    queue_init(&queue);
    fail_unless(queue_length(&queue) == 0);
    for(int i = 0; i < MAX_QUEUE_LENGTH; i++) {
        queue_push(&queue,  (uint8_t) (i % 255));
        if(i == MAX_QUEUE_LENGTH - 1) {
            break;
        }
        fail_unless(queue_length(&queue) == i + 1,
                "expected length of %d but found %d", i + 1,
                queue_length(&queue));
    }

    for(int i = 0; i < MAX_QUEUE_LENGTH; i++) {
        queue_pop(&queue);
        fail_unless(queue_length(&queue) == MAX_QUEUE_LENGTH - i - 1);
    }
    fail_unless(queue_length(&queue) == 0);

    for(int i = 0; i < MAX_QUEUE_LENGTH; i++) {
        queue_push(&queue, (uint8_t) (i % 255));
        fail_unless(queue_length(&queue) == i + 1,
                "expected length of %d but found %d", i + 1,
                queue_length(&queue));
    }

    for(int i = 0; i < MAX_QUEUE_LENGTH / 2; i++) {
        queue_pop(&queue);
        fail_unless(queue_length(&queue) == MAX_QUEUE_LENGTH - i - 1);
    }

    for(int i = 0; i < MAX_QUEUE_LENGTH / 2; i++) {
        queue_push(&queue, (uint8_t) (i % 255));
        int expectedLength =  i + (MAX_QUEUE_LENGTH / 2) + 1;
        fail_unless(queue_length(&queue) == expectedLength,
                "expected length of %d but found %d", expectedLength,
                queue_length(&queue));
    }
}
END_TEST

Suite* suite(void) {
    Suite* s = suite_create("queue");
    TCase *tc_core = tcase_create("core");
    tcase_add_test(tc_core, test_push);
    tcase_add_test(tc_core, test_pop);
    tcase_add_test(tc_core, test_fill_er_up);
    tcase_add_test(tc_core, test_length);
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
