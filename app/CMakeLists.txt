file(GLOB_RECURSE RBS_SRCS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE RBS_HDRS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(core STATIC ${RBS_SRCS} ${RBS_HDRS})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(core PUBLIC cxx_std_23)

if (NOT MSVC)
  target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(boomerang ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(boomerang PRIVATE core)

set_target_properties(boomerang PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)