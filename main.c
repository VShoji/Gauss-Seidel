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

int *getvaluesinline(char *str, int order) {
    char *trstr = strtrim(str);
    char *tok = strtok(trstr, " ");
    int *ret = malloc(sizeof(int) * (order + 1));

    for (int i = 0; tok != NULL; i++) {
        ret[i] = atoi(tok);
        tok = strtok(NULL, " ");
    }

    return ret;
}

struct EquationSystem getsystem(char *filepath) {
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
    float **matrix = malloc(sizeof(float) * (order + 1));

    for (int i = 0; fgets(line, linesize, file) != NULL; i++) {
        int *values = getvaluesinline(line, order);
        matrix[i] = values;
    }

    matrix[order] = malloc(sizeof(int) * (order + 1));
    for (int i = 0; i <  order; i++) 
        matrix[order][i] = i;

    struct EquationSystem ret = { matrix, order };
    return ret;
}

void intcpy(int *from, int *to, int size) {
    // TODO: Implement
}

int smallest(int *num, int size) {
    int ret = INT_MAX;

    for (int i = 0; i < size; i++) {
        if (ret > num[i])
            ret = num[i];
    }

    return ret;
}

int mcd(int *num, int size) {
    int *aux = malloc(sizeof(int) * size);
    intcpy(num, aux, size);

    int ret = 1;
    int min = smallest(aux, size);
    for (int n = min; n > 1; n--) {
        char isdiv = 1;
        for (int i = 0; i < size; i++) // TODO: Fix this logic
            if (aux[i] % n != 0)
                isdiv = 0;

        if (!isdiv)
            continue;

        ret = ret * n;
        for (int i = 0; i < size; i++)
            aux[i] = aux[i] / n;
    }
    free(aux);
    return ret;
}

char issolvable(struct EquationSystem *sys) {
    // TODO: implement
}

int main() {
    struct EquationSystem sys;
    sys = getsystem(fpath);

}