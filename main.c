#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define fpath ".\\file.txt"
#define linesize 100

struct EquationSystem {
    int **matrix;
    int order;
};

char *strtrim(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) 
        str++;

    if(*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end) 
    || end > str && (unsigned char)*end == '\n') 
        end--;

    end[1] = '\0';

    return str;
}

int *getValuesInLine(char *str, int order) {
    char *trstr = strtrim(str);
    char *tok = strtok(trstr, " ");
    int *ret = malloc(sizeof(int) * (order + 1));

    for (int i = 0; tok != NULL; i++) {
        ret[i] = atoi(tok);
        tok = strtok(NULL, " ");
    }

    return ret;
}

// Gets a system of linear equations from a text file
void getSystem(char *filepath, struct EquationSystem *ret) {
    // Open file
    FILE *file = fopen(filepath, "r");
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
    int **matrix = malloc(sizeof(int) * (order + 1));

        // Gets the rest of the lines
    for (int i = 0; fgets(line, linesize, file) != NULL; i++) {
            // Separates each value in the string and puts them into a line in the matrix
        int *values = getValuesInLine(line, order);
        matrix[i] = values;
    }

    // The last row contains an index for each column
    matrix[order] = (int*) malloc(sizeof(int) * (order + 1));
    for (int i = 0; i <  order; i++) 
        matrix[order][i] = i;

    // Returns the system of linear equations
    struct EquationSystem aux = { matrix, order };
    *ret = aux;
}


// Copies an array of integers into another
void intcpy(int *to, int *from, int size) {
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
int smallest(int *num, int size) {
    int ret = INT_MAX;

    for (int i = 0; i < size; i++) {
        if (ret > num[i])
            ret = num[i];
    }

    return ret;
}

int *getdiffactors(int *num, int size) {
    int *ret = malloc(sizeof(int) * size);
    intcpy(ret, num, size);

    // Gets lowest absolute value
    int *aux = malloc(sizeof(int) * size);
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
                isdiv = 0;

        if (!isdiv)
            continue;

        // Dividing every element by the number
        for (int i = 0; i < size; i++)
            ret[i] = ret[i] / n;
    }
    
    return ret;
}

char hassimilarlines(struct EquationSystem *sys) {
    char isDiff = 0;
    int order = sys->order;
    int **mat = sys->matrix;
    int **cpy = malloc(sizeof(int) * order);
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
}

void chrow(int **mat, int i, int j) {
    int *aux = mat[i];
    mat[i] = mat[j];
    mat[j] = aux;
}

void chcol(int **mat, int order, int i, int j) {
    for (int l = 0; l < order; l++) {
        int aux = mat[l][i];
        mat[l][i] = mat[l][j];
        mat[l][j] = aux;
    }
}

char rmzerorow(int **mat, int order, int l) {
    int changed = 0;
    for (int i = l + 1; i < order; i++) {
        if (mat[l][i] == 0 || mat[i][l] == 0)
            continue;

        chrow(mat, l, i);
        changed = 1;
        break;
    }
    return changed;
}

char rmzerocol(int **mat, int order, int c) {
    int changed = 0;
    for (int j = c + 1; j < order; j++) {
        if (mat[c][j] == 0 || mat[j][c] == 0)
            continue;

        chcol(mat, order, c, j);
        changed = 1;
        break;
    }
    return changed;
}

char rmzerodia(struct EquationSystem *sys) {
    int **mat = sys->matrix;
    int order = sys->order;
    char reordered = 1;

    for (int r = 0; r < order; r++) {
        if (mat[r][r] != 0)
            continue;

        rmzerorow(mat, order, r);
    }

    for (int c = 0; c < order; c ++) {
        if (mat[c][c] != 0)
            continue;
        
        reordered = rmzerocol(mat, order, c);;
        if (!reordered)
            break;
    }

    return reordered;
}

char issolvable(struct EquationSystem *sys) {
    if (hassimilarlines(sys)) 
        return 0;

    if (!rmzerodia(sys))
        return 0;

    return 1;
}

int greatest(int num1, int num2) {
    if (num1 > num2)
        return num1;

    return num2;
}

void solve(struct EquationSystem sys, float *ret) {
    int **mat = sys.matrix;
    int order = sys.order;
    for (int l = 0; l < order; l++) {
        if (mat[l][l] == 0)
            if (!rmzerorow(mat, order, l))
                if (!rmzerorow(mat, order, l)) {
                    printf("System is impossible to solve\n");
                    exit(-1);
                }

        for (int i = 0; i < order; i++) {
            if (l == i)
                continue;
            
            int coef = mat[l][l];
            int cancel = mat[i][l];
            if (coef > 0 && cancel > 0 ||
                coef < 0 && cancel < 0)
                cancel = -cancel;
            else {
                    coef = abs(coef);
                    cancel = abs(cancel);
            }

            for (int j = 0; j < order + 1; j++) {
                mat[i][j] = mat[i][j] * coef + mat[l][j] * cancel;
            }
        }
    }

    for (int i = 0; i < order; i++) {
        int j = 0;
        while (j < order) {
            if (i == mat[order][j])
                break;

            j++;
        }
        ret[i] = (float) mat[j][order] / mat[j][j];
    }
}

void getresstr(char *output, float *solution, int order) {
    output[0] = '(';
    output[1] = '\0';
    for (int i = 0; i < order; i++) {
        char *append;
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
    float *solution = malloc(sizeof(float) * sys.order);
    solve(sys, solution);

    // Prints the solution
    char output[1001];
    getresstr(output, solution, sys.order);
    printf("Solution: {%s}\n", output);

    exit(0);
} 