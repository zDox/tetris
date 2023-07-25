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

bool vecsAreEqual(std::vector<std::vector<sf::Color>> vec1, std::vector<std::vector<sf::Color>> vec2){
    if (vec1.size() != vec2.size()) {
        return false; // Vectors have different number of rows
    }

    for (size_t i = 0; i < vec1.size(); ++i) {
        if (vec1[i].size() != vec2[i].size()) {
            return false; // Rows have different number of elements
        }

        for (size_t j = 0; j < vec1[i].size(); ++j) {
            if (vec1[i][j] != vec2[i][j]) {
                return false; // Elements differ at position (i, j)
            }
        }
    }
    return true;
}

std::vector<std::vector<uint32_t>> convertGridToColors(std::vector<std::vector<sf::Color>> grid){
    std::vector<std::vector<uint32_t>> grid_colors;
    for(const auto& row : grid){
        std::vector<uint32_t> row_colors;
        for(const auto& cell : row){
            uint32_t color_uint = (cell.r << 24) | (cell.g << 16) | (cell.b << 8) | cell.a;
            row_colors.push_back(color_uint);
        }
        grid_colors.push_back(row_colors);
    }
    return grid_colors;
}
