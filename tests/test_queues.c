#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "bulk_transfer_queue.h"
#include "interrupt_transfer_queue.h"
#include "isochronous_queue.h"
#include "transfer.h"

static void test_bulk_queue_basic(void **state) {
    (void)state;
    transfer_request_t storage[2];
    bulk_transfer_queue_t q;
    bulk_transfer_queue_init(&q, storage, 2);
    transfer_request_t req1 = { .endpoint = 1 };
    transfer_request_t req2 = { .endpoint = 2 };
    assert_true(bulk_transfer_queue_enqueue(&q, &req1));
    assert_true(bulk_transfer_queue_enqueue(&q, &req2));
    transfer_request_t out;
    assert_true(bulk_transfer_queue_dequeue(&q, &out));
    assert_int_equal(out.endpoint, 1);
    assert_true(bulk_transfer_queue_dequeue(&q, &out));
    assert_int_equal(out.endpoint, 2);
}

static void test_interrupt_queue_basic(void **state) {
    (void)state;
    interrupt_transfer_queue_t q;
    interrupt_transfer_queue_init(&q, 2);
    interrupt_transfer_t t1 = { .endpoint = 1 };
    interrupt_transfer_t t2 = { .endpoint = 2 };
    interrupt_transfer_queue_enqueue(&q, t1);
    interrupt_transfer_queue_enqueue(&q, t2);
    interrupt_transfer_t out;
    interrupt_transfer_queue_dequeue(&q, &out);
    assert_int_equal(out.endpoint, 1);
    interrupt_transfer_queue_dequeue(&q, &out);
    assert_int_equal(out.endpoint, 2);
    interrupt_transfer_queue_destroy(&q);
}

static void test_isochronous_queue_basic(void **state) {
    (void)state;
    isochronous_transfer_queue_t q;
    isochronous_transfer_queue_init(&q, 2);
    isochronous_transfer_t t1 = { .data = (void*)1, .length = 1 };
    isochronous_transfer_t t2 = { .data = (void*)2, .length = 2 };
    assert_int_equal(0, isochronous_transfer_queue_enqueue(&q, t1));
    assert_int_equal(0, isochronous_transfer_queue_enqueue(&q, t2));
    isochronous_transfer_t out;
    assert_int_equal(0, isochronous_transfer_queue_dequeue(&q, &out));
    assert_ptr_equal(out.data, (void*)1);
    assert_int_equal(out.length, 1);
    assert_int_equal(0, isochronous_transfer_queue_dequeue(&q, &out));
    assert_ptr_equal(out.data, (void*)2);
    assert_int_equal(out.length, 2);
    isochronous_transfer_queue_destroy(&q);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_bulk_queue_basic),
        cmocka_unit_test(test_interrupt_queue_basic),
        cmocka_unit_test(test_isochronous_queue_basic),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
