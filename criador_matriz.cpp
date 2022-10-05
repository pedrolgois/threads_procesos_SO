#include <iostream>
#include <fstream>
#include <vector>

struct Dimensoes {
    int rows, cols;
    Dimensoes(int r = 0, int c = 0) : rows{r}, cols{c} {}
};

// Preenche os dados da matriz com valores aleatorios
void preencherMatriz(std::ofstream& matrix, Dimensoes d) {
    for(int x = 0; x < d.rows; x++) {
        for(int y = 0; y < d.cols; y++) {
            matrix << std::rand() % 10 + 1 << " ";
        }
        matrix << std::endl;
    }
}

// Cria dois arquivos de matriz com as dimensões passadas
int main (int argc, char** argv) {
    Dimensoes dMatriz1(std::atoi(argv[1]), std::atoi(argv[2]));
    Dimensoes dMatriz2(std::atoi(argv[3]), std::atoi(argv[4]));

    std::ofstream matriz1("M1.txt");
    preencherMatriz(matriz1, dMatriz1);
    matriz1.close();

    std::ofstream matriz2("M2.txt");
    preencherMatriz(matriz2, dMatriz2);
    matriz2.close();
}