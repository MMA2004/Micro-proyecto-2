# Micro-Proyecto No. 2 - Manipulating images with Parallel programming

Este repositorio contiene la solución modular para el procesamiento secuencial y paralelo de imágenes en formatos NetPBM (PPM y PGM).

---

## 1. Diseño 1: Aplicación Base (`processor`)

- **Objetivo:** Carga, clonación, serialización y validación de imágenes PGM (P2) y PPM (P3) sin alteración de datos.
- **E/S:** Admite argumentos de línea de comandos, redirección estándar (`stdin`) y tuberías (`pipes`).

---

## 2. Diseño 2: Versión Secuencial de Filtros (`filterer`)

- **Objetivo:** Procesamiento visual de imágenes mediante filtros de convolución 2D aplicados de forma secuencial.
- **Filtros Implementados:**
  - `blur` (Suavizado / Promedio 3x3)
  - `gaussian` (Desenfoque Gaussiano 3x3 normalizado sobre 16)
  - `laplace` (Detección de bordes Laplaciano 3x3)
  - `sharpen` (Realce / Sharpening 3x3)
  - `sobel` (Magnitud de gradiente Sobel $G = \sqrt{G_x^2 + G_y^2}$)
- **Encadenamiento:** Permite aplicar una secuencia de uno o varios filtros consecutivamente:
  ```bash
  ./filterer fruit.ppm fruit_out.ppm --f blur --f sharpen
  ```
- **Medición de Rendimiento:**
  - Registra el **Tiempo de CPU** (`std::clock`) y el **Tiempo Total / Wall-clock** (`std::chrono::high_resolution_clock`) exclusivamente durante la fase de convolución numérica (aislando los tiempos de lectura/escritura de disco).

---

## 3. Justificación Técnica y Estructuras de Datos

1. **Reemplazo de `std::vector`:**
   - La prueba de concepto de clase (`ejemplo clase/codigo_filtro.cpp`) usaba `vector<vector<double>>` para kernels y `vector<vector<Pixel>>` para la imagen.
   - En esta implementación, se transformaron a matrices numéricas estáticas continuas `double kernel[3][3]` y un buffer unidimensional dinámico contiguo (`unsigned char* data`).
   - **Beneficios:** Máxima localidad espacial de caché, cero sobrecosto por indirección de punteros, y total compatibilidad con las directivas de paralelismo de **OpenMP**, cuadrantes en **Pthreads** y envío de buffers contiguos con **MPI**.
2. **Reemplazo de `std::string`:**
   - Se utiliza el módulo `CharUtils` con punteros `const char*` y funciones estándar de `<cstring>`, cumpliendo la recomendación académica.
3. **Manejo de Bordes:**
   - Se implementa sujeción de bordes (*clamping*) para asegurar que ningún píxel periférico produzca accesos fuera de rango o artefactos oscuros artificiales.

---

## 4. Compilación y Pruebas en Docker / Linux

### Compilación completa con `make`:
```bash
make
```

### Compilación manual con `g++` (sin herramientas adicionales):
```bash
# Compilar 'filterer'
g++ -std=c++17 -O2 -Iinclude src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/Timer.cpp src/ConvolutionFilter.cpp src/BlurFilter.cpp src/LaplaceFilter.cpp src/SharpenFilter.cpp src/SobelFilter.cpp src/filter_main.cpp -o filterer

# Compilar 'processor'
g++ -std=c++17 -O2 -Iinclude src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/main.cpp -o processor
```

---

## 5. Ejemplos de Uso

```bash
# Filtro Blur a imagen a color
./filterer samples/sample.ppm samples/output_blur.ppm --f blur

# Filtro Laplace a imagen a color
./filterer samples/sample.ppm samples/output_laplace.ppm --f laplace

# Filtro Realce a imagen en escala de grises
./filterer samples/sample.pgm samples/output_sharpen.pgm --f sharpen

# Aplicar múltiples filtros encadenados
./filterer samples/sample.ppm samples/output_multi.ppm --f blur --f sharpen
```

### Ejecución de pruebas automáticas:
```bash
make test-filterer
```
