cd ~/Desktop/lr-main
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(CaesarCipher CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(caesar_cipher 
    src/main.cpp 
    src/cipher.cpp 
    src/json_parser.cpp 
    src/logger.cpp
)

target_include_directories(caesar_cipher PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(test_cipher tests/test_cipher.cpp src/cipher.cpp)
target_include_directories(test_cipher PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(test_json tests/test_json.cpp src/json_parser.cpp)
target_include_directories(test_json PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(NOT MSVC)
    target_compile_options(caesar_cipher PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME CipherTests COMMAND test_cipher)
add_test(NAME JsonTests COMMAND test_json)
EOF
