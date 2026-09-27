# Micro-Proyecto No. 2 - Manipulating images with Parallel programming

Solución integral y modular para el procesamiento secuencial y paralelo de imágenes en formatos NetPBM (PPM y PGM) en C++.

---

## 1. Módulos y Diseños del Proyecto

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

### Diseño 4: Memoria Distribuida (`mpi_filterer`)
- **Objetivo:** Paralelización mediante paso de mensajes con **MPI** entre procesos o contenedores Docker sin memoria compartida.
- **Descomposición de Dominio (Slabs):**
  - Divide la imagen en franjas horizontales de filas entre $P$ procesos.
  - **Manejo de Halos (Ghost Rows):** El nodo maestro (Rank 0) envía a cada worker su franja con 1 fila superior e inferior de margen para calcular la convolución 3x3 sin discontinuidades ni artefactos en las fronteras.
  - Cada worker devuelve únicamente sus filas útiles procesadas (descartando los halos).
- **Métricas Individuales:** Registra el tiempo de CPU y Wall-clock individual de cada nodo.

---

## 2. Justificación Técnica y Respuestas para el Informe

1. **¿Por qué los diseños pueden o no aprovechar múltiples núcleos del procesador?**
   - **Pthreads:** Al particionar en 4 cuadrantes, el planificador del sistema operativo asigna los 4 hilos a diferentes núcleos lógicos/físicos en memoria compartida.
   - **OpenMP:** Distribuye dinámicamente las $H$ filas entre todos los núcleos disponibles con `#pragma omp parallel for schedule(dynamic)`.
   - **MPI:** Cada proceso es una entidad completamente aislada con su propia memoria privada. Puede ejecutarse en múltiples núcleos del mismo procesador o en múltiples máquinas/contenedores en red.
2. **¿Qué estructuras de datos se utilizaron?**
   - Buffers continuos 1D `unsigned char* data`, estructuras de franja `ChunkHeaderMsg`, estructuras de métricas `NodeTimingMsg` y matrices estáticas `double[3][3]`. **Cero `std::vector` y cero `std::string`** en estricto cumplimiento con las restricciones académicas.
3. **Cálculo de Rendimiento:**
   - **Speedup (Aceleración):** $S_p = \frac{T_{\text{secuencial}}}{T_{\text{paralelo}}}$
   - **Eficiencia:** $E_p = \frac{S_p}{P}$

---

## 3. Compilación

### Compilación completa con `make`:
```bash
make
```

### Compilación manual de cada ejecutable con `g++` / `mpicxx`:
```bash
# Diseño 1: processor
g++ -std=c++17 -O2 -Iinclude src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/main.cpp -o processor

# Diseño 2: filterer (Secuencial)
g++ -std=c++17 -O2 -Iinclude src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/Timer.cpp src/ConvolutionFilter.cpp src/BlurFilter.cpp src/LaplaceFilter.cpp src/SharpenFilter.cpp src/SobelFilter.cpp src/filter_main.cpp -o filterer

# Diseño 3A: th_filterer (Pthreads)
g++ -std=c++17 -O2 -Iinclude -pthread src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/Timer.cpp src/ConvolutionFilter.cpp src/BlurFilter.cpp src/LaplaceFilter.cpp src/SharpenFilter.cpp src/SobelFilter.cpp src/PthreadProcessor.cpp src/th_main.cpp -o th_filterer

# Diseño 3B: omp_filterer (OpenMP)
g++ -std=c++17 -O2 -Iinclude -fopenmp src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/Timer.cpp src/ConvolutionFilter.cpp src/BlurFilter.cpp src/LaplaceFilter.cpp src/SharpenFilter.cpp src/SobelFilter.cpp src/OpenMPProcessor.cpp src/omp_main.cpp -o omp_filterer

# Diseño 4: mpi_filterer (MPI)
mpicxx -std=c++17 -O2 -Iinclude src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/Timer.cpp src/ConvolutionFilter.cpp src/BlurFilter.cpp src/LaplaceFilter.cpp src/SharpenFilter.cpp src/SobelFilter.cpp src/MPIProcessor.cpp src/mpi_main.cpp -o mpi_filterer
```

---

## 4. Ejecución de Pruebas y Comandos

### Pruebas individuales por diseño:
```bash
# Probar Diseño 1 (Base):
make test

# Probar Diseño 2 (Filtros Secuenciales):
make test-filterer

# Probar Diseño 3A (Pthreads 4 cuadrantes):
make test-threads

# Probar Diseño 3B (OpenMP):
make test-omp

# Probar Diseño 4 (MPI Memoria Distribuida con 4 procesos):
make test-mpi
```

### Comparativa simultánea de los 4 diseños (Ideal para el informe):
```bash
make test-all-designs
```

### Comandos manuales para MPI:
```bash
# Ejecutar MPI con 4 procesos aplicando blur a damma.ppm:
mpirun --allow-run-as-root -np 4 ./mpi_filterer samples/damma.ppm samples/output_mpi_damma.ppm --f blur

# Ejecutar MPI con 2 procesos aplicando laplace a sulfur.pgm:
mpirun --allow-run-as-root -np 2 ./mpi_filterer samples/sulfur.pgm samples/output_mpi_sulfur.pgm --f laplace

# Ejecutar MPI aplicando los tres filtros principales automáticamente:
mpirun --allow-run-as-root -np 4 ./mpi_filterer samples/sulfur.pgm samples/output_mpi_all.pgm
```
