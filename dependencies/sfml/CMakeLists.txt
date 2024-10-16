message(STATUS "Fetching SFML...")

# Делаем обязательными следующие модули
set(SFML_BUILD_GRAPHICS TRUE)
set(SFML_BUILD_SYSTEM TRUE)
set(SFML_BUILD_WINDOW TRUE)
set(SFML_BUILD_NETWORK TRUE)

FetchContent_MakeAvailable(sfml)