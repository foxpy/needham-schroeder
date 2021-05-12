#pragma once
#include <qc.h>
#include <sqlite3.h>

typedef struct {
    sqlite3* conn;
} server_ctx;

server_ctx* server_init(char const* database_path, qc_err* err);
void server_close(server_ctx* server);
