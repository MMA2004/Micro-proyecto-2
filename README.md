# Micro-Proyecto No. 2 - Manipulating Images with Parallel Programming

## Diseño 3: Memoria Compartida (Pthreads y OpenMP)

Este módulo implementa la paralelización en **memoria compartida** utilizando dos enfoques distintos sobre imágenes NetPBM (PPM y PGM):
1. **Parte A (`th_filterer`):** Paralelización por división espacial en **4 cuadrantes geográficos** utilizando hilos POSIX (**Pthreads**).
2. **Parte B (`omp_filterer`):** Paralelización a nivel de bucles de filas mediante directivas de compilador (**OpenMP**).

---

## 1. Guía Rápida de Uso en Docker (Paso a Paso Verificado)

### Paso 1: Iniciar el contenedor de Docker
Desde PowerShell en tu máquina Windows:
```powershell
docker run --rm -it -v "C:\Users\mma\Documents\paralela:/app" -w /app/Micro-proyecto-2 japeto/parallel-tools:v64 bash
```

---

### Paso 2: Compilar los ejecutables de Diseño 3
Dentro del contenedor:
```bash
make th_filterer omp_filterer
```
*(O simplemente `make` para compilar la suite completa).*

---

### Paso 3: Ejecutar la versión Pthreads (`th_filterer` - 4 cuadrantes)

Aplica el filtro dividiendo la imagen en 4 cuadrantes concurrentes con 4 hilos:

```bash
# Probar filtro Suavizado (Blur) en damma.ppm a color:
./th_filterer samples/anillo.ppm samples/output_blur_th.ppm --f blur

# Probar filtro Laplaciano (Bordes) en sulfur.pgm en escala de grises:
./th_filterer samples/anillo.pgm samples/output_laplace_th.pgm --f laplace
```

#### Salida esperada en pantalla (Pthreads):
```text
[th_filterer] Leyendo archivo de entrada: samples/damma.ppm...
[th_filterer] Imagen cargada:
  - Tipo:        P3
  - Resolucion:  8 x 8
  - Canales:     3
[th_filterer] Iniciando procesamiento paralelo por 4 cuadrantes...
  -> Filtro 1/1: Suavizado Promedio (Blur)
[Pthreads] Lanzando 4 hilos por cuadrantes:
  - Hilo 0 [Arriba-Izquierda (Top-Left)]: X=[0, 4), Y=[0, 4)
  - Hilo 1 [Arriba-Derecha (Top-Right)]:  X=[4, 8), Y=[0, 4)
  - Hilo 2 [Abajo-Izquierda (Bottom-Left)]: X=[0, 4), Y=[4, 8)
  - Hilo 3 [Abajo-Derecha (Bottom-Right)]:  X=[4, 8), Y=[4, 8)
----------------------------------------
 Reporte de Tiempo: Paralelo Pthreads (4 Cuadrantes)
----------------------------------------
 Tiempo de CPU:       0.1200 ms (0.0001 s)
 Tiempo Total (Wall): 0.1350 ms (0.0001 s)
----------------------------------------
[th_filterer] Guardando imagen resultante en: samples/output_blur_th.ppm...
```

---

### Paso 4: Ejecutar la versión OpenMP (`omp_filterer`)

Aplica el filtro paralelizando las filas con directivas `#pragma omp`:

```bash
# Ejecutar con 4 hilos en damma.ppm:
./omp_filterer samples/anillo.ppm samples/output_blur_omp.ppm --f blur --t 4

# Aplicar los 3 filtros principales automáticamente (Blur + Laplace + Sharpen) a sulfur.pgm:
./omp_filterer samples/anillo.pgm samples/output_omp_all.pgm
```

#### Salida esperada en pantalla (OpenMP):
```text
[omp_filterer] Leyendo archivo de entrada: samples/damma.ppm...
[omp_filterer] Imagen cargada:
  - Tipo:        P3
  - Resolucion:  8 x 8
  - Canales:     3
[omp_filterer] Iniciando procesamiento con OpenMP...
  -> Filtro 1/1: Suavizado Promedio (Blur)
[OpenMP] Ejecutando bucle paralelo con 4 hilos...
----------------------------------------
 Reporte de Tiempo: Paralelo OpenMP
----------------------------------------
 Tiempo de CPU:       0.1100 ms (0.0001 s)
 Tiempo Total (Wall): 0.1210 ms (0.0001 s)
----------------------------------------
[omp_filterer] Guardando imagen resultante en: samples/output_blur_omp.ppm...
```

---

### Paso 5: Comparativa Directa de Rendimiento (Secuencial vs Pthreads vs OpenMP)

Para ver la comparativa de tiempos de los tres enfoques sobre la misma imagen en una sola pantalla:
```bash
make test-compare
```

---

## 2. Filtros Disponibles

Ambos ejecutables soportan los siguientes filtros mediante la bandera `--f`:
* `--f blur` : Suavizado promedio uniforme 3x3 ($1/9$).
* `--f gaussian` : Desenfoque gaussiano 3x3 ($1/16$).
* `--f laplace` : Detección de bordes laplaciano 3x3.
* `--f sharpen` : Realce de detalles 3x3.
* `--f sobel` : Magnitud de gradiente Sobel horizontal y vertical.

Se pueden encadenar múltiples filtros en la misma ejecución:
```bash
./th_filterer samples/damma.ppm samples/output_combo.ppm --f blur --f sharpen
```

---

## 3. Fundamentos Técnicos y Respuestas para el Informe

### 1. Esquema de 4 Cuadrantes y Ausencia de Condiciones de Carrera (*Race-Free*):
* La imagen fuente `src` es **de solo lectura (inmutable)**.
* Cada uno de los 4 hilos POSIX calcula y escribe exclusivamente en su propio cuadrante dentro de la imagen destino `dst`.
* Dado que las regiones de escritura son mutuamente disyuntas ($\text{Región}_i \cap \text{Región}_j = \emptyset$), **no existen condiciones de carrera** y no se requiere el uso de *mutexes* ni semáforos de sincronización durante el filtrado.

```text
(0, 0)
  ┌──────────────────────┬──────────────────────┐ (W, 0)
  │     CUADRANTE 0      │     CUADRANTE 1      │
  │   Arriba-Izquierda   │    Arriba-Derecha    │
  │  [0, midX) x [0, midY)│ [midX, W) x [0, midY)│
  │       (Hilo 0)       │       (Hilo 1)       │
  ├──────────────────────┼──────────────────────┤ (W, midY)
  │     CUADRANTE 2      │     CUADRANTE 3      │
  │   Abajo-Izquierda    │    Abajo-Derecha     │
  │  [0, midX) x [midY, H)│ [midX, W) x [midY, H)│
  │       (Hilo 2)       │       (Hilo 3)       │
  └──────────────────────┴──────────────────────┘
(0, H)                                          (W, H)
```

### 2. Estructuras de Datos Utilizadas:
* **Estructura de contexto por hilo (`ThreadRegionData`):** Almacena punteros inmutables `const Image* src`, puntero mutable `Image* dst` y los límites rectangulares $[start_x, end_x) \times [start_y, end_y)$.
* **Buffers contiguos 1D (`unsigned char* data`):** Máxima localidad espacial de caché L1/L2.
* **Matrices estáticas `double[3][3]`:** Sin sobrecosto de indirección.
* **Cero `std::vector` y Cero `std::string`:** Cumplimiento estricto de las directivas del curso.

### 3. Fórmulas de Rendimiento:
* **Speedup (Aceleración):**
  $$S_p = \frac{T_{\text{secuencial}}}{T_{\text{paralelo}}}$$
* **Eficiencia:**
  $$E_p = \frac{S_p}{P} \quad (P = 4 \text{ hilos})$$

---

## 4. Compilación Manual (Sin Make)

Si deseas compilar manualmente con `g++`:

```bash
# Compilar th_filterer (Pthreads)
g++ -std=c++17 -O2 -Iinclude -pthread src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/Timer.cpp src/ConvolutionFilter.cpp src/BlurFilter.cpp src/LaplaceFilter.cpp src/SharpenFilter.cpp src/SobelFilter.cpp src/PthreadProcessor.cpp src/th_main.cpp -o th_filterer

# Compilar omp_filterer (OpenMP)
g++ -std=c++17 -O2 -Iinclude -fopenmp src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/Timer.cpp src/ConvolutionFilter.cpp src/BlurFilter.cpp src/LaplaceFilter.cpp src/SharpenFilter.cpp src/SobelFilter.cpp src/OpenMPProcessor.cpp src/omp_main.cpp -o omp_filterer
```
