#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

struct Dimensoes {
    int rows, cols;
    Dimensoes(int r = 0, int c = 0) : rows{r}, cols{c} {}
};

// Recebe o arquivo com a matriz e armazena ela em uma variavel
void receberMatriz(std::ifstream& file, std::vector<std::vector<int>>& matriz, Dimensoes& dimensao) {
    int rowAtual = 0;
    while(!file.eof()) {
        std::string line;
        std::getline(file, line);

        // Caso a linha esteja vazia, pula.
        if(line == ""){
            continue;
        }

        // Lê a linha e armazena seus dados em uma array
        std::vector<std::string> numbers;
        std::istringstream iss;
        iss.str(line);
        std::string token;
        while (std::getline( iss >> std::ws, token, ' ')) {
            numbers.push_back( token );
        }

        // Define o tamanho de colunas da matriz.
        if(rowAtual == 0) {
            dimensao.cols = numbers.size();
        }

        // Envia a linha para a matriz que é passada como parâmetro
        matriz.push_back(std::vector<int>());
        for(int i = 0; i < dimension.cols; ++i) {
            matriz[rowAtual].push_back(std::atoi(numbers[i].c_str()));
        }
        rowAtual++;
    }
    dimensao.rows = rowAtual;
}

void multiplicarMatrizes(const std::vector<std::vector<int>>& matriz1, Dimension dMatriz1, const std::vector<std::vector<int>>& matriz2, Dimension dMatriz2, std::vector<std::vector<int>>& matrizR) {
    for(int x = 0; x < dMatriz1.rows; x++) {
        matrizR.push_back(std::vector<int>());
        for(int y = 0; y < dMatriz2.cols; y++) {
            int value = 0;
            for(int z = 0; z < dMatriz1.cols; z++) {
                value += matriz1[x][z] * matriz2[z][y];
            }
            matrizR[x].push_back(value);
        }
    }
}

int main (int argc, char** argv) {
    Dimensoes dMatriz1, dMatriz2;
    std::vector<std::vector<int>> matriz1, matriz2, matrizR;

    std::ifstream M1(argv[1]);
    receberMatriz(M1, matriz1, dMatriz1);
    M1.close();

    std::ifstream M2(argv[2]);
    receberMatriz(M2, matriz2, dMatriz2);
    M2.close();

    // Inicia a contagem e faz a multiplicação das matrizes
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    multiplicarMatrizes(matriz1, dMatriz1, matriz2, dMatriz2, matrizR);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    int64_t timeResult = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // Pega os valores e salva em um arquivo
    Dimensoes dMatrizR(dMatriz1.rows, dMatriz2.cols);
    std::ofstream result("MR_sequencial.txt");
    result << dMatrizR.rows << " " << dMatrizR.cols << std::endl;
    for(int i = 0; i < dMatrizR.rows; ++i) {
        for(int j = 0; j < dMatrizR.cols; ++j) {
            result << "c" << i+1 << j+1 << " " << matrizR[i][j] << std::endl;
        }
    }
    result << timeResult << std::endl;
    result.close();

    std::cout << "Tempo: " << timeResult << " ms" << std::endl;
    return EXIT_SUCCESS;
}