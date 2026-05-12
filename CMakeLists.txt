if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    message(STATUS "使用 vcpkg toolchain: $ENV{VCPKG_ROOT}")
else()
    message(WARNING "VCPKG_ROOT 环境变量未设置！")
endif()

cmake_minimum_required(VERSION 3.10)
project(AccountingServer LANGUAGES C CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 输出路径
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)

# MySQL 路径（和你之前一样）
set(MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/MySQL Server 8.0/include")
set(MYSQL_LIB_DIR "C:/Program Files/MySQL/MySQL Server 8.0/lib")

find_package(OpenSSL REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)


# 可执行文件
add_executable(AccountingServer
    AccountingServer.cpp
    src/record_dao.cpp
    src/db_connect.cpp
    src/handler.cpp
    src/config.cpp
    src/user_dao.cpp
    src/token_manager.cpp
    src/validator.cpp
    third_party/inih/INIReader.cpp
    third_party/inih/ini.c
)

# add_executable(test test.cpp)

# 添加编译选项
# if (MSVC)
# target_compile_options(AccountingServer PRIVATE /utf-8)
# else()
# target_compile_options(AccountingServer PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
# endif()

# 头文件路径
target_include_directories(AccountingServer PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party # httplib.h 在这里  //设置头文件搜索的起点
    ${MYSQL_INCLUDE_DIR}
)

# 链接 MySQL
target_link_directories(AccountingServer PRIVATE ${MYSQL_LIB_DIR})
target_link_libraries(AccountingServer PRIVATE libmysql)
target_link_libraries(AccountingServer PRIVATE crypt32)
target_link_libraries(AccountingServer PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    jwt-cpp::jwt-cpp
)

target_compile_options(AccountingServer PRIVATE /utf-8) # 必须是 MSVC，GCC会报错

# 拷贝 libmysql.dll 到输出目录
add_custom_command(TARGET AccountingServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${MYSQL_LIB_DIR}/libmysql.dll"
    $<TARGET_FILE_DIR:AccountingServer>
)
add_custom_command(TARGET AccountingServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/config.ini
    $<TARGET_FILE_DIR:AccountingServer>
)
