add_executable(ns-server
    main.c
    server.c
)
target_link_libraries(ns-server PUBLIC qc SQLite::SQLite3)
