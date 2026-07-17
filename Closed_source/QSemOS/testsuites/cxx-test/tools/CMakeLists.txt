include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../util 
)

file(GLOB TEST_UTIL ${CMAKE_CURRENT_SOURCE_DIR}/../util/*.cc)
file(GLOB TEST_BOOST ${CMAKE_CURRENT_SOURCE_DIR}/../boost/system/*.cc)
if (${APP} STREQUAL "UniProton_SMP_test")
    file(GLOB TEST_SMP ${CMAKE_CURRENT_SOURCE_DIR}/../smp/*.cc)
endif()

execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_helper.py
    OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/.compiles_list
    ERROR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/.compiles_list
)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/.compiles_list SRC_LIST)

if (${APP} STREQUAL "UniProton_SMP_test")
    set(ALL_SRC ${TEST_UTIL} ${TEST_BOOST} ${TEST_SMP} ${SRC_LIST} .run_test.cc)
else()
    set(ALL_SRC ${TEST_UTIL} ${TEST_BOOST} ${SRC_LIST} .run_test.cc)
endif()

add_library(cxxtest OBJECT ${ALL_SRC})



