#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

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

void multiplyMatrixes(const std::vector<std::vector<int>>& matrix1, Dimension dimensionMatrix1, const std::vector<std::vector<int>>& matrix2, Dimension dimensionMatrix2, std::vector<std::vector<int>>& result) {
    for(int i = 0; i < dimensionMatrix1.rows; ++i) {
        result.push_back(std::vector<int>());
        for(int j = 0; j < dimensionMatrix2.cols; ++j) {
            int value = 0;
            for(int k = 0; k < dimensionMatrix1.cols; ++k) {
                value += matrix1[i][k] * matrix2[k][j];
            }
            result[i].push_back(value);
        }
    }
}

void saveResult(std::vector<std::vector<int>>& matrixResult, Dimension& dimensionResult, int64_t& timeResult) {
    std::ofstream result("result_sequential.txt");
    result << dimensionResult.rows << " " << dimensionResult.cols << std::endl;
    for(int i = 0; i < dimensionResult.rows; ++i) {
        for(int j = 0; j < dimensionResult.cols; ++j) {
            result << "c" << i+1 << j+1 << " " << matrixResult[i][j] << std::endl;
        }
    }
    result << timeResult << std::endl;
    result.close();
}

int main (int argc, char** argv) {
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

    std::vector<std::vector<int>> result;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    multiplyMatrixes(matrix1, dimensionsMatrix1, matrix2, dimensionsMatrix2, result);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    int64_t timeResult = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    Dimension dimensionResult(dimensionsMatrix1.rows, dimensionsMatrix2.cols);
    saveResult(result, dimensionResult, timeResult);
    std::cout << "Time: " << timeResult << " ms" << std::endl;
    return EXIT_SUCCESS;
}