#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

// ==========================================
// FILTROS
// ==========================================

vector<vector<double>> suavizado = {
    {1.0/9, 1.0/9, 1.0/9},
    {1.0/9, 1.0/9, 1.0/9},
    {1.0/9, 1.0/9, 1.0/9},
};

vector<vector<double>> desenfoque = {
    {1.0/16, 2.0/16, 1.0/16},
    {2.0/16, 4.0/16, 2.0/16},
    {1.0/16, 2.0/16, 1.0/16}
};

vector<vector<double>> realce = {
    {0, -1, 0},
    {-1, 5, -1},
    {0, -1, 0},
};

vector<vector<double>> deteccion_bordes = {
    {-1, -1, -1},
    {-1, 8, -1},
    {-1, -1, -1},
};

vector<vector<double>> sobel_horizontal = {
    {-1, -2, -1},
    {0, 0, 0},
    {1, 2, 1},
};


// ==========================================
// ESTRUCTURA PARA UN PIXEL RGB
// ==========================================

struct Pixel {
    int r;
    int g;
    int b;
};


// ==========================================
// LIMITAR VALOR ENTRE 0 Y 255
// ==========================================

int limitar(int valor) {

    if (valor < 0) {
        return 0;
    }

    if (valor > 255) {
        return 255;
    }

    return valor;
}


int main() {

    string nombreArchivo = "monito.ppm";
    string nombreSalida = "monito_filtrado.ppm";

    // ==========================================
    // ABRIR IMAGEN
    // ==========================================

    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
        return 1;
    }


    // ==========================================
    // LEER ENCABEZADO
    // ==========================================

    string tipo;
    int columnas, filas;
    int maximo;

    archivo >> tipo;

    if (tipo != "P3") {
        cout << "El archivo no es un PPM P3." << endl;
        return 1;
    }


    // ==========================================
    // LEER DIMENSIONES Y VALOR MAXIMO
    // ==========================================
    //archivo.ignore(1);
    archivo >> columnas >> filas;
    archivo >> maximo;


    cout << "Imagen leida correctamente." << endl;
    cout << "Columnas: " << columnas << endl;
    cout << "Filas: " << filas << endl;
    cout << "Valor maximo: " << maximo << endl;


    // ==========================================
    // CREAR MATRIZ DE PIXELES
    // ==========================================

    vector<vector<Pixel>> matriz(
        filas,
        vector<Pixel>(columnas)
    );


    // ==========================================
    // LEER LOS PIXELES
    // ==========================================

    for (int i = 0; i < filas; i++) {

        for (int j = 0; j < columnas; j++) {
            
            archivo >> matriz[i][j].r;
            archivo >> matriz[i][j].g;
            archivo >> matriz[i][j].b;
        }
    }


    archivo.close();


    // ==========================================
    // CREAR MATRIZ PARA LA IMAGEN FILTRADA
    // ==========================================

    vector<vector<Pixel>> filtrada(
        filas,
        vector<Pixel>(columnas)
    );


    // ==========================================
    // POSICIONES DEL FILTRO 3x3
    // ==========================================

    vector<int> pos = {-1, 0, 1};


    // ==========================================
    // APLICAR FILTRO PROMEDIO 3x3
    // ==========================================

    for (int x = 0; x < filas; x++) {

        for (int y = 0; y < columnas; y++) {

            double nuevoR = 0;
            double nuevoG = 0;
            double nuevoB = 0;


            // Recorrer el filtro 3x3
            for (int i = 0; i < 3; i++) {

                for (int j = 0; j < 3; j++) {

                    int nuevX = x + pos[i];
                    int nuevY = y + pos[j];


                    // Verificar que el vecino esté dentro de la imagen
                    if (nuevX >= 0 && nuevX < filas &&
                        nuevY >= 0 && nuevY < columnas) {


                        // Canal rojo
                        nuevoR +=
                            deteccion_bordes[i][j] *
                            matriz[nuevX][nuevY].r;


                        // Canal verde
                        nuevoG +=
                            deteccion_bordes[i][j] *
                            matriz[nuevX][nuevY].g;


                        // Canal azul
                        nuevoB +=
                            deteccion_bordes[i][j] *
                            matriz[nuevX][nuevY].b;
                    }
                }
            }


            // ==========================================
            // GUARDAR RESULTADO
            // ==========================================

            filtrada[x][y].r = limitar(round(nuevoR));
            filtrada[x][y].g = limitar(round(nuevoG));
            filtrada[x][y].b = limitar(round(nuevoB));
        }
    }


    // ==========================================
    // CREAR NUEVO ARCHIVO PPM
    // ==========================================

    ofstream salida(nombreSalida);

    if (!salida.is_open()) {

        cout << "No se pudo crear la imagen de salida." << endl;
        return 1;
    }


    // ==========================================
    // ESCRIBIR ENCABEZADO PPM
    // ==========================================

    salida << "P3\n";
    salida << columnas << " " << filas << "\n";
    salida << maximo << "\n";


    // ==========================================
    // ESCRIBIR PIXELES
    // ==========================================

    for (int i = 0; i < filas; i++) {

        for (int j = 0; j < columnas; j++) {

            salida << filtrada[i][j].r << " "
                   << filtrada[i][j].g << " "
                   << filtrada[i][j].b << "\n";
        }
    }


    salida.close();


    cout << "Imagen filtrada creada: "
         << nombreSalida << endl;


    return 0;
}