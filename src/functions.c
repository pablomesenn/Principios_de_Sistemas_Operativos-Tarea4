#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/includes.h"

unsigned char data_blocks[NUM_BLOCKS][BLOCK_SIZE];
int blocks_used[NUM_BLOCKS]; // 0 = free, 1 = not free

FileEntry file_table[MAX_FILES];

void create_file(const char *filename, int size){

        // 1. Validate filename
        if (filename == NULL || strlen(filename) == 0) {
           printf("Error: nombre de archivo inválido.\n");
           return;
        }
        if (strlen(filename) >= MAX_FILENAME) {
           printf("Error: nombre de archivo demasiado largo.\n");
           return;
        }

        // 2. Verify filename is not already in use
        if (is_fname_used(filename) != -1) {
            printf("Error: ya existe un archivo con el nombre '%s'.\n", filename);
            return;
        }

        // 3. Search for a free file entry
        int file_index = find_free_file_entry();
        if (file_index == -1) {
            printf("Error: número máximo de archivos alcanzado.\n");
            return;
        }

        int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

        if (blocks_needed > MAX_BLOCKS_PER_FILE) {
            printf("Error: el archivo requiere más bloques de los permitidos por archivo.\n");
            return;
        }

        // 5. Check if there are enough free blocks
        int free_blocks = count_free_blocks();
        if (free_blocks < blocks_needed) {
            printf("Error: no hay espacio suficiente en el sistema de archivos.\n");
            return;
        }

        // 6. Allocate blocks and save their indices
        if (allocate_blocks(blocks_needed, file_table[file_index].blocks) != 0) {
            printf("Error: fallo al asignar bloques.\n");
            return;
        }

        // 7. Initialize the file entry
        memset(file_table[file_index].name, 0, MAX_FILENAME);
        strcpy(file_table[file_index].name, filename);

        file_table[file_index].size = 0;          // havent written anything yet
        file_table[file_index].max_size = size;
        file_table[file_index].used = 1;
        file_table[file_index].num_blocks = blocks_needed;
}

void write_file(const char *filename, int offset, const char *data){

    int file_index = is_fname_used(filename);

    // 1. Verify if the file exists
    if (file_index == -1) {
        printf("Error: el archivo '%s' no existe.\n", filename);
        return;
    }

    FileEntry *f = &file_table[file_index];
    int data_len = strlen(data);

    // 2. Verify if there is enough space in the file
    if ((offset + data_len) > f->max_size){
        printf("Error: el archivo '%s' no tiene suficiente espacio.\n", filename);
        return;
    }

    // 3. Write data to the file
    for (int i = 0; i < data_len; i++) {
            int global_pos      = offset + i;              // position inside the file
            int block_index     = global_pos / BLOCK_SIZE; // logic index of the block
            int internal_offset = global_pos % BLOCK_SIZE; // position inside the block

            int real_block = f->blocks[block_index];       // physical block in data_blocks

            data_blocks[real_block][internal_offset] = (unsigned char)data[i];
        }

    // 4. Update logical size of the file
    if (offset + data_len > f->size) {
        f->size = offset + data_len;
    }
}

void read_file(const char *filename, int offset, int size){

    // 1. Search for the file
    int file_index = is_fname_used(filename);
    if (file_index == -1) {
        printf("Error: archivo '%s' no existe.\n", filename);
        return;
    }

    FileEntry *f = &file_table[file_index];

    // 2. Validate ranges
    if (offset < 0 || offset >= f->max_size) {
        printf("Error: offset fuera del rango del archivo.\n");
        return;
    }

    if (offset + size > f->max_size) {
        printf("Error: lectura excede el tamaño del archivo.\n");
        return;
    }

    // 3. Create buffer to store what is read
    char *buffer = malloc(size + 1);
    if (!buffer) {
        printf("Error: no se pudo asignar memoria para la lectura.\n");
        return;
    }

    // 4. Read byte by byte, block by block
    for (int i = 0; i < size; i++) {
        int global_pos      = offset + i;              // position within the file
        int block_index     = global_pos / BLOCK_SIZE; // logical block index
        int internal_offset = global_pos % BLOCK_SIZE; // position within the block

        int real_block = f->blocks[block_index];       // physical block in data_blocks

        buffer[i] = (char) data_blocks[real_block][internal_offset];
    }

    buffer[size] = '\0'; // terminate string

    // 5. Print result
    printf("\n%s\n", buffer);

    free(buffer);
}

void delete_file(const char *filename){

    // 1. Search for the file in the table
    int file_index = is_fname_used(filename);
    if (file_index == -1) {
        printf("Error: archivo '%s' no existe.\n", filename);
        return;
    }

    FileEntry *f = &file_table[file_index];

    // 2. Free blocks
    for (int i = 0; i < f->num_blocks; i++) {
        int b = f->blocks[i];
        if (b >= 0 && b < NUM_BLOCKS) {
            blocks_used[b] = 0;                  // mark block as free
            // clear the content
            memset(data_blocks[b], 0, BLOCK_SIZE);
        }
        f->blocks[i] = -1; // mark block as free
    }

    // 3. Clear file entry
    f->used      = 0;
    f->size      = 0;
    f->max_size  = 0;
    f->num_blocks = 0;
    memset(f->name, 0, MAX_FILENAME);

    printf("\nArchivo '%s' eliminado correctamente.\n", filename);
}

void list_files(void){
    int count = 0;
    printf("\n");
    for (int i = 0; i < MAX_FILES; i++){
        if (file_table[i].used == 1){
            printf("%s - %d bytes\n", file_table[i].name, file_table[i].max_size);
            count++;
        }
    }

    if (count == 0) {
        printf("(no hay archivos)");
    }
}

// === AUX FUNCTIONS ===

void read_test_file(const char *file){
    FILE *fp = fopen(file, "r");

    if (!fp) {
        printf("No se ha podido abrir %s\n", file);
        exit(1);
    }

    char line[128];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        char fname[40], data[200];
        int size, offset;

        if (sscanf(line, "CREATE %s %d", fname, &size) == 2) {
            printf("\n> CREATE %s %d ", fname, size);
            create_file(fname, size);
        } else if (sscanf(line, "WRITE %39s %d \"%199[^\"]\"", fname, &offset, data) == 3) {
            printf("\n> WRITE %s %d \"%s\" ", fname, offset, data);
            write_file(fname, offset, data);
        } else if (sscanf(line, "READ %s %d %d",  fname, &offset, &size) == 3) {
            printf("\n> READ %s %d %d ", fname, offset, size);
            read_file(fname, offset, size);
        } else if (sscanf(line, "DELETE %s", fname) == 1) {
            printf("\n> DELETE %s ", fname);
            delete_file(fname);
        } else if (strncmp(line, "LIST", 4) == 0) {
            printf("\n> LIST ");
            list_files();
        }
    }

    fclose(fp);
}

int is_fname_used(const char *name){
    for (int i = 0; i < MAX_FILES; i++){
        if (file_table[i].used == 1 && strcmp(file_table[i].name, name) == 0){
            return i;
        }
    }
    return -1;
}

int find_free_file_entry(){
    for (int i = 0; i < MAX_FILES; i++){
        if (file_table[i].used == 0){
            return i;
        }
    }
    return -1;
}

int count_free_blocks(){
    int count = 0;
    for (int i = 0; i < NUM_BLOCKS; i++){
        if (!blocks_used[i]){
            count++;
        }
    }
    return count;
}

int allocate_blocks(int n, int out_blocks[]){
    int found = 0;
    // Allocate free blocks
    for (int i = 0; i < NUM_BLOCKS && found < n; i++) {
        if (!blocks_used[i]) {
            blocks_used[i] = 1;
            out_blocks[found++] = i;
        }
    }

    if (found < n) {
        // Not enough free blocks: revert the allocated blocks
        for (int j = 0; j < found; j++) {
            blocks_used[out_blocks[j]] = 0;
        }
        return -1;
    }
    return 0;
}
