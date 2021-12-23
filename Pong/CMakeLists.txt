cmake_minimum_required (VERSION 3.20)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_C_FLAGS_DEBUG "/MDd /ZI /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /ZI /Ob0 /Od /RTC1")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL /LTCG:OFF")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL /LTCG:OFF")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL /LTCG:OFF")
endif()

find_package(SDL2 CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)

file(COPY "Images" DESTINATION ".")

add_executable (Pong WIN32 "pong.cpp"  "pong_main.cpp" "game.hpp" "game.cpp" "hal.hpp" "hal.cpp")
target_link_libraries(Pong PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(Pong PRIVATE Microsoft.GSL::GSL)