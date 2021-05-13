#include <stdio.h>
#include <string.h>
#include <qc.h>
#include "server.h"

static void help(void* help_data) {
    char const* program_name = help_data;
    fprintf(stderr, "Usage: %s --database=DB_PATH --mode=MODE -- <MODE_ARGS>\n", program_name);
    fprintf(stderr, "Where MODE is one of:\n");
    fprintf(stderr, "\tadd-user -- USERNAME: adds user USERNAME to database and outputs his data\n");
    fprintf(stderr, "\tlisten -- IP PORT: starts listen server on IP:PORT and serves requests\n");
}

static qc_result add_user_cmd(server_ctx* ctx, int extra_args_idx, int extra_args_cnt, char* argv[], qc_err* err) {
    if (extra_args_cnt != 1) {
        qc_err_set(err, "Add user command: expected exactly one argument, got %d", extra_args_cnt);
        return QC_FAILURE;
    } else {
        char const* name = argv[extra_args_idx+0];
        if (server_register(ctx, name, err) == QC_SUCCESS) {
            // TODO: print id and key
            return QC_SUCCESS;
        } else {
            return QC_FAILURE;
        }
    }
}

static qc_result listen_cmd(server_ctx* ctx, int extra_args_idx, int extra_args_cnt, char* argv[], qc_err* err) {
    QC_UNUSED(ctx);
    QC_UNUSED(extra_args_idx);
    QC_UNUSED(extra_args_cnt);
    QC_UNUSED(argv);
    QC_UNUSED(err);
    QC_UNIMPLEMENTED();
}

int main(int argc, char* argv[]) {
    char const* database_path;
    char const* mode;
    qc_err* err = qc_err_new();
    qc_args* args = qc_args_new();
    qc_args_set_help(args, help, argv[0]);
    qc_args_string(args, "database", &database_path, "Path to server database file");
    qc_args_string(args, "mode", &mode, "Server operation mode");
    if (qc_args_parse(args, argc, argv, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to parse CLI arguments");
    } else {
        server_ctx* ctx;
        qc_result result;
        if ((ctx = server_init(database_path, err)) == NULL) {
            qc_err_fatal(err, "Failed to initialize server");
        } else if (strcmp(mode, "add-user") == 0) {
            result = add_user_cmd(ctx, qc_args_extras_index(args), qc_args_extras_count(args), argv, err);
        } else if (strcmp(mode, "listen") == 0) {
            result = listen_cmd(ctx, qc_args_extras_index(args), qc_args_extras_count(args), argv, err);
        } else {
            qc_err_set(err, "Unknown operating mode: %s", mode);
            qc_err_fatal(err, "Failed to start server");
        }
        server_close(ctx);
        if (result == QC_FAILURE) {
            qc_err_fatal(err, "Failed to execute command %s", mode);
        }
    }
    qc_args_free(args);
    qc_err_free(err);
}
