#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <sstream>
#include <fstream>
#define int long long
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

void saveResult(std::vector<std::vector<int>>& matrixResult, int initialRow, int initialCol, int P, Dimension& dimensionsResult, int64_t& timeResult, std::ofstream& result) {
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

void multiplyMatrixes(const std::vector<std::vector<int>>& matrix1, Dimension& dimensionsMatrix1, 
    const std::vector<std::vector<int>>& matrix2, Dimension& dimensionsMatrix2, 
    std::vector<std::vector<int>>& result, std::vector<std::pair<int,int>>& indexes,
    int currentIndex, int P, std::chrono::steady_clock::time_point begin) {

    int count = 0;
    int initialRow = currentIndex == 0 ? 0 : indexes[currentIndex-1].first;
    int initialCol = currentIndex == 0 ? 0 : indexes[currentIndex-1].second;

    for(int i = initialRow; i < dimensionsMatrix1.rows; ++i) {
        if(i != initialRow) {
            initialCol = 0;
        }
        for(int j = initialCol; j < dimensionsMatrix1.cols; ++j) {
            int value = 0;
            for(int k = 0; k < dimensionsMatrix1.cols; ++k) {
                value += matrix1[i][k] * matrix2[k][j];
            }
            result[i][j] = value;
            ++count;
            if(count == P) {
                break;
            }
        }
        if(count == P) {
            break;
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    int64_t timeResult = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    // std::cout << "Time: " << timeResult << " ms" << std::endl;

    std::ofstream file("results_parallel_threads/result_threads_" + std::to_string(currentIndex+1) + ".txt");
    Dimension dimensionsResult(dimensionsMatrix1.rows, dimensionsMatrix2.cols);
    initialCol = currentIndex == 0 ? 0 : indexes[currentIndex-1].second;
    saveResult(result, initialRow, initialCol, P, dimensionsResult, timeResult, file);
}

std::vector<std::pair<int, int>> resolveIndex(int factor, int P, Dimension& dimensionsResult) {
    std::vector<std::pair<int, int>> indexes;
    int currentRow = 0;
    int currentCol = 0;
    for(int i = 0; i < factor; ++i) {
        for(int j = 0; j < P; ++j) {
            ++currentCol;
            if(currentCol >= dimensionsResult.cols) {
                currentCol = 0;
                ++currentRow;
            }
        }
        indexes.push_back({currentRow, currentCol});
    }
    return indexes;
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

    std::vector<std::vector<int>> result;
    Dimension dimensionsResult(dimensionsMatrix1.rows, dimensionsMatrix2.cols);
    
    // Create an matrix filled with zeros
    for(int k = 0; k < dimensionsResult.rows; ++k) {
        result.push_back(std::vector<int>());
        for(int l = 0; l < dimensionsResult.cols; ++l) {
            result[k].push_back(0);
        }
    }


    int P = std::atoi(argv[3]);
    // Calculate ceil of n1*m2/P
    int factor = std::ceil((dimensionsResult.rows * dimensionsResult.cols) / P);
    int diffFactorPerTotalElements = (dimensionsResult.rows * dimensionsResult.cols) - factor * P;

    std::vector<std::pair<int,int>> indexes = resolveIndex(factor, P, dimensionsResult);
    
    // Creating threads dynamically
    std::vector<std::thread> threads;
    for(int i = 0; i < factor; ++i) {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        
        // Fix diff in factor when (P * factor) is not equals to total elements in matrix.
        if(i == factor - 1 && diffFactorPerTotalElements > 0)  {
            P += diffFactorPerTotalElements;
        }

        threads.emplace_back(
            multiplyMatrixes, std::ref(matrix1), std::ref(dimensionsMatrix1), std::ref(matrix2),
            std::ref(dimensionsMatrix2), std::ref(result), std::ref(indexes), i, P, begin
        );
    }
    for (int j = 0; j < factor; j++) {
        threads[j].join();
    }
}