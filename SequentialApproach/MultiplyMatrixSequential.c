#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif

// Function to dynamically read a line of any length from a file.
// The returned string must be freed by the caller.
char *readLine(FILE *file) {
    int capacity = 128;
    int length = 0;
    char *buffer = malloc(capacity);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating memory for line buffer.\n");
        exit(EXIT_FAILURE);
    }
    int c;
    while ((c = fgetc(file)) != EOF) {
        // If we hit a newline, break out of the loop.
        if (c == '\n')
            break;
        // Resize the buffer if necessary.
        if (length + 1 >= capacity) {
            capacity *= 2;
            char *temp = realloc(buffer, capacity);
            if (temp == NULL) {
                free(buffer);
                fprintf(stderr, "Error reallocating memory for line buffer.\n");
                exit(EXIT_FAILURE);
            }
            buffer = temp;
        }
        buffer[length++] = (char)c;
    }
    // If no characters were read and EOF is reached, free and return NULL.
    if (length == 0 && c == EOF) {
        free(buffer);
        return NULL;
    }
    buffer[length] = '\0';
    return buffer;
}

// Function to dynamically allocate a matrix of integers.
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

// Function to free the memory allocated for a matrix.
void freeMatrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to read a matrix from a file using dynamic line reading.
// It determines the number of rows and columns by reading the file line by line.
int **readMatrixFromFile(const char *filePath, int *rows, int *columns) {
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s for reading.\n", filePath);
        exit(EXIT_FAILURE);
    }
    
    *rows = 0;
    *columns = 0;
    char *line = NULL;

    // First pass: determine the dimensions.
    while ((line = readLine(file)) != NULL) {
        // Skip empty lines.
        if (strlen(line) == 0) {
            free(line);
            continue;
        }
        (*rows)++;
        // For the first non-empty line, count the numbers (columns).
        if (*rows == 1) {
            char *copy = strdup(line);
            if (copy == NULL) {
                fprintf(stderr, "Error duplicating line string.\n");
                exit(EXIT_FAILURE);
            }
            char *token = strtok(copy, " \t");
            while (token != NULL) {
                (*columns)++;
                token = strtok(NULL, " \t");
            }
            free(copy);
        }
        free(line);
    }

    // Allocate the matrix.
    int **matrix = allocateMatrix(*rows, *columns);

    // Second pass: fill the matrix.
    rewind(file);
    int r = 0;
    while ((line = readLine(file)) != NULL && r < *rows) {
        if (strlen(line) == 0) {
            free(line);
            continue;
        }
        int c = 0;
        char *token = strtok(line, " \t");
        while (token != NULL && c < *columns) {
            matrix[r][c] = atoi(token);
            token = strtok(NULL, " \t");
            c++;
        }
        r++;
        free(line);
    }
    fclose(file);
    return matrix;
}

// Function to multiply two matrices.
// Matrix A is of dimension rowsA x colsA and B is of dimension colsA x colsB.
// The result matrix must be preallocated with dimensions rowsA x colsB.
void multiplyMatrices(int **A, int **B, int **result, int rowsA, int colsA, int colsB) {
    // Initialise the result matrix to zero.
    for (int i = 0; i < rowsA; i++)
        for (int j = 0; j < colsB; j++)
            result[i][j] = 0;

    // Best sequential implementation: loop ordering i, k, j for improved cache usage.
    for (int i = 0; i < rowsA; i++) {
        for (int k = 0; k < colsA; k++) {
            for (int j = 0; j < colsB; j++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Function to save a matrix to a text file.
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
    // Start timing the entire programme.
    clock_t startTime = clock();

    // Define input file paths (ensure the backslashes are escaped).
    const char *inputFolder = "C:\\Users\\Gdrtu\\Documents\\University\\MatrixMultiplicationCaseStudy\\MatricesToMultiply\\512";
    char inputFilePath1[512], inputFilePath2[512];
    snprintf(inputFilePath1, sizeof(inputFilePath1), "%s\\matrix1.txt", inputFolder);
    snprintf(inputFilePath2, sizeof(inputFilePath2), "%s\\matrix2.txt", inputFolder);

    // Read the two matrices from files.
    int rows1, cols1;
    int **matrix1 = readMatrixFromFile(inputFilePath1, &rows1, &cols1);

    int rows2, cols2;
    int **matrix2 = readMatrixFromFile(inputFilePath2, &rows2, &cols2);

    // Check if the matrices can be multiplied.
    if (cols1 != rows2) {
        fprintf(stderr, "Matrix dimensions are not compatible for multiplication (matrix1: %dx%d, matrix2: %dx%d).\n",
                rows1, cols1, rows2, cols2);
        freeMatrix(matrix1, rows1);
        freeMatrix(matrix2, rows2);
        exit(EXIT_FAILURE);
    }

    // Allocate the result matrix with dimensions rows1 x cols2.
    int **resultMatrix = allocateMatrix(rows1, cols2);

    // Multiply the matrices.
    multiplyMatrices(matrix1, matrix2, resultMatrix, rows1, cols1, cols2);

    // Define output folder and file path (create the folder if necessary).
    const char *outputFolder = "C:\\Users\\Gdrtu\\Documents\\University\\MatrixMultiplicationCaseStudy\\ResultMatrix";
    if (MKDIR(outputFolder) != 0) {
        if (errno != EEXIST) {
            perror("Error creating output directory");
            exit(EXIT_FAILURE);
        }
    }
    char outputFilePath[512];
    snprintf(outputFilePath, sizeof(outputFilePath), "%s\\result.txt", outputFolder);

    // Save the result matrix to file.
    saveMatrixToFile(resultMatrix, rows1, cols2, outputFilePath);
    printf("The resulting matrix has been saved to '%s'.\n", outputFilePath);

    // Free allocated memory.
    freeMatrix(matrix1, rows1);
    freeMatrix(matrix2, rows2);
    freeMatrix(resultMatrix, rows1);

    // End timing the entire programme.
    clock_t endTime = clock();
    double totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    printf("Total programme execution time: %.6f seconds.\n", totalTime);

    return 0;
}
