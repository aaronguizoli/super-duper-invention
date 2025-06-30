cmake_minimum_required(VERSION 3.10)

project(ufmg_carona)

# Define o padrão do C++ para C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define o diretório de saída para os executáveis (ex: build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Adiciona o diretório 'include' para que os cabeçalhos sejam encontrados
include_directories(include)

# Encontra todos os arquivos .cpp na pasta src
file(GLOB SOURCES "src/*.cpp")

# Cria o executável principal da aplicação
add_executable(app_carona ${SOURCES})

# Passa o caminho absoluto do projeto para o código C++
target_compile_definitions(app_carona PRIVATE "PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")

# Copia o arquivo de dados para a pasta de build junto com o executável
add_custom_command(
    TARGET app_carona POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/dados_ufmg.txt"
        "$<TARGET_FILE_DIR:app_carona>/dados_ufmg.txt"
    COMMENT "Copiando dados_ufmg.txt para o diretorio do executavel"
)

# --- CONFIGURAÇÃO DE TESTES COM DOCTEST ---

# Habilita o ctest, a ferramenta de teste do CMake
enable_testing()

# Adiciona o subdiretório de testes. O CMake irá procurar por um
# CMakeLists.txt dentro da pasta 'test' e executá-lo.
add_subdirectory(test)
