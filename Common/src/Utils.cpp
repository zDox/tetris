#include "Utils.hpp"

void transpose(std::vector<std::vector<bool>> &matrix){
    std::vector<std::vector<bool>> temp;
    
    // Resize temp
    temp.resize(matrix.size());
    for (std::vector<std::vector<bool>>::size_type i=0; i < matrix.size(); i++){
        temp[i].resize(matrix.size());
    }
    // Transpose
    for(std::vector<std::vector<bool>>::size_type j=0; j<matrix.size();j++){
        for(std::vector<bool>::size_type k=0; k<matrix[0].size(); k++){
            temp[k][j] = matrix[j][k];
        }
    }
    matrix = temp;
}

void reverse_rows(std::vector<std::vector<bool>> &matrix){
    // Reverse every row
    for(std::vector<std::vector<bool>>::size_type i=0; i<matrix.size();i++){
        std::reverse(matrix[i].begin(), matrix[i].end());
    }
}
