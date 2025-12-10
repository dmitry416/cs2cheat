cmake_minimum_required(VERSION 3.26)
project(cs2cheat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

if(MSVC)
    add_compile_options(
            /GS
            /sdl
            /guard:cf
            /DYNAMICBASE
            /NXCOMPAT
            /Qpar
    )

    add_compile_options(/wd4201 /wd4100 /wd4189)
else()
    add_compile_options(
            -fstack-protector
            -fPIE
            -Wl,-pie
            -D_FORTIFY_SOURCE=2
            -O2
            -fno-plt
    )
endif()

include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY	https://github.com/g-truc/glm.git
        GIT_TAG 	0af55ccecd98d4e5a8d1fad7de25ba429d60e863
)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.5-docking
)
FetchContent_MakeAvailable(imgui)

add_executable(cs2cheat WIN32
        main.cpp
        security/Security.h
        headers/offsets.h
        headers/include.h
        utils/Player.cpp
        utils/Player.h
        utils/Entity.h
        utils/Entity.cpp
        utils/utils.h
        utils/utils.cpp
        ui/Overlay.h
        ui/Overlay.cpp
)

target_sources(cs2cheat PRIVATE
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

target_include_directories(cs2cheat PRIVATE headers security ui ${imgui_SOURCE_DIR})

target_link_libraries(cs2cheat PRIVATE glm::glm)

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
            dbghelp
            psapi
            iphlpapi
            winhttp
            d3d11
            d3dcompiler
            dwmapi
    )
endif()

target_include_directories(cs2cheat PRIVATE
        headers
        security
        ui
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)

    target_compile_definitions(cs2cheat PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            _CRT_SECURE_NO_WARNINGS
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