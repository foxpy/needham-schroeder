#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <qc.h>
#include <mq_setup.h>
#include <ciphers.h>

static struct parameters {
    mqd_t in;
    mqd_t out;
    uint8_t my_id[16];
    uint8_t target_id[16];
    uint8_t nonce[32];
    uint8_t cipher[1];
    uint8_t my_key[32];
    uint8_t session_key[32];
    uint8_t payload_for_target[48];
} param;

static qc_result parse_hex(char const* str, size_t len, uint8_t dst[static len], qc_err* err) {
    uint8_t* parsed;
    ptrdiff_t l;
    if ((l = qc_hexstr_to_bytes(str, &parsed)) == -1) {
        qc_err_set(err, "Supplied string is not a hexadecimal number");
        return QC_FAILURE;
    } else if (l != (ptrdiff_t) len) {
        free(parsed);
        qc_err_set(err, "Supplied hexadecimal number should represent %zu-bit number", len*8);
        return QC_FAILURE;
    } else {
        memmove(dst, parsed, len);
        free(parsed);
        return QC_SUCCESS;
    }
}

static void prepare_message(uint8_t buf[static BUFSIZ]) {
    memmove(&buf[0], param.my_id, 16);
    memmove(&buf[16], param.target_id, 16);
    memmove(&buf[32], param.nonce, 32);
    memmove(&buf[64], param.cipher, 1);
}

static void xorshift_decrypt_msg_in_place(uint8_t const key[static 32], uint8_t buf[static 128]) {
    uint8_t* tmp = qc_malloc(128);
    xorshift_decrypt(key, 128, buf, tmp);
    memmove(buf, tmp, 128);
    free(tmp);
}

static void parse_server_reply(uint8_t msg[static 128]) {
    xorshift_decrypt_msg_in_place(param.my_key, msg);
    if (memcmp(&msg[0], param.nonce, 32) == 0 && memcmp(&msg[64], param.target_id, 16) == 0) {
        memmove(param.session_key, &msg[32], 32);
        memmove(param.payload_for_target, &msg[80], 48);
        puts("ok, ready to talk to client B");
    }
}

int main(int argc, char* argv[]) {
    qc_err* err = qc_err_new();
    qc_args* args = qc_args_new();
    char const* mqueue_in_path;
    char const* mqueue_out_path;
    char const* my_id;
    char const* target_id;
    char const* my_key;
    qc_args_string(args, "mqueue-in", &mqueue_in_path, "Name of input message queue for communication with server");
    qc_args_string(args, "mqueue-out", &mqueue_out_path, "Name of output message queue for communication with server");
    qc_args_string(args, "id", &my_id, "My user ID");
    qc_args_string(args, "target-id", &target_id, "Target user ID");
    qc_args_string(args, "my-key", &my_key, "My secret key");
    *(param.cipher) = XORSHIFT_CIPHER;
    if (qc_args_parse(args, argc, argv, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to parse CLI arguments");
    } else if (connect_mq(mqueue_in_path, mqueue_out_path, &param.in, &param.out, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to open message queue");
    } else if (parse_hex(my_id, 16, param.my_id, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to parse My ID");
    } else if (parse_hex(target_id, 16, param.target_id, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to parse Target ID");
    } else if (parse_hex(my_key, 32, param.my_key, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to parse My key");
    } else if (qc_rnd_os_buf(32, param.nonce, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to obtain nonce from OS entropy pool");
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
            parse_server_reply((uint8_t*) msg);
        }
    }
    mq_close(param.in);
    mq_close(param.out);
    qc_args_free(args);
    qc_err_free(err);
}
