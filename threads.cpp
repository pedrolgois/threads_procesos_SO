#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <sstream>
#include <fstream>

#define int long long

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
        for(int i = 0; i < dimensao.cols; ++i) {
            matriz[rowAtual].push_back(std::atoi(numbers[i].c_str()));
        }
        rowAtual++;
    }
    dimensao.rows = rowAtual;
}

void saveResult(std::vector<std::vector<int>>& matrixResult, int initialRow, int initialCol, int P, Dimensoes& dimensionsResult, int64_t& timeResult, std::ofstream& result) {
    result << dimensionsResult.rows << " " << dimensionsResult.cols << std::endl;
    int count = 0;
    for(int i = initialRow; i < dimensionsResult.rows; ++i) {
        if(i != initialRow) {
            initialCol = 0;
        }
        for(int j = initialCol; j < dimensionsResult.cols; ++j) {
            result << "c" << i+1 << j+1 << " " << matrixResult[i][j] << std::endl;
            ++count;
            if(count == P) {
                break;
            }
        }
        if(count == P) {
            break;
        }
    }
    result << timeResult << std::endl;
    result.close();
}

void multiplicarMatrizes(
    const std::vector<std::vector<int>>& matriz1, Dimensoes& dMatriz1, 
    const std::vector<std::vector<int>>& matriz2, Dimensoes& dMatriz2, 
    std::vector<std::vector<int>>& matrizR, 
    std::vector<std::pair<int,int>>& indexes,
    int currentIndex, 
    int P, 
    std::chrono::steady_clock::time_point begin)
    {
    // Inicio
    int initialRow = currentIndex == 0 ? 0 : indexes[currentIndex-1].first;
    int initialCol = currentIndex == 0 ? 0 : indexes[currentIndex-1].second;

    // .
    int count = 0;
    for(int i = initialRow; i < dMatriz1.rows; ++i) {
        if(i != initialRow) {
            initialCol = 0;
        }
        for(int j = initialCol; j < dMatriz1.cols; ++j) {
            int value = 0;
            for(int k = 0; k < dMatriz1.cols; ++k) {
                value += matriz1[i][k] * matriz2[k][j];
            }
            matrizR[i][j] = value;
            count++ ;
            if(count == P) break;
        }
        if(count == P) break;
    }

    // Calculo do tempo percorrido
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    int64_t timeResult = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::ofstream file("resultado_threads/result_threads_" + std::to_string(currentIndex+1) + ".txt");
    Dimensoes dimensionsResult(dMatriz1.rows, dMatriz2.cols);
    initialCol = currentIndex == 0 ? 0 : indexes[currentIndex-1].second;
    saveResult(matrizR, initialRow, initialCol, P, dimensionsResult, timeResult, file);
}

// Função que define cada index com a coluna e row atual
std::vector<std::pair<int, int>> definirIndex(int factor, int P, Dimensoes& dimensionsResult) {
    std::vector<std::pair<int, int>> index;
    int rowAtual = 0,colAtual = 0;
    for(int i = 0; i < factor; ++i) {
        for(int j = 0; j < P; ++j) {
            colAtual++;
            if(colAtual >= dimensionsResult.cols) {
                colAtual = 0;
                rowAtual++;
            }
        }
        index.push_back({rowAtual, colAtual});
    }
    return index;
}

int32_t main (int32_t argc, char** argv) {
    // Recebe os valores passados como parametro e os armazenam em variaveis
    Dimensoes dMatriz1, dMatriz2;
    std::vector<std::vector<int>> matriz1, matriz2, matrizR;

    std::ifstream M1(argv[1]);
    receberMatriz(M1, matriz1, dMatriz1);
    M1.close();

    std::ifstream M2(argv[2]);
    receberMatriz(M2, matriz2, dMatriz2);
    M2.close();

    int P = std::atoi(argv[3]);

    Dimensoes dMatrizR(dMatriz1.rows, dMatriz2.cols);

    // Inicializa a matriz resultado
    for(int k = 0; k < dMatrizR.rows; ++k) {
        matrizR.push_back(std::vector<int>());
        for(int l = 0; l < dMatrizR.cols; ++l) {
            matrizR[k].push_back(0);
        }
    }

    // Calcula o numero de arquivos que serão criados
    int numeroDeArquivos = std::ceil((dMatrizR.rows * dMatrizR.cols) / P);
    int valoresRestantes = (dMatrizR.rows * dMatrizR.cols) - numeroDeArquivos * P;
    std::vector<std::pair<int,int>> index = definirIndex(numeroDeArquivos, P, dMatrizR);
    
    // Cria as threads e em seguida armazena elas
    std::vector<std::thread> threads;
    for(int i = 0; i < numeroDeArquivos; ++i) {
        
        // Caso sobre valores fora da conta de numero de arquivos
        if(i == numeroDeArquivos - 1 && valoresRestantes > 0)  P += valoresRestantes;

        // Inicia o contador
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        threads.emplace_back(
            multiplicarMatrizes, std::ref(matriz1), std::ref(dMatriz1), std::ref(matriz2),
            std::ref(dMatriz2), std::ref(matrizR), std::ref(index), i, P, begin
        );
    }

    for (int j = 0; j < numeroDeArquivos; j++) {
        threads[j].join();
    }
}