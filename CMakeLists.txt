cmake_minimum_required(VERSION 3.15)
project(gazou)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

file(GLOB_RECURSE SRCFILES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(gazou ${SRCFILES})

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )

find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
find_package(Crow CONFIG REQUIRED)

include_directories(${ImageMagick_INCLUDE_DIRS})
target_link_libraries(gazou ${ImageMagick_LIBRARIES} ${Crow_LIBRARIES})