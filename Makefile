# Makefile Final com Estrutura de Diretórios Profissional

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -Iinclude # Alterado para C++11
LDFLAGS = -static-libgcc -static-libstdc++

# --- Estrutura de Diretórios ---
INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# --- Arquivos ---
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
EXECUTABLE = $(BIN_DIR)/app_carona

# --- Regras de Compilação ---
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Ligando o programa..."
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regras de Limpeza e Execução ---
clean:
	@echo "Limpando arquivos de build..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)

run: all
	@echo "Executando o programa..."
	./$(EXECUTABLE)

.PHONY: all clean run
