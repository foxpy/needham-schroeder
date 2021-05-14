#pragma once
#include <qc.h>
#include <sqlite3.h>

#define SQLITE3_STMT_NULL_TERMINATED (-1)

typedef struct {
    sqlite3* conn;
} server_ctx;

typedef struct {
    char* name;
    char* id;
    char* key;
} user;

void user_free(user* user);

server_ctx* server_init(char const* database_path, qc_err* err);
void server_close(server_ctx* ctx);

qc_result server_register(server_ctx* ctx, char const* username, qc_err* err);
qc_result server_query(server_ctx* ctx, char const* username, user** dst, qc_err* err);
qc_result server_unregister(server_ctx* ctx, char const* username, qc_err* err);
// TODO: list users command
