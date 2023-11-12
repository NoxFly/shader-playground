# Version of CMake
cmake_minimum_required (VERSION 3.12)

# Name of the project (can be changed)
set(PROJECT_NAME "ShaderPlayground")

project ("${PROJECT_NAME}")

# Load configuration file
include(config.cmake OPTIONAL)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")

# Add the include directory of the project
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)


## --- LIBRARIES --- ##
# Set default paths if not defined by the config file
set(GLEW_INCLUDE_DIR $ENV{GLEW_INCLUDE_DIR} CACHE PATH "Path to GLEW include directory" FORCE)
set(GLFW_INCLUDE_DIR $ENV{GLFW_INCLUDE_DIR} CACHE PATH "Path to GLFW include directory" FORCE)
set(GLM_INCLUDE_DIR $ENV{GLM_INCLUDE_DIR} CACHE PATH "Path to GLM include directory" FORCE)

# Set default paths if not defined by the config file
set(GLEW_LIBRARY_DEBUG $ENV{GLEW_LIBRARY_DEBUG} CACHE FILEPATH "Path to GLEW library for Debug" FORCE)
set(GLEW_LIBRARY_RELEASE $ENV{GLEW_LIBRARY_RELEASE} CACHE FILEPATH "Path to GLEW library for Release" FORCE)
set(GLFW_LIBRARY $ENV{GLFW_LIBRARY} CACHE FILEPATH "Path to GLFW library for Debug" FORCE)
## --- --- --- --- ##


# Enable Hot-reload the the MSVC compilers if this is supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# Add the executable
add_executable("${PROJECT_NAME}" ${SOURCES})

# Set the current working directory for binary files (e.g., executables)
set(CMAKE_CURRENT_BINARY_DIR "${CMAKE_SOURCE_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")



# Visual Studio specific settings
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")

# Create the "bin/" directory if it doesn't exist
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Visual Studio specific: Determine the configuration-specific output directory
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CONFIGURATION_DIR "x64-debug")
else()
    set(CONFIGURATION_DIR "x64-release")
endif()

# Add a custom command to copy the executable after build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${PROJECT_NAME}>
            ${EXECUTABLE_OUTPUT_PATH}/${CONFIGURATION_DIR}
    COMMENT "Copying executable to bin folder"
)

# Visual Studio specific: Set the property to use with MSBuild
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")

# Include directories for external libraries
target_include_directories("${PROJECT_NAME}" PRIVATE ${PROJECT_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${GLM_INCLUDE_DIR})

# Link against the external libraries
# Use generator expressions to set the library directories based on configuration
target_link_libraries("${PROJECT_NAME}" PRIVATE
    $<$<CONFIG:Debug>:${GLEW_LIBRARY_DEBUG};${GLFW_LIBRARY};OpenGL32.lib>
    $<$<CONFIG:Release>:${GLEW_LIBRARY_RELEASE};${GLFW_LIBRARY};OpenGL32.lib>
)
