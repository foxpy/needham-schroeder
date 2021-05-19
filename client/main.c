#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <qc.h>
#include <mq_setup.h>

int main(int argc, char* argv[]) {
    qc_err* err = qc_err_new();
    qc_args* args = qc_args_new();
    char const* mqueue_in_path;
    char const* mqueue_out_path;
    mqd_t in, out;
    qc_args_string(args, "mqueue-in", &mqueue_in_path, "Name of input message queue for communication with server");
    qc_args_string(args, "mqueue-out", &mqueue_out_path, "Name of output message queue for communication with server");
    if (qc_args_parse(args, argc, argv, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to parse CLI arguments");
    } else if (connect_mq(mqueue_in_path, mqueue_out_path, &in, &out, err) == QC_FAILURE) {
            qc_err_fatal(err, "Failed to open message queue");
    } else {
        char msg[BUFSIZ] = "Hello world";
        errno = 0;
        ssize_t rc;
        if (mq_send(out, msg, BUFSIZ, 0) == -1) {
            qc_err_fatal(err, "Failed to send message: %s", strerror(errno));
        } else if ((rc = mq_receive(in, msg, BUFSIZ, 0)) == -1) {
            qc_err_fatal(err, "Failed to receive answer: %s", strerror(errno));
        } else {
            assert(rc <= INT_MAX);
            printf("Got answer: %.*s\n", (int) rc, msg);
        }
    }
    mq_close(in);
    mq_close(out);
    qc_args_free(args);
    qc_err_free(err);
}
