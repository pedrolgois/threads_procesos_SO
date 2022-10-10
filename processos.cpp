#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#define int long long

#define PASTA "resultado_processos/"

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

void multiplicarMatrizes(const std::vector<std::vector<int>>& matriz1, Dimensoes dMatriz1, const std::vector<std::vector<int>>& matriz2, Dimensoes dMatriz2, int p) {
    std::vector<pid_t> pid;
    std::ofstream arquivo;
    std::chrono::steady_clock::time_point begin;
    int elementosCalculados = 0;

    // Passa por cada elemento da matriz
    for(int i = 0; i < dMatriz1.rows; ++i) {
        for(int j = 0; j < dMatriz2.cols; ++j) {

            // A cada P elementos calculados, cria um arquivo
            if(elementosCalculados % p == 0 || (i == dMatriz1.rows -1 && j == dMatriz2.cols-1)){
                
                // Cria os processos filhos e inicia a contagem
                if((pid.empty() || pid.back() > 0) && !(i == dMatriz1.rows -1 && j == dMatriz2.cols-1)){
                    begin = std::chrono::steady_clock::now();
                    pid_t tmp = fork();
                    pid.push_back(tmp);
                    if(pid.back() == 0){ // inicio processo filho
                        arquivo.open(PASTA +(std::string)("resultado_processo_")+ std::to_string((elementosCalculados / p)+1) +".txt");
                        arquivo << dMatriz1.rows << " " << dMatriz2.cols << '\n';
                    }
                }
                // Finaliza o processo e faz calculo do tempo
                else if(pid.back() == 0){/*Processo filho termina*/
                    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                    int64_t timeResult = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                    arquivo << timeResult/*<< " milliseconds"*/ <<std::endl;
                    arquivo.close();
                    exit(0);
                }
                
            }
                if(pid.back() < 0){
                    fprintf(stderr, "Criação de um dos processos falhou");
                    exit(-1);
                }else if(pid.back() == 0){
                    // processo filho
                    int value = 0;
                    for(int k = 0; k < dMatriz1.cols; ++k) {
                        value += matriz1[i][k] * matriz2[k][j];
                    }
                    arquivo << "c" << i+1 << j+1 << " " << value << '\n';
                }else{
                    //processo pai
                }
            
            ++elementosCalculados;
        }
    }

    while(wait(NULL) != -1){}
}

int32_t main (int32_t argc, char** argv) {
    Dimensoes dMatriz1, dMatriz2;
    std::vector<std::vector<int>> matriz1, matriz2;

    std::ifstream M1(argv[1]);
    receberMatriz(M1, matriz1, dMatriz1);
    M1.close();

    std::ifstream M2(argv[2]);
    receberMatriz(M2, matriz2, dMatriz2);
    M2.close();

    int p = atoi(argv[3]);
    int p_atual=0;

    multiplicarMatrizes(matriz1, dMatriz1,matriz2, dMatriz2, p);
}