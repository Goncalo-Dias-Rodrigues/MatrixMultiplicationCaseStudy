#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>  // For mkdir
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif

// Function to dynamically allocate a matrix of integers
int **allocateMatrix(int rows, int columns) {
    int **matrix = (int **)malloc(rows * sizeof(int *));
    if (matrix == NULL) {
        fprintf(stderr, "Error allocating memory for rows.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int *)malloc(columns * sizeof(int));
        if (matrix[i] == NULL) {
            fprintf(stderr, "Error allocating memory for row %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }
    return matrix;
}

// Function to free the memory allocated for the matrix
void freeMatrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to fill the matrix with random numbers from 1 to 9
void fillMatrix(int **matrix, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            matrix[i][j] = rand() % 9 + 1;
        }
    }
}

// Function to save the matrix in a text file
void saveMatrixToFile(int **matrix, int rows, int columns, const char *filePath) {
    FILE *file = fopen(filePath, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s for writing.\n", filePath);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            fprintf(file, "%d ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int main() {
    int rows, columns;
    char folderPath[] = "C:/Users/Gdrtu/Documents/University/MatrixMultiplicationCaseStudy/MatricesToMultiply/8192";

    printf("Enter the number of rows: ");
    if (scanf("%d", &rows) != 1 || rows <= 0) {
        fprintf(stderr, "Invalid value for rows.\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter the number of columns: ");
    if (scanf("%d", &columns) != 1 || columns <= 0) {
        fprintf(stderr, "Invalid value for columns.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate the two matrices
    int **matrix1 = allocateMatrix(rows, columns);
    int **matrix2 = allocateMatrix(rows, columns);

    // Initialise the random number generator
    srand((unsigned int)time(NULL));

    // Fill the matrices
    fillMatrix(matrix1, rows, columns);
    fillMatrix(matrix2, rows, columns);

    // File paths to save the matrices
    char filePath1[256];
    char filePath2[256];
    snprintf(filePath1, sizeof(filePath1), "%s/matrix1.txt", folderPath);
    snprintf(filePath2, sizeof(filePath2), "%s/matrix2.txt", folderPath);

    // Save the matrices in files
    saveMatrixToFile(matrix1, rows, columns, filePath1);
    saveMatrixToFile(matrix2, rows, columns, filePath2);

    printf("The matrices have been generated and saved in '%s'.\n", folderPath);

    // Free the allocated memory
    freeMatrix(matrix1, rows);
    freeMatrix(matrix2, rows);

    return 0;
}
