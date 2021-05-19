#include <mqueue.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <qc.h>
#include <mq_setup.h>
#include "server.h"

void parse_message(const uint8_t msg[static BUFSIZ]) {
    char* a_id;
    char* b_id;
    qc_bytes_to_hexstr(false, 16, &msg[0], &a_id);
    qc_bytes_to_hexstr(false, 16, &msg[16], &b_id);
    printf("User %s is willing to talk to user %s\n", &a_id[2], &b_id[2]);
    free(a_id);
    free(b_id);
}

static void listen_loop(mqd_t in, mqd_t out, qc_err* err) {
    ssize_t rc;
    char msg[BUFSIZ];
    for (;;) {
        errno = 0;
        rc = mq_receive(in, msg, BUFSIZ, NULL);
        if (rc == -1) {
            qc_err_set(err, "Failed to obtain message from queue: %s", strerror(errno));
            return;
        } else {
            assert(rc == BUFSIZ);
            parse_message((uint8_t*) msg);
            mq_send(out, msg, rc, 0);
        }
    }
}

void server_listen(server_ctx* ctx, char const* mqueue_in_name, char const* mqueue_out_name, qc_err* err) {
    QC_UNUSED(ctx);
    mqd_t in, out;
    if (setup_mq(mqueue_in_name, mqueue_out_name, &in, &out, err) == QC_FAILURE) {
        qc_err_append_front(err, "Failed to setup message queues for IPC");
    } else {
        listen_loop(in, out, err);
    }
}
