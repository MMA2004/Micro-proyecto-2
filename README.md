# Micro-Proyecto No. 2 - Manipulating images with Parallel programming

Este repositorio contiene la solución modular para el procesamiento secuencial y paralelo de imágenes en formatos NetPBM (PPM y PGM).

---

## 1. Módulos y Diseños Implementados

### Diseño 1: Aplicación Base (`processor`)
- **Objetivo:** Carga, clonación, serialización y validación de imágenes PGM (P2) y PPM (P3) sin alteración de datos.
- **E/S:** Admite argumentos de línea de comandos, redirección estándar (`stdin`) y tuberías (`pipes`).

### Diseño 2: Versión Secuencial con Filtros (`filterer`)
- **Objetivo:** Convolución 2D secuencial de filtros visuales: `blur`, `gaussian`, `laplace`, `sharpen`, `sobel`.
- **Características:** Soporte para encadenar múltiples filtros (`--f blur --f sharpen`). Medición aislada de tiempos de convolución.

### Diseño 3: Memoria Compartida (`th_filterer` y `omp_filterer`)
- **Parte A (`th_filterer`):**
  - Divide la imagen en **4 cuadrantes geográficos independientes**:
    - Arriba-Izquierda: $[0, mid_X) \times [0, mid_Y)$
    - Arriba-Derecha:   $[mid_X, W) \times [0, mid_Y)$
    - Abajo-Izquierda:  $[0, mid_X) \times [mid_Y, H)$
    - Abajo-Derecha:    $[mid_X, W) \times [mid_Y, H)$
  - Asigna 4 hilos POSIX (`pthread_t`) a cada región.
  - **Sin condiciones de carrera (Race-Free):** La imagen fuente `src` es de solo lectura y las regiones de escritura en `dst` son mutuamente disyuntas.
- **Parte B (`omp_filterer`):**
  - Paralelización a nivel de bucles de filas mediante directivas `#pragma omp parallel for schedule(dynamic)`.
  - Soporta parametrizar número de hilos con `--t <num_hilos>`.

---

## 2. Justificación Técnica y Respuestas para el Informe

1. **¿Por qué los diseños pueden o no aprovechar múltiples núcleos del procesador?**
   - **Pthreads:** Al particionar la matriz de la imagen en 4 cuadrantes, el sistema operativo distribuye los 4 hilos en hasta 4 núcleos lógicos o físicos simultáneamente, reduciendo el tiempo de cálculo.
   - **OpenMP:** Descompone las $H$ filas de la imagen dinámicamente entre todos los núcleos disponibles, ofreciendo un balanceo de carga adaptativo.
2. **¿Qué estructuras de datos se utilizaron?**
   - Buffers continuos 1D `unsigned char* data`, estructuras de región `ThreadRegionData` y matrices estáticas `double[3][3]`, eliminando por completo `std::vector` y `std::string` para maximizar la localidad espacial en la caché de CPU.
3. **Cálculo de Aceleración y Eficiencia:**
   - **Speedup:** $S_p = \frac{T_{\text{secuencial}}}{T_{\text{paralelo}}}$
   - **Eficiencia:** $E_p = \frac{S_p}{P}$

---

## 3. Compilación

Para compilar todos los ejecutables (`processor`, `filterer`, `th_filterer`, `omp_filterer`):

```bash
make
```

O compilación manual con `g++`:

```bash
# Compilar th_filterer (Pthreads)
g++ -std=c++17 -O2 -Iinclude -pthread src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/Timer.cpp src/ConvolutionFilter.cpp src/BlurFilter.cpp src/LaplaceFilter.cpp src/SharpenFilter.cpp src/SobelFilter.cpp src/PthreadProcessor.cpp src/OpenMPProcessor.cpp src/th_main.cpp -o th_filterer

# Compilar omp_filterer (OpenMP)
g++ -std=c++17 -O2 -Iinclude -fopenmp src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/Timer.cpp src/ConvolutionFilter.cpp src/BlurFilter.cpp src/LaplaceFilter.cpp src/SharpenFilter.cpp src/SobelFilter.cpp src/PthreadProcessor.cpp src/OpenMPProcessor.cpp src/omp_main.cpp -o omp_filterer
```

---

## 4. Ejecución de Pruebas

```bash
# Probar versión Pthreads (4 cuadrantes en damma y sulfur)
make test-threads

# Probar versión OpenMP
make test-omp

# Comparativa directa de rendimiento (Secuencial vs Pthreads vs OpenMP)
make test-compare
```

---

## 5. Ejemplos de Uso

```bash
# Pthreads: Filtro blur en damma.ppm
./th_filterer samples/damma.ppm samples/output_blur_th.ppm --f blur

# Pthreads: Filtro laplace en sulfur.pgm
./th_filterer samples/sulfur.pgm samples/output_laplace_th.pgm --f laplace

# OpenMP: Aplicar los 3 filtros a sulfur.pgm
./omp_filterer samples/sulfur.pgm samples/output_omp.pgm

# OpenMP: Filtro sharpen con 4 hilos
./omp_filterer samples/damma.ppm samples/output_sharpen_omp.ppm --f sharpen --t 4
```
