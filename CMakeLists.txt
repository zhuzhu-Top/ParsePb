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
FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.12.0)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(protobuf
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
        GIT_TAG v23.2)
FetchContent_MakeAvailable(protobuf)

include(GoogleTest)

add_executable(${PROJECT_NAME}
        ProtobufHelper.cpp
        ProtobufHelper.h
        test.cpp
        )

target_link_libraries(${PROJECT_NAME} nlohmann_json protobuf::libprotobuf)

