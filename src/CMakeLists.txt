project(server LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB INCLUDE_FILES
    *.h
    **/*.h
    **/**/*.h
    **/**/**/*.h
    **/**/**/**/*.h
)
file(GLOB SOURCE_FILES
    *.cpp
    **/*.cpp
    **/**/*.cpp
    **/**/**/*.cpp
    **/**/**/**/*.cpp
)
add_executable(${PROJECT_NAME}
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
)

if (MSVC)
    add_definitions(/EHsc)
else ()
    add_definitions(-fexceptions)
endif ()

if (CONAN_EXPORTED)
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else ()
    include(${CMAKE_CURRENT_SOURCE_DIR}/conan.cmake)
    conan_cmake_configure(REQUIRES
            fmt/10.2.1
            spdlog/1.14.1
            openssl/3.2.2
            sqlite3/3.46.0
            magic_enum/0.9.6
            cpp-httplib/0.18.1
            mariadb-connector-c/3.3.8
        GENERATORS CMakeDeps CMakeToolchain
        IMPORTS "bin, *.dll -> ./bin"
        OPTIONS
            fmt/*:shared=True
            spdlog/*:shared=True
            openssl/*:shared=True
            sqlite3/*:shared=True
            magic_enum/*:shared=True
            cpp-httplib/*:shared=True
            mariadb-connector-c/*:shared=True)

    conan_cmake_autodetect(settings)
    conan_cmake_install(PATH_OR_REFERENCE .
        BUILD missing
        REMOTE conancenter)
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}" ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}" ${CMAKE_PREFIX_PATH})
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

find_package(fmt REQUIRED)
if (fmt_FOUND)
    message(STATUS "Found fmt: ${fmt_VERSION}, at ${fmt_INCLUDE_DIRS}, ${fmt_LIBRARIES}")
    target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
    target_include_directories(${PROJECT_NAME} PUBLIC ${fmt_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find fmt")
endif ()

find_package(spdlog REQUIRED)
if (spdlog_FOUND)
    message(STATUS "Found spdlog: ${spdlog_VERSION}, at ${spdlog_INCLUDE_DIRS}, ${spdlog_LIBRARIES}")
    target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
    target_include_directories(${PROJECT_NAME} PUBLIC ${fmt_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find spdlog")
endif ()

find_package(OpenSSL REQUIRED)
if (OpenSSL_FOUND)
    message(STATUS "Found OpenSSL: ${OpenSSL_VERSION}, at ${OpenSSL_INCLUDE_DIRS}, ${OpenSSL_LIBRARIES}")
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenSSL::SSL OpenSSL::Crypto)
    target_include_directories(${PROJECT_NAME} PUBLIC ${OpenSSL_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find OpenSSL")
endif ()

find_package(SQLite3 REQUIRED)
if (SQLite3_FOUND)
    message(STATUS "Found SQLite3: ${SQLite3_VERSION}, at ${SQLite3_INCLUDE_DIRS}, ${SQLite3_LIBRARIES}")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SQLite3_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SQLite3_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find SQLite3")
endif ()

find_package(magic_enum REQUIRED)
if (magic_enum_FOUND)
    message(STATUS "Found magic_enum: ${magic_enum_VERSION}, at ${magic_enum_INCLUDE_DIRS}, ${magic_enum_LIBRARIES}")
    target_link_libraries(${PROJECT_NAME} PUBLIC magic_enum::magic_enum)
    target_include_directories(${PROJECT_NAME} PUBLIC ${magic_enum_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find magic_enum")
endif ()

find_package(httplib REQUIRED)
if (httplib_FOUND)
    message(STATUS "Found httplib: ${httplib_VERSION}, at ${httplib_INCLUDE_DIRS}, ${httplib_LIBRARIES}")
    target_link_libraries(${PROJECT_NAME} PUBLIC httplib::httplib)
    target_include_directories(${PROJECT_NAME} PUBLIC ${httplib_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find httplib")
endif ()

find_package(mariadb-connector-c REQUIRED)
if (mariadb-connector-c_FOUND)
    message(STATUS "Found mariadb-connector-c: ${mariadb-connector-c_VERSION}, at ${mariadb-connector-c_INCLUDE_DIRS}, ${mariadb-connector-c_LIBRARIES}")
    target_link_libraries(${PROJECT_NAME} PUBLIC mariadb-connector-c::mariadb-connector-c)
    target_include_directories(${PROJECT_NAME} PUBLIC ${mariadb-connector-c_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find mariadb-connector-c")
endif ()

# find_package(Sqlpp11 REQUIRED)
# if (Sqlpp11_FOUND)
#     message(STATUS "Found Sqlpp11: ${Sqlpp11_VERSION}, at ${Sqlpp11_INCLUDE_DIRS}, ${Sqlpp11_LIBRARIES}")
#     target_link_libraries(${PROJECT_NAME} PUBLIC sqlpp11::sqlpp11)
#     target_include_directories(${PROJECT_NAME} PUBLIC ${Sqlpp11_INCLUDE_DIRS})
# else ()
#     message(FATAL_ERROR "Could not find Sqlpp11")
# endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 11
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN 
    SPDLOG_FMT_EXTERNAL
    CPPHTTPLIB_OPENSSL_SUPPORT
)

# set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# find_package(unofficial-sqlite3 CONFIG REQUIRED)
# find_package(fmt REQUIRED)
find_package(magic_enum REQUIRED)
# find_package(spdlog REQUIRED)
# find_package(OpenSSL REQUIRED)
# find_package(unofficial-libmariadb REQUIRED)
# find_path(HTTPLIB_INCLUDE_DIR "httplib.h")

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    # ${HTTPLIB_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    # unofficial::sqlite3::sqlite3
    # fmt::fmt
    # magic_enum::magic_enum
    # spdlog::spdlog
    OpenSSL::SSL
    # unofficial::libmariadb
    # sqlpp11::sqlpp11
    Pal::Sigslot
    enet
)
