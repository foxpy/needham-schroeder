#include <qc.h>
#include "server.h"

int main(int argc, char* argv[]) {
    char const* database_path;
    qc_err* err = qc_err_new();
    qc_args* args = qc_args_new();
    qc_args_string(args, "database", &database_path, "Path to server database file");
    if (qc_args_parse(args, argc, argv, err) == QC_FAILURE) {
        qc_err_fatal(err, "Failed to parse CLI arguments");
    } else {
        server_ctx* ctx;
        if ((ctx = server_init(database_path, err)) == NULL) {
            qc_err_fatal(err, "Failed to initialize server");
        }
        server_close(ctx);
    }
    qc_args_free(args);
    qc_err_free(err);
}
