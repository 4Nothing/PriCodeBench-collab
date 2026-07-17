if((${CONFIG_OS_ARCH_ARMV8}) OR (${CONFIG_OS_ARCH_ARMV7_R}))
    add_subdirectory(gic)
endif()
