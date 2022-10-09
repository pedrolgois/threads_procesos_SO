#include <iostream>
#include <fstream>
#include <vector>

struct Dimension {
    int rows, cols;
    Dimension(int r = 0, int c = 0) : rows{r}, cols{c} {}
};

void fillMatrix(std::ofstream& matrix, Dimension dimension) {
    for(int i = 0; i < dimension.rows; ++i) {
        for(int j = 0; j < dimension.cols; ++j) {
            matrix << std::rand() % 10 + 1 << " ";
        }
        matrix << std::endl;
    }
}

int main (int argc, char** argv) {
    Dimension dimensionsMatrix1(std::atoi(argv[1]), std::atoi(argv[2]));
    Dimension dimensionsMatrix2(std::atoi(argv[3]), std::atoi(argv[4]));

    std::ofstream matrix1("matrix1.txt");
    std::ofstream matrix2("matrix2.txt");

    fillMatrix(matrix1, dimensionsMatrix1);
    fillMatrix(matrix2, dimensionsMatrix2);

    matrix1.close();
    matrix2.close();

}