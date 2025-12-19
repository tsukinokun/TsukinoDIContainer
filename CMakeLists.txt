cmake_minimum_required(VERSION 3.12)
project(TsukinoDIContainer LANGUAGES CXX)

# ヘッダオンリーライブラリとして定義
add_library(TsukinoDIContainer INTERFACE)

# インクルードディレクトリを公開
target_include_directories(TsukinoDIContainer INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# C++17 以上を要求
target_compile_features(TsukinoDIContainer INTERFACE cxx_std_17)

# examples ディレクトリをオプションで追加
option(BUILD_EXAMPLES "Build example programs" ON)

if(BUILD_EXAMPLES)
    add_executable(example1_basic examples/example1_basic.cpp)
    target_link_libraries(example1_basic PRIVATE TsukinoDIContainer)

    add_executable(example2_singleton examples/example2_singleton.cpp)
    target_link_libraries(example2_singleton PRIVATE TsukinoDIContainer)

    add_executable(example3_scoped examples/example3_scoped.cpp)
    target_link_libraries(example3_scoped PRIVATE TsukinoDIContainer)

    add_executable(example4_error examples/example4_error.cpp)
    target_link_libraries(example4_error PRIVATE TsukinoDIContainer)

    add_executable(example5_threadsafe examples/example5_threadsafe.cpp)
    target_link_libraries(example5_threadsafe PRIVATE TsukinoDIContainer)

    add_executable(example6_scoped_multithread examples/example6_scoped_multithread.cpp)
    target_link_libraries(example6_scoped_multithread PRIVATE TsukinoDIContainer)

    add_executable(example7_registerCtor examples/example7_registerCtor.cpp)
    target_link_libraries(example7_registerCtor PRIVATE TsukinoDIContainer)

    add_executable(example8_mock examples/example8_mock.cpp)
    target_link_libraries(example8_mock PRIVATE TsukinoDIContainer)
endif()
