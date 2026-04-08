cmake_minimum_required(VERSION 3.10)
project(AccountingServer LANGUAGES C CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 输出路径
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)

# MySQL 路径（和你之前一样）
set(MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/MySQL Server 8.0/include")
set(MYSQL_LIB_DIR     "C:/Program Files/MySQL/MySQL Server 8.0/lib")

# 可执行文件
add_executable(AccountingServer
    main.cpp
    src/RecordDao.cpp
    src/DBconnect.cpp
    src/handler.cpp
    src/Config.cpp
    third_party/inih/INIReader.cpp
    third_party/inih/ini.c
)

# 头文件路径
target_include_directories(AccountingServer PRIVATE
    ${PROJECT_SOURCE_DIR}/include  
    ${PROJECT_SOURCE_DIR}/third_party   # httplib.h 在这里  //设置头文件搜索的起点
    ${MYSQL_INCLUDE_DIR}
)

#添加编译选项
target_compile_options(AccountingServer PRIVATE /utf-8)


# 链接 MySQL
target_link_directories(AccountingServer PRIVATE ${MYSQL_LIB_DIR})
target_link_libraries(AccountingServer PRIVATE libmysql)

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
