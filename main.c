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

struct EquationSystem getSystem(char *filepath) {
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

    // Converts the order from the line into an integer
    int order = atoi(strtrim(line));
    int **matrix = malloc(sizeof(int) * (order + 1));

    for (int i = 0; fgets(line, linesize, file) != NULL; i++) {
        int *values = getValuesInLine(line, order);
        matrix[i] = values;
    }

    matrix[order] = malloc(sizeof(int) * (order + 1));
    for (int i = 0; i <  order; i++) 
        matrix[order][i] = i;

    struct EquationSystem ret = { matrix, order };
    return ret;
}

void intcpy(int *from, int *to, int size) {
    for (int i = 0; i < size; i++)
        to[i] = from[i];
}

int smallest(int *num, int size) {
    int ret = INT_MAX;

    for (int i = 0; i < size; i++) {
        if (ret > num[i])
            ret = num[i];
    }

    return ret;
}

int *getDifFactors(int *num, int size) {
    int *ret = malloc(sizeof(int) * size);
    intcpy(num, ret, size);

    int ret = 1;
    int min = smallest(ret, size);
    for (int n = min; n > 1; n--) {
        char isdiv = 1;
        for (int i = 0; i < size; i++) // TODO: Fix this logic
            if (ret[i] % n != 0)
                isdiv = 0;

        if (!isdiv)
            continue;

        for (int i = 0; i < size; i++)
            ret[i] = ret[i] / n;
    }
    
    return ret;
}

char hasSimilarLines(struct EquationSystem *sys) {
    char isDiff = 0;
    int order = sys->order;
    int **cpy = malloc(sizeof(int) * order);
    for (int i = 0; i < order; i++)
        cpy[i] = getDifFactors(sys->matrix[i], order);

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

char hasBeenReordered(struct EquationSystem *sys) {
    int **mat = sys->matrix;
    int order = sys->order;
    char reordered = 0;

    for (int i = 0; i < order; i ++) {
        if (mat[i][i] != 0)
            continue;

        for (int j = 0; j < order; j++) {
            if (i == j &&
                mat[j][i] == 0 || mat[i][j] == 0)
                continue;

            int *aux = mat[i];
            mat[i] = mat[j];
            mat[j] = aux;
        }
    }

    for (int i = 0; i < order; i ++) {
        if (mat[i][i] != 0)
            continue;

        reordered = 0;
        for (int j = 0; j < order + 1; j++) {
            if (i == j &&
                mat[j][i] == 0 || mat[i][j] == 0)
                continue;

            reordered = 1;
            for (int l = 0; l < order; l++) {
                int aux = mat[l][i];
                mat[l][i] = mat[l][j];
                mat[l][j] = aux;
            }
        }

        if (!reordered)
            break;
    }

    return reordered;
}

char isSolvable(struct EquationSystem *sys) {
    if (hasSimilarLines(sys)) 
        return 0;

    if (!hasBeenReordered(sys))
        return 0;

    return 1;
}

int main() {
    struct EquationSystem sys;
    sys = getSystem(fpath);
    if (!isSolvable(&sys)) {
        printf("System is impossible to solve");
        exit(-1);
    }
    
}