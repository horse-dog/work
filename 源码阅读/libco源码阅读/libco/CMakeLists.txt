cmake_minimum_required(VERSION 3.12)
project(libco)

# This for mac osx only
set(CMAKE_MACOSX_RPATH 0)

set(LIBCO_VERSION 0.5)

# -g: 开启调试信息。
# -fno-strict-aliasing: 禁用strict aliasing优化。strict aliasing是一种编译器优化手段，但在某些情况下会影响程序正确性。
# -O2: 开启优化级别2，优化代码执行效率。
# -Wall: 开启所有警告。
# -export-dynamic: 在动态链接时导出符号表。
# -pipe: 将编译过程中的管道用内存代替。
# -D_GNU_SOURCE: 定义_GNU_SOURCE宏，启用GNU扩展的函数和变量。
# -D_REENTRANT: 定义_REENTRANT宏，启用线程安全的函数和变量。
# -fPIC: 生成与位置无关的代码，以便将编译后的代码在不同的地址空间中重定位。
# -Wno-deprecated: 禁用弃用的警告。
# -m64: 生成64位目标代码。
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -g -fno-strict-aliasing -O2 -Wall -export-dynamic -Wall -pipe  -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64)

# Use c and asm
enable_language(C ASM)

# Add source files
set(SOURCE_FILES
        co_comm.cpp
        co_epoll.cpp
        co_hook_sys_call.cpp
        co_routine.cpp
        coctx.cpp
        coctx_swap.S)

# Add static and shared library target
add_library(colib_static STATIC ${SOURCE_FILES})
add_library(colib_shared SHARED ${SOURCE_FILES})

# 将colib_static的输出文件名设置为colib
set_target_properties(colib_static PROPERTIES OUTPUT_NAME colib)
# 将colib_shared的输出文件名设置为colib
set_target_properties(colib_shared PROPERTIES OUTPUT_NAME colib)

# 设置colib_static的清理策略为直接输出清理，即make clean命令可以直接删除静态库文件
set_target_properties(colib_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
# 设置colib_shared的清理策略为直接输出清理，即make clean命令可以直接删除共享库文件
set_target_properties(colib_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# Set shared library version, will generate libcolib.${LIBCO_VERSION}.so and a symbol link named libcolib.so
# For mac osx, the extension name will be .dylib

# 设置colib_shared的版本号为${LIBCO_VERSION}，同时也将库的SO版本设置为${LIBCO_VERSION}
set_target_properties(colib_shared PROPERTIES VERSION ${LIBCO_VERSION} SOVERSION ${LIBCO_VERSION})

# Macro for add example target
macro(add_example_target EXAMPLE_TARGET)
    add_executable("example_${EXAMPLE_TARGET}" "example_${EXAMPLE_TARGET}.cpp")
    target_link_libraries("example_${EXAMPLE_TARGET}" colib_static pthread dl)
endmacro(add_example_target)

add_example_target(closure)
add_example_target(cond)
add_example_target(copystack)
add_example_target(echocli)
add_example_target(echosvr)
add_example_target(poll)
add_example_target(setenv)
add_example_target(specific)
add_example_target(thread)
