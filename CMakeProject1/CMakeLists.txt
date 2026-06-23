cmake_minimum_required(VERSION 3.10)

# Nazwa Twojego projektu
project(CMakeProject1)

# Szukamy pakietów zainstalowanych przez vcpkg
find_package(dpp REQUIRED)
find_package(unofficial-libmysql CONFIG REQUIRED)

# DODAWANIE PLIKÓW: Tutaj musisz podać wszystkie pliki .cpp, które bierzesz do projektu
# Upewnij się, że nazwa głównego pliku to na pewno CMakeProject1.cpp!
add_executable(CMakeProject1 
    CMakeProject1.cpp 
    sql_functions.cpp
)

# Łączenie bibliotek z Twoim programem
target_link_libraries(CMakeProject1 PRIVATE 
    dpp::dpp
    unofficial::libmysql::libmysql
)