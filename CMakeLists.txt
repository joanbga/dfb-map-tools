cmake_minimum_required(VERSION 3.16)

# Nom du projet
project(MapUtils 
    VERSION 1.0.0
    DESCRIPTION "Map Utils for dofus"
    LANGUAGES CXX)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options de compilation selon le compilateur
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Options spécifiques à MSVC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /MDd")
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /MD")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Options pour GCC et Clang (Linux, MinGW, etc.)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    endif()
endif()

# Définir le répertoire des sources
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Créer une bibliothèque statique pour MapCell
add_library(MapCellLib STATIC
    ${SRC_DIR}/errors/DfbException.hpp

    ${SRC_DIR}/Commands.hpp
    ${SRC_DIR}/MapCell.cpp
    ${SRC_DIR}/MapCell.hpp
    ${SRC_DIR}/Map.hpp
    ${SRC_DIR}/Map.cpp
    
    ${SRC_DIR}/MapReader.hpp
    ${SRC_DIR}/MapReader.cpp
    ${SRC_DIR}/Neighbor.hpp
    
)

# Propriétés de la bibliothèque
target_include_directories(MapCellLib PUBLIC 
    ${SRC_DIR}
    ${SRC_DIR}/errors  # Ajouter cette ligne
)

# Exécutable principal
add_executable(map_utils ${SRC_DIR}/main.cpp)

# Lier la bibliothèque à l'exécutable
target_link_libraries(map_utils PRIVATE MapCellLib)

# Installation (optionnelle)
install(TARGETS map_utils 
    RUNTIME DESTINATION bin)

install(FILES ${SRC_DIR}/MapCell.hpp 
    DESTINATION include)

install(TARGETS MapCellLib
    ARCHIVE DESTINATION lib)

# Affichage d'informations sur la configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")

# Configuration CPack pour créer des packages
set(CPACK_PACKAGE_NAME "MapUtils")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "Votre Nom")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)