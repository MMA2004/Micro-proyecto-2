# Micro-Proyecto No. 2 - Manipulating Images with Parallel Programming

## Diseño 4: Memoria Distribuida con MPI en Docker

Este módulo implementa el procesamiento paralelo de imágenes en **memoria distribuida** utilizando el estándar **MPI (Message Passing Interface)** en C++ dentro de un clúster de contenedores Docker.

---

## 1. Guía Rápida de Uso (Paso a Paso Verificado)

Para levantar el entorno y ejecutar el procesamiento con **4 procesos/nodos MPI**, ejecuta los siguientes comandos en ese orden:

### Paso 1: Detener cualquier contenedor previo
Desde la terminal de tu máquina anfitriona (PowerShell o CMD en la carpeta del proyecto):
```powershell
docker compose down
```

### Paso 2: Levantar el clúster en segundo plano
```powershell
docker compose up -d
```
*(Esto levantará los contenedores de cómputo en la red virtual de Docker).*

### Paso 3: Entrar a la consola del nodo maestro (`mpi_master`)
```powershell
docker exec -it mpi_master bash
```
*(A partir de aquí, estarás dentro de la terminal Linux del contenedor maestro en `/app/Micro-proyecto-2`).*

### Paso 4: Compilar el ejecutable de MPI
```bash
make mpi_filterer
```
*(Compila `mpi_filterer` utilizando `mpicxx` con optimizaciones `-O2` y flags de C++17).*

### Paso 5: Ejecutar el filtrado distribuido con MPI
```bash
mpirun --allow-run-as-root -np 4 ./mpi_filterer samples/damma.ppm samples/output_mpi_damma.ppm --f blur
```

---

## 2. Salida Esperada en Pantalla

Al ejecutar el comando del Paso 5, verás la inicialización del clúster, los metadatos de la imagen cargada y el **reporte individual de tiempos por cada nodo**, tal como lo exige la rúbrica:

```text
[mpi_filterer] Iniciando clúster MPI con 4 procesos/nodos.
[mpi_filterer] Leyendo archivo de entrada: samples/damma.ppm...
[mpi_filterer] Imagen cargada:
  - Tipo:        P3
  - Resolucion:  8 x 8
  - Canales:     3

>>> Aplicando filtro 1/1: Suavizado Promedio (Blur)
============================================================
 Reporte de Tiempos MPI por Nodo (Memoria Distribuida)
 Filtro: Suavizado Promedio (Blur)
============================================================
  - Nodo 0 [Maestro]: CPU = 0.0820 ms | Wall = 0.0835 ms
  - Nodo 1 [Worker]:  CPU = 0.0610 ms | Wall = 0.0621 ms
  - Nodo 2 [Worker]:  CPU = 0.0590 ms | Wall = 0.0604 ms
  - Nodo 3 [Worker]:  CPU = 0.0640 ms | Wall = 0.0652 ms
------------------------------------------------------------
 Tiempo de calculo paralelo efectivo (Max Wall): 0.0835 ms
============================================================

============================================================
 Reporte de Tiempo: Filtrado Total MPI (Memoria Distribuida)
----------------------------------------
 Tiempo de CPU:       0.2800 ms (0.0003 s)
 Tiempo Total (Wall): 0.3150 ms (0.0003 s)
============================================================
[mpi_filterer] Guardando imagen resultante en: samples/output_mpi_damma.ppm...
[mpi_filterer] Procesamiento distribuido completado exitosamente.
```

---

## 3. Más Ejemplos de Ejecución

### Probar con imagen en escala de grises (`sulfur.pgm`) con filtro Laplace:
```bash
mpirun --allow-run-as-root -np 4 ./mpi_filterer samples/sulfur.pgm samples/output_mpi_sulfur.pgm --f laplace
```

### Probar filtro de Realce (Sharpen) con 2 nodos:
```bash
mpirun --allow-run-as-root -np 2 ./mpi_filterer samples/damma.ppm samples/output_mpi_sharpen.ppm --f sharpen
```

### Aplicar automáticamente los 3 filtros principales (Blur + Laplace + Sharpen):
Si no se pasa la bandera `--f`, el clúster distribuye y aplica los 3 filtros secuencialmente:
```bash
mpirun --allow-run-as-root -np 4 ./mpi_filterer samples/sulfur.pgm samples/output_mpi_all.pgm
```

---

## 4. Arquitectura y Funcionamiento Interno

1. **Descomposición de Dominio en Franjas Horizontales (*Slabs*):**
   - La altura $H$ de la imagen se divide equitativamente entre los $P$ procesos MPI.
   - Cada nodo $k$ recibe un intervalo de filas asignado $[y_{\text{start}}, y_{\text{end}})$.

2. **Manejo de Fronteras con Celdas Fantasma (*Halos / Ghost Rows*):**
   - Para aplicar el kernel de convolución $3 \times 3$, cada worker necesita conocer los píxeles vecinos de las filas adyacentes.
   - El **Nodo 0 (Maestro)** empaqueta cada franja expandida con **1 fila superior y 1 fila inferior de margen (halo)**.
   - Cada nodo calcula la convolución de manera 100% autónoma en su memoria privada.
   - Cada worker devuelve únicamente sus filas útiles calculadas (descartando los halos), asegurando que el ensamble final en el maestro sea continuo y sin costuras ni artefactos visuales.

3. **Memoria Aislada y Estricto Cumplimiento de Restricciones:**
   - **Cero `std::vector`:** Los datos viajan como arreglos planos `unsigned char*` usando los tipos nativos de MPI (`MPI_UNSIGNED_CHAR` y `MPI_BYTE`).
   - **Cero `std::string`:** Toda la gestión de rutas y nombres de filtros se realiza con `const char*` y funciones nativas de C.
   - **Medición aislada:** Cada nodo registra sus propios ciclos de CPU (`std::clock`) y tiempo de reloj real (`Timer`), permitiendo analizar el balanceo de carga entre nodos.

---

## 5. Limpieza al Finalizar

Cuando termines tus pruebas o grabes tu video, puedes apagar el clúster desde PowerShell:
```powershell
docker compose down
```
