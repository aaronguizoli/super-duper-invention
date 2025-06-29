CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -Iinclude
LDFLAGS = -static-libgcc -static-libstdc++

INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
EXECUTABLE = $(BIN_DIR)/app_carona

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Ligando o programa..."
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Limpando arquivos de build..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)

run: all
	@echo "Executando o programa..."
	./$(EXECUTABLE)

.PHONY: all clean run
