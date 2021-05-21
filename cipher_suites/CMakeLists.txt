add_library(CipherSuites xorshift.c)
target_link_libraries(CipherSuites PUBLIC qc)
target_include_directories(CipherSuites PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
