#include <stdio.h>
#include <qc.h>
#include <sqlite3.h>

int main(void) {
    puts("Hello world");
    printf("%d\n", (int) qc_min(3, 6));
    printf("SQLite %s\n", sqlite3_version);
}
