cmake_minimum_required(VERSION 3.10)
project(3d_on_cpp)

# Поиск библиотек (если они установлены в системе или через vcpkg)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)

# Добавляем все ваши файлы
file(GLOB SOURCES "3d_on_cpp/*.cpp")

add_executable(main ${SOURCES})

# Связываем библиотеки с вашим проектом
target_link_libraries(main sfml-graphics sfml-window sfml-system glfw glm::glm nlohmann_json::nlohmann_json)
