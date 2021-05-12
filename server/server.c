#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <qc.h>
#include "server.h"
#include "schema.h"

server_ctx* server_init(char const* database_path, qc_err* err) {
    server_ctx* server = qc_malloc(sizeof(server_ctx));
    memset(server, 0, sizeof(server_ctx));
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX;
    char* zErr;
    if (sqlite3_open_v2(database_path, &server->conn, flags, NULL) != SQLITE_OK) {
        char const* err_msg = (server->conn == NULL) ? "not enough memory" : sqlite3_errmsg(server->conn);
        qc_err_set(err, "Unable to open \"%s\": %s", database_path, err_msg);
        goto fail;
    } else if (sqlite3_exec(server->conn, schema, NULL, NULL, &zErr) != SQLITE_OK) {
        qc_err_set(err, "Failed to apply database schema: %s", zErr);
        sqlite3_free(zErr);
        goto fail;
    }
    return server;
fail:
    sqlite3_close_v2(server->conn);
    free(server);
    return NULL;
}

void server_close(server_ctx* server) {
    sqlite3_close_v2(server->conn);
    free(server);
}
