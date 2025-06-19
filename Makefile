# Makefile Final
# Conceitos: aula17-modularizacao.pdf
# Requisito do trabalho: PDS2-TP-2025-1.pdf

CXX = C:/msys64/mingw64/bin/g++.exe
CXXFLAGS = -std=c++17 -Wall -Wextra -g -Iinclude
LDFLAGS = -static-libgcc -static-libstdc++

INCLUDE_DIR = include
SRC_DIR = src
BUILD_DIR = build

SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/Sistema.cpp \
          $(SRC_DIR)/Usuario.cpp \
          $(SRC_DIR)/Aluno.cpp \
          $(SRC_DIR)/Funcionario.cpp \
          $(SRC_DIR)/Carona.cpp \
          $(SRC_DIR)/CaronaFactory.cpp \
          $(SRC_DIR)/Veiculo.cpp \
          $(SRC_DIR)/Rotina.cpp \
          $(SRC_DIR)/Pagamento.cpp \
          $(SRC_DIR)/Solicitacao.cpp \
          $(SRC_DIR)/Avaliacao.cpp \
          $(SRC_DIR)/Notificacao.cpp

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

EXECUTABLE = app_carona

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo "Ligando o programa..."
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p build
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Limpando arquivos de build..."
	rm -rf $(BUILD_DIR) *.o $(EXECUTABLE)

run: all
	@echo "Executando o programa..."
	./$(EXECUTABLE)

.PHONY: all clean run