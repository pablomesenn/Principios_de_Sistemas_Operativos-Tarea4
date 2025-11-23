#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define BLOCK_SIZE 512
#define NUM_BLOCKS 2048

#define MAX_FILES 100
#define MAX_FILENAME 32
#define MAX_BLOCKS_PER_FILE 256

typedef struct {
    char name[MAX_FILENAME];
    int size;                          // in bytes
    int max_size;                      // in bytes as well
    int blocks[MAX_BLOCKS_PER_FILE];   // block indexes
    int num_blocks;                    // number of blocks used
    int used;                          // 0 = free, 1 = not free
} FileEntry;

// === MAIN OPERATIONS ===

void create_file(const char *name, int size);
void write_file(const char *filename, int offset, const char *data);
void read_file(const char *filename, int offset, int size);
void delete_file(const char *filename);
void list_files(void);

// === AUX FUNCTIONS ===

void read_test_file(const char *file);
int is_fname_used(const char *name);
int find_free_file_entry();
int count_free_blocks();
int allocate_blocks(int n, int out_blocks[]);

#endif
