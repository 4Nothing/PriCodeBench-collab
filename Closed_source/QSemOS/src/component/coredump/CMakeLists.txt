include_directories(.)
add_library_ex(coredump_core.c)
add_library_ex(coredump_memory_regions.c)
add_library_ex(backend/coredump_backend_print.c)
