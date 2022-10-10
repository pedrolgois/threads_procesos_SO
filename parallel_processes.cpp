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
struct Dimension {
    int rows, cols;
    Dimension(int r = 0, int c = 0) : rows{r}, cols{c} {}
};

std::vector<std::string> split(std::string line, char delimiter) {
    // Store the tokens.
    std::vector<std::string> tokens;

    // Read line
    std::istringstream iss;
    iss.str(line);
    std::string token;
    while (std::getline( iss >> std::ws, token, delimiter)) {
        tokens.push_back( token );
    }
    return tokens;
}

void parseMatrix(std::ifstream& file, std::vector<std::vector<int>>& matrix, Dimension& dimension) {
    int currentRow = 0;
    while(!file.eof()) {
        std::string line;
        std::getline(file, line);
        if(line == "") {
            continue;
        }
        std::vector<std::string> numbers = split(line, ' ');
        if(currentRow == 0) {
            dimension.cols = numbers.size();
        }

        matrix.push_back(std::vector<int>());
        for(int i = 0; i < dimension.cols; ++i) {
            matrix[currentRow].push_back(std::atoi(numbers[i].c_str()));
        }
        currentRow++;
    }
    dimension.rows = currentRow;
}

void saveResult(std::vector<std::vector<int>>& matrixResult, Dimension& dimensionResult, int64_t& timeResult) {
    std::ofstream result("result_sequential.txt");
    for(int i = 0; i < dimensionResult.rows; ++i) {
        for(int j = 0; j < dimensionResult.cols; ++j) {
            result << "c" << i+1 << j+1 << " " << matrixResult[i][j] << std::endl;
        }
    }
    result << timeResult << std::endl;
    result.close();
}

void multiplyMatrixes(const std::vector<std::vector<int>>& matrix1, Dimension dimensionMatrix1, const std::vector<std::vector<int>>& matrix2, Dimension dimensionMatrix2, int p) {
    std::vector<pid_t> pid;

    std::ofstream output;
    std::chrono::steady_clock::time_point begin;
    int elementosCalculados=0;
    for(int i = 0; i < dimensionMatrix1.rows; ++i) {
        for(int j = 0; j < dimensionMatrix2.cols; ++j) {
            //Verificação de ciclo
            if(elementosCalculados%p == 0 || (i == dimensionMatrix1.rows -1 && j == dimensionMatrix2.cols-1)){
                //Criação dos processos filhos
                if((pid.empty() || pid.back() > 0) && !(i == dimensionMatrix1.rows -1 && j == dimensionMatrix2.cols-1)){
                    begin = std::chrono::steady_clock::now();
                    pid_t tmp = fork();
                    pid.push_back(tmp);
                    if(pid.back() == 0){ // inicio processo filho
                        output.open(PASTA +(std::string)("result_processes_")+ std::to_string((elementosCalculados/p)+1) +".txt");
                        output << dimensionMatrix1.rows << " " << dimensionMatrix2.cols << '\n';
                    }
                }else if(pid.back() == 0){/*Processo filho termina*/
                    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                    int64_t timeResult = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                    output << timeResult/*<< " milliseconds"*/ <<std::endl;
                    output.close();
                    exit(0);
                }
                
            }
                if(pid.back() < 0){
                    fprintf(stderr, "Criação de um dos processos falhou");
                    exit(-1);
                }else if(pid.back()==0){
                    // processo filho
                    int value = 0;
                    for(int k = 0; k < dimensionMatrix1.cols; ++k) {
                        value += matrix1[i][k] * matrix2[k][j];
                    }
                    output << "c" << i+1 << j+1 << " " << value << '\n';
                }else{
                    //processo pai
                }
            
            ++elementosCalculados;
        }
    }

    while(wait(NULL) != -1){}
}

int32_t main (int32_t argc, char** argv) {
    Dimension dimensionsMatrix1;
    std::ifstream fileMatrix1(argv[1]);
    std::vector<std::vector<int>> matrix1;
    parseMatrix(fileMatrix1, matrix1, dimensionsMatrix1);
    fileMatrix1.close();

    Dimension dimensionsMatrix2;
    std::ifstream fileMatrix2(argv[2]);
    std::vector<std::vector<int>> matrix2;
    parseMatrix(fileMatrix2, matrix2, dimensionsMatrix2);
    fileMatrix2.close();

    int p = atoi(argv[3]);
    int p_atual=0;

    multiplyMatrixes(matrix1, dimensionsMatrix1,matrix2, dimensionsMatrix2, p);
}