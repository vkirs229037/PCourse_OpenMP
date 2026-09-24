cmake_minimum_required(VERSION 3.10)
project(Course LANGUAGES CXX)

# Установка стандарта C++ (опционально, но рекомендуется)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск библиотеки OpenMP
find_package(OpenMP REQUIRED)

# Создание исполняемого файла из исходников
add_executable(course main.cpp algs.cpp algs.hpp)

# Подключение OpenMP к исполняемому файлу
target_link_libraries(course PRIVATE OpenMP::OpenMP_CXX)