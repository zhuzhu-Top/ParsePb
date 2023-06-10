cmake_minimum_required(VERSION 3.24)
project(ParsePb)

if (MSVC)
    add_compile_options(
            $<$<CONFIG:>:/MT> #---------|
            $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
            $<$<CONFIG:Release>:/MT> #--|
    )
    add_compile_options(/EHsc)
endif ()
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include(FetchContent)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.12.0
        )
FETCHCONTENT_GETPROPERTIES(googletest)
if (NOT googletest_POPULATED)

    FETCHCONTENT_POPULATE(googletest)
    MESSAGE("-> ${googletest_SOURCE_DIR}")
    MESSAGE("-> ${googletest_BUILD_DIR}")
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BUILD_DIR})
endif ()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
        )
set(protobuf_BUILD_TESTS OFF)
set(protobuf_WITH_ZLIB OFF)
set(protobuf_USE_EXTERNAL_GTEST ON)
FetchContent_MakeAvailable(nlohmann_json)


FetchContent_Declare(protobuf
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
        GIT_TAG v23.2)
FetchContent_MakeAvailable(protobuf)


add_executable(${PROJECT_NAME}
        ProtobufHelper.cpp
        ProtobufHelper.h
        test.cpp
        )

MESSAGE("11-> ${googlemock_source_dir}")
target_include_directories(${PROJECT_NAME} PRIVATE
        ${GOOGLETEST_INCLUDE_DIR}
        )


get_cmake_property(_variableNames VARIABLES)
foreach(_variableName ${_variableNames})
    if(_variableName MATCHES "^protobuf_.*")
        message("${_variableName} = ${${_variableName}}")
    endif()
endforeach()

target_link_libraries(${PROJECT_NAME} nlohmann_json protobuf::libprotobuf gmock_main gtest )



