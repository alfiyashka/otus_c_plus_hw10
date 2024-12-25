cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "PATCH_VERSION" )
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(async VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

configure_file(version.h.in version.h)

set ( HW10_INCLUDE
   include/cmd.parse/CmdFileWriter.hpp
   include/cmd.parse/CommandParser.hpp
   include/cmd.parse/CmdParserState.hpp
   include/cmd.parse/CmdRange.hpp
)

include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inlude)

set ( HW10_SOURCES
   cmd.parse/CmdFileWriter.cpp
   cmd.parse/CommandParser.cpp
   cmd.parse/CmdParserState.cpp
)

find_package (Threads)

add_library(async SHARED ${HW10_INCLUDE} ${HW10_SOURCES} )


target_link_libraries (async Threads::Threads)
set_property(TARGET async PROPERTY CXX_STANDARD 20)

install(TARGETS async DESTINATION . )

install(DIRECTORY include DESTINATION . ) 


include("functions.cmake")
generate_packages(${PROJECT_NAME})


#async_user
project(bulk_server LANGUAGES C CXX)

set ( BULK_SERVER_INCLUDE
   bulk_server/TcpConnection.hpp
   bulk_server/ConnectionManager.hpp
   bulk_server/AsyncTcpServer.hpp
)
set ( BULK_SERVER_SRC
   bulk_server/TcpConnection.cpp
   bulk_server/ConnectionManager.cpp
   bulk_server/AsyncTcpServer.cpp
)

find_package(Boost COMPONENTS asio)

include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR} ${Boost_INCLUDE_DIRS})
include_directories(hm9/asynclib/include/)

add_library(asynclib SHARED IMPORTED)
set_property(TARGET asynclib PROPERTY IMPORTED_LOCATION ${PROJECT_BINARY_DIR}/libasync.so)

add_executable(bulk_server ${BULK_SERVER_INCLUDE} ${BULK_SERVER_SRC} bulk_server/main.cpp)

target_link_libraries (bulk_server asynclib)

set_property(TARGET bulk_server PROPERTY CXX_STANDARD 20)


install(TARGETS bulk_server DESTINATION .)