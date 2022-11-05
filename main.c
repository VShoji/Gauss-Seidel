#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define fpath ".\\file.txt"
#define linesize 100

struct EquationSystem {
    int** matrix;
    int order;
};

char* strtrim(char* str) {
    char* end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)
        || end > str && (unsigned char)*end == '\n')
        end--;

    end[1] = '\0';

    return str;
}

// Separates each value in a string and stores it into an array
int* getValuesInLine(char* str, int order) {
    char* trstr = strtrim(str);
    char* tok = strtok(trstr, " ");
    int* ret = malloc(sizeof(int) * (order + 1));

    for (int i = 0; tok != NULL; i++) {
        ret[i] = atoi(tok);
        tok = strtok(NULL, " ");
    }

    return ret;
}

// Gets a system of linear equations from a text file
void getSystem(char* filepath, struct EquationSystem* ret) {
    // Open file
    FILE* file = fopen(filepath, "r");
    // Checks if file has been opened succesfully
    if (file == NULL) {
        printf("Could not open file");
        exit(-1);
    }

    // Gets the order of the matrix located in the first line of the file
    char line[linesize];
    // Checks if the file is empty
    if (fgets(line, linesize, file) == NULL) {
        printf("File is empty");
        exit(-1);
    }

    // Gets the order of the matrix from the file
    int order = atoi(strtrim(line));
    int** matrix = malloc(sizeof(int) * (order + 1));

    // Gets the rest of the lines
    for (int i = 0; fgets(line, linesize, file) != NULL; i++) {
        // Separates each value in the string and puts them into a line in the matrix
        int* values = getValuesInLine(line, order);
        matrix[i] = values;
    }

    // The last row contains an index for each column
    matrix[order] = (int*)malloc(sizeof(int) * (order + 1));
    for (int i = 0; i < order; i++)
        matrix[order][i] = i;

    // Returns the system of linear equations
    struct EquationSystem aux = { matrix, order };
    *ret = aux;
}


// Copies an array of integers into another
void intcpy(int* to, int* from, int size) {
    for (int i = 0; i < size; i++) {
        to[i] = from[i];
    }
}

int abs(int n) {
    if (n > 0)
        return n;

    return -n;
}

// Gets the lowest value in an array
/*int smallest(int* num, int size) {
    int ret = INT_MAX;

    for (int i = 0; i < size; i++) {
        if (ret > num[i])
            ret = num[i];
    }

    return ret;
}

int* getdiffactors(int* num, int size) {
    int* ret = malloc(sizeof(int) * size);
    intcpy(ret, num, size);

    // Gets lowest absolute value
    int* aux = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++)
        aux[i] = abs(ret[i]);
    int min = smallest(aux, size);
    free(aux);

    // Divides every element by the maximum common divisor
    for (int n = min; n > 1; n--) {
        char isdiv = 1;
        // Checking if every element is dividable by the number
        for (int i = 0; i < size; i++)
            if (ret[i] % n != 0)
            {
                isdiv = 0;
                break;
            }

        if (!isdiv)
            continue;

        // Dividing every element by the number
        for (int i = 0; i < size; i++)
            ret[i] = ret[i] / n;
    }

    return ret;
}

// Checks if any lines are similar
char hassimilarlines(struct EquationSystem* sys) {
    char isDiff = 0;
    int order = sys->order;
    int** mat = sys->matrix;
    int** cpy = malloc(sizeof(int) * order);

    for (int i = 0; i < order; i++)
        cpy[i] = getdiffactors(mat[i], order);

    for (int c = 0; c < order - 1; c++) {
        for (int i = c + 1; i < order; i++) {
            isDiff = 0;
            for (int j = 0; j < order; j++)
                if (cpy[c][j] != cpy[i][j]) {
                    isDiff = 1;
                    break;
                }

            if (!isDiff)
                break;
        }

        if (!isDiff)
            break;
    }

    for (int i = 0; i < order; i++)
        free(cpy[i]);
    free(cpy);

    return !isDiff;
}*/

// Checks if any lines are similar
char hasSimilarLines(struct EquationSystem* sys) {
    int order = sys->order;
    int** mat = sys->matrix;

    // Iterates through all of the lines
    for (int curr = 0; curr < order - 1; curr++)
        // Gets all other lines
        for (int other = curr + 1; other < order; other++) {
            char isSim = 1;
            float firstProp = (float) mat[curr][0] / mat[other][0]; // First proportion for comparisons
            
            // Checks if all proportions are the same, which would indicates that the equations are equivalent
            for (int col = 1; col < order; col++) {
                int currCoef = mat[curr][col];
                int otherCoef = mat[other][col];
                float prop = (float) currCoef / otherCoef;

                if (firstProp != prop) {
                    isSim = 0;
                    break;
                }
            }

            // If two lines are similar, return true
            if (isSim)
                return 1;
        }

    return 0;
}

//Change row, change the order of rows 
void chrow(int** mat, int i, int j) {
    // Changes array pointers
    int* aux = mat[i];
    mat[i] = mat[j];
    mat[j] = aux;
}

//Change column change the order of columns
void chcol(int** mat, int order, int i, int j) {
    // Changes each element in the columns individually
    for (int l = 0; l < order; l++) {
        int aux = mat[l][i];
        mat[l][i] = mat[l][j];
        mat[l][j] = aux;
    }
}

//Remove zero from row
char rmzerorow(int** mat, int order, int r) {
    int changed = 0;

    // Checks if there are any rows available for swapping
    for (int i = 0; i < order; i++) {
        if (mat[r][i] == 0 || mat[i][r] == 0)
            continue;

        // Changes row if found
        chrow(mat, r, i);
        changed = 1;
        break;
    }
    return changed;
}

//Remove zero from column
char rmzerocol(int** mat, int order, int c) {
    int changed = 0;

    // Checks if there are any rows available for swapping
    for (int j = 0; j < order; j++) {
        if (mat[c][j] == 0 || mat[j][c] == 0)
            continue;

        // Changes column if found
        chcol(mat, order, c, j);
        changed = 1;
        break;
    }
    return changed;
}

//Remove zero from the principal diagonal
char rmzerodia(struct EquationSystem* sys) {
    int** mat = sys->matrix;
    int order = sys->order;
    char reordered = 1;

    // Checks if there are any zeros in the main diagonal and
    // attempts to remove them by swapping rows
    for (int r = 0; r < order; r++) {
        if (mat[r][r] != 0)
            continue;

        if (!rmzerorow(mat, order, r))
            reordered = 0;
    }

    // Checks if any zeros remain
    if (reordered)
        return reordered;

    // Checks if there are any zeros in the main diagonal and
    // attempts to remove them by swapping columns
    for (int c = 0; c < order; c++) {
        if (mat[c][c] != 0)
            continue;

        if (!rmzerocol(mat, order, c))
            break;
    }

    return reordered;
}

// Checks if the linear equations are solvable
char issolvable(struct EquationSystem* sys) {
    // Checks if there are any zeros in the main diagonal and removes them if there are any
    if (!rmzerodia(sys)) {
        printf("0 não removido\n");
        return 0;
    }

    // Checks if any equations are equivalent
    if (hasSimilarLines(sys)) {
        printf("Similar lines\n");
        return 0;
    }

    return 1;
}

void solve(struct EquationSystem sys, float* ret) {
    int** mat = sys.matrix;
    int order = sys.order;

    for (int curr = 0; curr < order; curr++) {

        // Checks if any zeros were introduced by the solving algorithm
        if (mat[curr][curr] == 0)
            if (!rmzerorow(mat, order, curr))
                if (!rmzerorow(mat, order, curr)) {
                    printf("System impossible to solve while solving\n");
                    exit(-1);
                }

        // Canceling every other coeficient on other lines
        for (int other = 0; other < order; other++) {
            if (curr == other)
                continue;

            // Calculates factors to cancel the coeficient
            int currCoef = mat[curr][curr];
            int otherCoef = mat[other][curr];
            if (currCoef > 0 && otherCoef > 0 ||
                currCoef < 0 && otherCoef < 0)
                otherCoef = -otherCoef;
            else {
                currCoef = abs(currCoef);
                otherCoef = abs(otherCoef);
            }

            // Cancels the coeficient
            for (int c = 0; c < order + 1; c++) {
                mat[other][c] = mat[other][c] * currCoef + mat[curr][c] * otherCoef;
            }
        }
    }

    // Retrieves every solution in order
    for (int curr = 0; curr < order; curr++) {
        int idx = 0;
        while (idx < order) {
            if (curr == mat[order][idx])
                break;

            idx++;
        }
        ret[curr] = (float)mat[idx][order] / mat[idx][idx];
    }
}

// Formats the result into a readable string
void getresstr(char* output, float* solution, int order) {
    output[0] = '(';
    output[1] = '\0';
    for (int i = 0; i < order; i++) {
        char append[1001];
        gcvt(solution[i], 4, append);
        strcat(output, append);
        if (i != order - 1)
            strcat(output, ", ");
    }
    strcat(output, ")");
}

int main() {
    // Gets the system of linear equations from the text file
    struct EquationSystem sys;
    getSystem(fpath, &sys);

    // Checks if the system is solvable
    if (!issolvable(&sys)) {
        printf("System is impossible to solve\n");
        exit(-1);
    }

    // Attempts to solve the system
    float* solution = malloc(sizeof(float) * sys.order);
    solve(sys, solution);

    // Prints the solution
    char output[1001];
    getresstr(output, solution, sys.order);
    printf("Solution: {%s}\n", output);

    // Require enter to exit
    //char ignore[101];
    //scanf("%s", &ignore);
    exit(0);
}
