cmake_minimum_required(VERSION 3.26)
project(cs2cheat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

add_executable(cs2cheat
        main.cpp
        headers/offsets.h
        headers/include.h
        utils/Player.cpp
        utils/Player.h
        utils/Entity.h
        utils/Entity.cpp
        utils/utils.h
        utils/utils.cpp
)

if(WIN32)
    target_link_libraries(cs2cheat PRIVATE
            kernel32
            user32
            gdi32
            winspool
            shell32
            ole32
            oleaut32
            uuid
            comdlg32
            advapi32
    )
endif()

target_include_directories(cs2cheat
        PRIVATE
        headers
)

if(WIN32)
    target_compile_definitions(cs2cheat PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )
endif()

if(MSVC)
    target_compile_options(cs2cheat PRIVATE /W4)
else()
    target_compile_options(cs2cheat PRIVATE -Wall -Wextra)
endif()

set_target_properties(cs2cheat PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)