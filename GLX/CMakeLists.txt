file(GLOB_RECURSE CXX_SRC src/*.cpp src/*.h)

add_executable(${PROJECT_NAME} ${CXX_SRC})

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/GLFW/include ${CMAKE_SOURCE_DIR}/vendor/Glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/spdlog/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/stb)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glm)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/ImGui)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/Assimp/include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} glfw glad spdlog stb_image ImGui assimp)

# Create the debug build mode
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:GL_DEBUG>" "GLFW_INCLUDE_NONE" "_CRT_SECURE_NO_WARNINGS")

# Create the release build mode
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:RELEASE>:GL_RELEASE>" "GLFW_INCLUDE_NONE" "_CRT_SECURE_NO_WARNINGS")

# Create the dist build mode
set(CMAKE_CXX_FLAGS_DIST "${CMAKE_CXX_FLAGS_DIST} -O3 -g0")
target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DIST>:GL_DIST>" "GLFW_INCLUDE_NONE" "_CRT_SECURE_NO_WARNINGS")
