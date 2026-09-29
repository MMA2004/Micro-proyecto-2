# Micro-Proyecto No. 2 - Manipulating Images with Parallel Programming

## Diseño 2: Versión Secuencial con Filtros (`filterer`)

Este módulo implementa el procesamiento visual y la convolución 2D de imágenes en **ejecución secuencial** en C++, actuando como la **línea base de rendimiento** para comparar la aceleración (*Speedup*) con los diseños paralelos (Pthreads, OpenMP y MPI).

---

## 1. Guía Rápida de Uso en Docker (Paso a Paso Verificado)

### Paso 1: Iniciar el contenedor de Docker
Desde PowerShell en tu máquina Windows:
```powershell
docker run --rm -it japeto/parallel-tools:v64 bash
```

---

### Paso 2: Compilar el ejecutable secuencial
Dentro del contenedor en `/app/Micro-proyecto-2`:
```bash
make filterer
```
*(O si prefieres compilar la suite completa: `make`).*

---

### Paso 3: Ejecutar los filtros individuales

La sintaxis establecida por la guía es:
```bash
./filterer <imagen_entrada> <imagen_salida> --f <filtro>
```

#### Ejemplos con las entradas solicitadas por la guía (fruit, lena, puj):

```bash
# 1. Filtro Suavizado (Blur) a color (PPM):
./filterer samples/anillo.ppm samples/anillo_blur.ppm --f blur

# 2. Filtro Laplaciano (Detección de Bordes) a color:
./filterer samples/anillo.ppm samples/anillo_laplace.ppm --f laplace

# 3. Filtro Realce (Sharpening) a color:
./filterer samples/anillo.ppm samples/anillo_sharpen.ppm --f sharpen

# 4. Filtro Laplaciano en escala de grises (PGM):
./filterer samples/anillo.pgm samples/anillo_laplace.pgm --f laplace
```

---

### Paso 4: Encadenar múltiples filtros en una sola ejecución

El ejecutable `filterer` permite aplicar una secuencia de dos o más filtros consecutivamente:
```bash
./filterer samples/anillo.ppm samples/anillo_multi.ppm --f blur --f sharpen
```

---

### Paso 5: Ejecutar la suite de pruebas automáticas
Para procesar automáticamente imágenes tanto en formato PPM (color) como PGM (escala de grises) con todos los filtros:
```bash
make test-filterer
```

---

## 2. Salida Esperada en Pantalla y Métricas de Rendimiento

El programa reporta las dimensiones de la imagen y mide con alta precisión el **tiempo de CPU** (`std::clock`) y el **tiempo total de reloj (Wall-clock)** (`std::chrono`), aislando la fase de cálculo matemático de los tiempos de lectura y escritura en disco:

```text
[Filterer] Leyendo archivo de entrada: samples/fruit.ppm...
[Filterer] Imagen cargada:
  - Tipo:        P3
  - Resolucion:  512 x 512
  - Canales:     3
[Filterer] Aplicando secuencia de 1 filtro(s)...
  -> Paso 1/1: Suavizado Promedio (Blur)
----------------------------------------
 Reporte de Tiempo: Filtrado Secuencial (Diseno 2)
----------------------------------------
 Tiempo de CPU:       18.4200 ms (0.0184 s)
 Tiempo Total (Wall): 18.5100 ms (0.0185 s)
----------------------------------------
[Filterer] Guardando imagen resultante en: samples/fruit_blur.ppm...
[Filterer] Procesamiento completado exitosamente.
```

> **Importante para el Informe:** Este tiempo total de reloj ($T_{\text{sec}}$) es la **línea base** requerida para calcular el **Speedup** de los Diseños 3 y 4:
> $$S = \frac{T_{\text{sec}}}{T_{\text{paralelo}}}$$

---

## 3. Catálogo de Filtros Implementados y Kernels 3x3

| Filtro | Bandera CLI | Matriz de Convolución 3x3 | Efecto Visual |
| :--- | :--- | :--- | :--- |
| **Suavizado (Blur)** | `--f blur` | $\begin{bmatrix} 1/9 & 1/9 & 1/9 \\ 1/9 & 1/9 & 1/9 \\ 1/9 & 1/9 & 1/9 \end{bmatrix}$ | Reduce el ruido homogeneizando los píxeles vecinos. |
| **Desenfoque Gaussiano** | `--f gaussian` | $\frac{1}{16}\begin{bmatrix} 1 & 2 & 1 \\ 2 & 4 & 2 \\ 1 & 2 & 1 \end{bmatrix}$ | Suavizado ponderado que preserva mejor las estructuras generales. |
| **Laplaciano (Bordes)** | `--f laplace` | $\begin{bmatrix} -1 & -1 & -1 \\ -1 & 8 & -1 \\ -1 & -1 & -1 \end{bmatrix}$ | Resalta los cambios bruscos de intensidad lumínica (contornos). |
| **Realce (Sharpen)** | `--f sharpen` | $\begin{bmatrix} 0 & -1 & 0 \\ -1 & 5 & -1 \\ 0 & -1 & 0 \end{bmatrix}$ | Incrementa el contraste y la nitidez de los detalles finos. |
| **Sobel** | `--f sobel` | $G = \sqrt{G_x^2 + G_y^2}$ con máscaras Sobel $3\times3$ | Detección direccional de gradientes (pág. 4 de la guía). |

---

## 4. Fundamentos Técnicos y Estructuras de Datos

1. **Reemplazo de `std::vector`:**
   - La prueba de clase (`codigo_filtro.cpp`) utilizaba `vector<vector<double>>` para los kernels y `vector<vector<Pixel>>` para los píxeles.
   - En este diseño se transformaron a matrices estáticas `double kernel[3][3]` y un buffer continuo 1D (`unsigned char* data`).
   - **Beneficios:** Garantiza **localidad espacial de caché**, cero sobrecosto por indirección de punteros, y sienta la base para la división en cuadrantes de Pthreads y franjas continuas de MPI.

2. **Reemplazo de `std::string`:**
   - Se utiliza el módulo `CharUtils` con punteros `const char*` y funciones estándar de `<cstring>`, cumpliendo la directriz académica.

3. **Tratamiento de Bordes (*Edge Clamping*):**
   - En lugar de ignorar los bordes o rellenar con ceros (lo que provocaría un borde negro artificial en el filtrado laplaciano), se implementa sujeción perimetral (*clamp to edge*), proyectando el píxel válido más cercano.

4. **Soporte Multicanal Automático:**
   - La misma función de convolución procesa imágenes de 1 canal (**PGM**) o 3 canales (**PPM**) iterando sobre $c \in [0, \text{canales}-1]$.

---

## 5. Compilación Manual (Sin Make)

Si prefieres compilar directamente con `g++`:

```bash
g++ -std=c++17 -O2 -Iinclude src/Image.cpp src/PGMImage.cpp src/PPMImage.cpp src/ImageIO.cpp src/CharUtils.cpp src/Timer.cpp src/ConvolutionFilter.cpp src/BlurFilter.cpp src/LaplaceFilter.cpp src/SharpenFilter.cpp src/SobelFilter.cpp src/filter_main.cpp -o filterer
```
