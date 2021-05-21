add_executable(ns-client
    main.c
)
target_link_libraries(ns-client PUBLIC qc rt MqSetup CipherSuites)
