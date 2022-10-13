#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define fpath ".\\file.txt"

struct EquationSystem {
    float **matrix;
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

float *getvaluesinline(char *str, int order) {
    char *trstr = strtrim(str);
    char *tok = strtok(trstr, " ");
    float *ret = malloc(sizeof(float) * (order + 1));

    for (int i = 0; i < order; i++) {
        ret[i] = atof(tok);
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
    char line[100];
        // Checks if the file is empty
    if (fgets(line, 100, file) == NULL) {
        printf("File is empty");
        exit(-1);
    }

    // Converts the order from the line into an integer
    int order = atoi(strtrim(line));
    float **matrix = malloc(sizeof(float) * (order + 1));

    for (int i = 0; fgets(line, 100, file) != NULL; i++) {
        float *values = getvaluesinline(line, order);
        matrix[i] = values;
    }

    for (int i = 0; i <  order; i++) 
        matrix[order + 1][i] = i;

    struct EquationSystem ret = { matrix, order };
    return ret;
}

int main() {
    struct EquationSystem sys;
    sys = getsystem(fpath);
}