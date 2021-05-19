#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <qc.h>
#include <mq_setup.h>

static struct parameters {
    mqd_t in;
    mqd_t out;
    uint8_t my_id[16];
    uint8_t target_id[16];
} param;

static qc_result parse_id(char const* str, uint8_t dst[static 16], qc_err* err) {
    uint8_t* parsed;
    ptrdiff_t len;
    if ((len = qc_hexstr_to_bytes(str, &parsed)) == -1) {
        qc_err_set(err, "Supplied string is not a hexadecimal number");
        return QC_FAILURE;
    } else if (len != 16) {
        free(parsed);
        qc_err_set(err, "Supplied hexadecimal number should represent 128-bit number");
        return QC_FAILURE;
    } else {
        memmove(dst, parsed, 16);
        free(parsed);
        return QC_SUCCESS;
    }
}

static void prepare_message(uint8_t buf[static BUFSIZ]) {
    memmove(&buf[0], param.my_id, 16);
    memmove(&buf[16], param.target_id, 16);
}

int main(int argc, char* argv[]) {
    qc_err* err = qc_err_new();
    qc_args* args = qc_args_new();
    char const* mqueue_in_path;
    char const* mqueue_out_path;
    char const* my_id;
    char const* target_id;
    qc_args_string(args, "mqueue-in", &mqueue_in_path, "Name of input message queue for communication with server");
    qc_args_string(args, "mqueue-out", &mqueue_out_path, "Name of output message queue for communication with server");
    qc_args_string(args, "id", &my_id, "My user ID");
    qc_args_string(args, "target-id", &target_id, "Target user ID");
    if (qc_args_parse(args, argc, argv, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to parse CLI arguments");
    } else if (connect_mq(mqueue_in_path, mqueue_out_path, &param.in, &param.out, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to open message queue");
    } else if (parse_id(my_id, param.my_id, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to parse My ID");
    } else if (parse_id(target_id, param.target_id, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to parse Target ID");
    } else {
        char msg[BUFSIZ] = {0};
        prepare_message((uint8_t*) msg);
        errno = 0;
        ssize_t rc;
        if (mq_send(param.out, msg, BUFSIZ, 0) == -1) {
            qc_err_fatal(err, "Failed to send message: %s", strerror(errno));
        } else if ((rc = mq_receive(param.in, msg, BUFSIZ, 0)) == -1) {
            qc_err_fatal(err, "Failed to receive answer: %s", strerror(errno));
        } else {
            assert(rc == BUFSIZ);
            printf("Negatiated\n");
        }
    }
    mq_close(param.in);
    mq_close(param.out);
    qc_args_free(args);
    qc_err_free(err);
}
