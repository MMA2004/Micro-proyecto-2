CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2 -Iinclude -pthread -fopenmp

SRC_DIR := src
INC_DIR := include
OBJ_DIR := build
BIN_DIR := bin

# Modulos compartidos
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
               $(SRC_DIR)/SobelFilter.cpp \
               $(SRC_DIR)/PthreadProcessor.cpp \
               $(SRC_DIR)/OpenMPProcessor.cpp

COMMON_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(COMMON_SRCS))

TARGET_PROCESSOR := processor
TARGET_FILTERER  := filterer
TARGET_THREADS   := th_filterer
TARGET_OMP       := omp_filterer

.PHONY: all clean test test-filterer test-threads test-omp test-compare dirs

all: dirs $(TARGET_PROCESSOR) $(TARGET_FILTERER) $(TARGET_THREADS) $(TARGET_OMP)

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Ejecutable Diseño 1: Aplicación base
$(TARGET_PROCESSOR): $(OBJ_DIR)/main.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Ejecutable Diseño 2: Versión secuencial con filtros
$(TARGET_FILTERER): $(OBJ_DIR)/filter_main.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Ejecutable Diseño 3 (Parte A): Paralelización por 4 cuadrantes con Pthreads
$(TARGET_THREADS): $(OBJ_DIR)/th_main.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Ejecutable Diseño 3 (Parte B): Paralelización con OpenMP
$(TARGET_OMP): $(OBJ_DIR)/omp_main.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla de compilación de objetos con creación automática de carpeta build
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

# Pruebas Diseño 2: Filtros secuenciales
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

# Pruebas Diseño 3 (Parte A): Pthreads por 4 cuadrantes
test-threads: $(TARGET_THREADS)
	@echo "=== [Diseno 3 - Pthreads] Filtro Blur en damma.ppm (4 cuadrantes) ==="
	./$(TARGET_THREADS) samples/damma.ppm samples/output_th_damma.ppm --f blur
	@echo "\n=== [Diseno 3 - Pthreads] Filtro Laplace en sulfur.pgm (4 cuadrantes) ==="
	./$(TARGET_THREADS) samples/sulfur.pgm samples/output_th_sulfur.pgm --f laplace
	@echo "\nPruebas de Pthreads finalizadas con exito."

# Pruebas Diseño 3 (Parte B): OpenMP
test-omp: $(TARGET_OMP)
	@echo "=== [Diseno 3 - OpenMP] Filtros en sulfur.pgm ==="
	./$(TARGET_OMP) samples/sulfur.pgm samples/output_omp_sulfur.pgm
	@echo "\n=== [Diseno 3 - OpenMP] Filtro Blur en damma.ppm ==="
	./$(TARGET_OMP) samples/damma.ppm samples/output_omp_damma.ppm --f blur
	@echo "\nPruebas de OpenMP finalizadas con exito."

# Comparativa de rendimiento: Secuencial vs Pthreads vs OpenMP
test-compare: $(TARGET_FILTERER) $(TARGET_THREADS) $(TARGET_OMP)
	@echo "=========================================================="
	@echo " COMPARATIVA DE RENDIMIENTO: Secuencial vs Pthreads vs OMP"
	@echo "=========================================================="
	@echo "\n1. Ejecucion Secuencial (Diseno 2):"
	./$(TARGET_FILTERER) samples/damma.ppm samples/output_seq.ppm --f blur
	@echo "\n2. Ejecucion Paralela con Pthreads (4 cuadrantes, Diseno 3):"
	./$(TARGET_THREADS) samples/damma.ppm samples/output_th.ppm --f blur
	@echo "\n3. Ejecucion Paralela con OpenMP (Diseno 3):"
	./$(TARGET_OMP) samples/damma.ppm samples/output_omp.ppm --f blur
	@echo "\nComparativa completada."

clean:
	rm -rf $(OBJ_DIR) $(TARGET_PROCESSOR) $(TARGET_PROCESSOR).exe \
	       $(TARGET_FILTERER) $(TARGET_FILTERER).exe \
	       $(TARGET_THREADS) $(TARGET_THREADS).exe \
	       $(TARGET_OMP) $(TARGET_OMP).exe \
	       samples/output_* samples/pipe_*
