CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2 -Iinclude

SRC_DIR := src
INC_DIR := include
OBJ_DIR := build
BIN_DIR := bin

# Objetos compartidos
COMMON_SRCS := $(SRC_DIR)/Image.cpp \
               $(SRC_DIR)/PGMImage.cpp \
               $(SRC_DIR)/PPMImage.cpp \
               $(SRC_DIR)/ImageIO.cpp \
               $(SRC_DIR)/CharUtils.cpp \
               $(SRC_DIR)/Timer.cpp \
               $(SRC_DIR)/ConvolutionFilter.cpp \
               $(SRC_DIR)/BlurFilter.cpp \
               $(SRC_DIR)/LaplaceFilter.cpp \
               $(SRC_DIR)/SharpenFilter.cpp \
               $(SRC_DIR)/SobelFilter.cpp

COMMON_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(COMMON_SRCS))

TARGET_PROCESSOR := processor
TARGET_FILTERER := filterer

.PHONY: all clean test test-filterer dirs

all: dirs $(TARGET_PROCESSOR) $(TARGET_FILTERER)

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Ejecutable Diseño 1: Aplicación base
$(TARGET_PROCESSOR): $(OBJ_DIR)/main.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Ejecutable Diseño 2: Versión secuencial con filtros
$(TARGET_FILTERER): $(OBJ_DIR)/filter_main.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla de compilación de objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pruebas Diseño 1
test: $(TARGET_PROCESSOR)
	@echo "=== [Diseno 1] Probando processor con PGM (P2) ==="
	./$(TARGET_PROCESSOR) samples/sample.pgm samples/output_sample.pgm
	@echo "\n=== [Diseno 1] Probando processor con PPM (P3) ==="
	./$(TARGET_PROCESSOR) samples/sample.ppm samples/output_sample.ppm
	@echo "\n=== [Diseno 1] Probando processor por stdin/pipe ==="
	./$(TARGET_PROCESSOR) < samples/sample.pgm > samples/pipe_output.pgm
	@echo "\nPruebas de Diseno 1 finalizadas con exito."

# Pruebas Diseño 2: Filtros
test-filterer: $(TARGET_FILTERER)
	@echo "=== [Diseno 2] Filtro Suavizado (Blur) ==="
	./$(TARGET_FILTERER) samples/sample.ppm samples/output_blur.ppm --f blur
	./$(TARGET_FILTERER) samples/sample.pgm samples/output_blur.pgm --f blur
	@echo "\n=== [Diseno 2] Filtro Laplace (Bordes) ==="
	./$(TARGET_FILTERER) samples/sample.ppm samples/output_laplace.ppm --f laplace
	./$(TARGET_FILTERER) samples/sample.pgm samples/output_laplace.pgm --f laplace
	@echo "\n=== [Diseno 2] Filtro Realce (Sharpen) ==="
	./$(TARGET_FILTERER) samples/sample.ppm samples/output_sharpen.ppm --f sharpen
	./$(TARGET_FILTERER) samples/sample.pgm samples/output_sharpen.pgm --f sharpen
	@echo "\n=== [Diseno 2] Filtro Sobel (Gradiente) ==="
	./$(TARGET_FILTERER) samples/sample.ppm samples/output_sobel.ppm --f sobel
	./$(TARGET_FILTERER) samples/sample.pgm samples/output_sobel.pgm --f sobel
	@echo "\n=== [Diseno 2] Encadenamiento: Blur + Sharpen ==="
	./$(TARGET_FILTERER) samples/sample.ppm samples/output_chained.ppm --f blur --f sharpen
	./$(TARGET_FILTERER) samples/sample.pgm samples/output_chained.pgm --f blur --f sharpen
	@echo "\nPruebas de Diseno 2 finalizadas con exito."

clean:
	rm -rf $(OBJ_DIR) $(TARGET_PROCESSOR) $(TARGET_PROCESSOR).exe $(TARGET_FILTERER) $(TARGET_FILTERER).exe samples/output_* samples/pipe_*
