#include <stdio.h>
#include "../include/includes.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Uso: %s <ruta/archivo.txt>", argv[0]);
        return 1;
    }

    const char *file = argv[1];

    // read test file
    read_test_file(file);

    return 0;
}
