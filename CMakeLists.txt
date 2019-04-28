cmake_minimum_required(VERSION 2.8.12)
project(uv_secnet)

option(USE_CONAN_DEPS "Link against conan dependencies, system libs used otherwise")

if(CMAKE_VERSION VERSION_LESS 3.0.0)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-std=c++11 COMPILER_SUPPORTS_CXX11)
    check_cxx_compiler_flag(-std=c++0x COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX11)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif(COMPILER_SUPPORTS_CXX0X)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif()
else()
    SET(CMAKE_CXX_STANDARD 11)
    SET(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# IF (NOT WIN32)
#   remove_definitions(-D_WIN32)
# ENDIF()

file(GLOB SRC_FILES "src/*.cc")
file(GLOB HEADER_FILES "src/include/uv_secnet/*.hh")
file(GLOB MAIN_HEADERS "src/include/*.hh")

if(USE_CONAN_DEPS)
  message("using conan shared libs")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()

  set(LIB_DEPENDENCIES ${CONAN_LIBS})
else()
  message("using system shared libs")
  set(LIB_DEPENDENCIES http_parser uv ssl crypto})
endif()

add_library(uv_secnet SHARED ${SRC_FILES})
set_property(TARGET uv_secnet PROPERTY POSITION_INDEPENDENT_CODE ON)
set_target_properties(uv_secnet PROPERTIES SOVERSION 1)
target_include_directories(uv_secnet PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
  $<INSTALL_INTERFACE:include>
)

add_library(uv_secnet_static STATIC ${SRC_FILES})
set_property(TARGET uv_secnet_static PROPERTY POSITION_INDEPENDENT_CODE ON)
set_target_properties(uv_secnet_static PROPERTIES OUTPUT_NAME uv_secnet)

target_include_directories(uv_secnet_static PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
  $<INSTALL_INTERFACE:include>
)

# install headers
install(FILES ${HEADER_FILES} DESTINATION include/uv_secnet)
install(FILES ${MAIN_HEADERS} DESTINATION include)

# install library
install(TARGETS uv_secnet
  LIBRARY DESTINATION lib)

add_executable(tr_bin test/main.cc)
target_include_directories(tr_bin PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(tr_bin uv_secnet ${LIB_DEPENDENCIES})

add_executable(tr_test test/test.cc)
