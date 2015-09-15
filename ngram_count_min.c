#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MURMUR_SEED 0xf00df00d

/* MurmurHash lifted from Wikipedia */
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed) {
    static const uint32_t c1 = 0xcc9e2d51;
    static const uint32_t c2 = 0x1b873593;
    static const uint32_t r1 = 15;
    static const uint32_t r2 = 13;
    static const uint32_t m = 5;
    static const uint32_t n = 0xe6546b64;

    uint32_t hash = seed;

    const int nblocks = len / 4;
    const uint32_t *blocks = (const uint32_t *) key;
    int i;
    for (i = 0; i < nblocks; i++) {
        uint32_t k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
    uint32_t k1 = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];

            k1 *= c1;
            k1 = (k1 << r1) | (k1 >> (32 - r1));
            k1 *= c2;
            hash ^= k1;
    }

    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}

#define BIG_PRIME  9223372036854775783L
#define MAX_LINE 131072

#define W 54366
#define D 24
#define EXP_1 2.7182818284590451

int main(int argc, char **argv) {
    char *output_fname;
    unsigned char *sketch_buffer;
    int sketch_fd;

    int bytes_read;

    output_fname = argv[1];
    
    uint64_t line_count = 0;

    /* number of hash functions */
    uint64_t d = D;
    size_t hash_array_width = d * 2;

    uint32_t hash_functions[hash_array_width];

    /* initialize hash functions */

    for (int i=0; i < hash_array_width; i+=2) {
        hash_functions[i] = (uint32_t) rand() * (BIG_PRIME - 1);
        hash_functions[i+1] = (uint32_t) rand() * (BIG_PRIME - 1);
    }

    /* width of array */
    uint64_t w = W;

    printf("d: %" PRIu64 ", w: %" PRIu64 "\n", d, w);

    /* set up output file */
    if ((sketch_fd = open(output_fname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
        fprintf(stderr, "Failed to open output file: %s\n", output_fname);
        return 1;
    }

    size_t file_size = 
        d * (w * sizeof(uint32_t)) + /* arrays */
        hash_array_width * sizeof(uint32_t) + /* hash function parameters */
        sizeof(d) + /* d */
        sizeof(w); /* w */

    /* allocate output file size */
    ftruncate(sketch_fd, file_size);

    printf("calling mmap; size: %d\n", file_size);
    sketch_buffer = mmap(0, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, sketch_fd, 0);

    if (sketch_buffer == MAP_FAILED) {
        fprintf(stderr, "Failed to map file\n");
        return 1;
    }

    printf("alocating pointers for d and w\n");
    uint64_t *file_d;
    uint64_t *file_w;
    file_d = (uint64_t *) sketch_buffer;
    sketch_buffer += sizeof(uint64_t);
    file_w = (uint64_t *) sketch_buffer;
    sketch_buffer += sizeof(uint64_t);

    printf("setting d and w\n");
    *file_d = d;
    *file_w = w;

    uint32_t *sketch_arrays;
    sketch_arrays = (uint32_t *) sketch_buffer;

    printf("setting hash function params\n");
    /* write out hash function parameters */
    for (int i=0; i < d*2; i++) {

        sketch_arrays[i] = hash_functions[i];

    }

    sketch_arrays += sizeof(uint32_t) * (d * 2);

    unsigned char word_buf[MAX_LINE];
    size_t word_buf_end = 0;

    printf("running\n");
    int in_word = -1;
    uint32_t prev_hash = 0;
    while(1) {
        int inchar_int = getchar();
        unsigned char inchar = (unsigned char) inchar_int;

        if (inchar_int == EOF) break;
        if (word_buf_end >= MAX_LINE) word_buf_end = 0;

        word_buf[word_buf_end] = inchar;

        switch (inchar) {

            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':

            case 'x':
            case 'y':
            case 'z':

            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':

            case '-':

                /* This is part of a word */
                in_word = 1;
                break;
            default:
                /* This is not part of a word */
                if (in_word == 1 && word_buf_end > 1) {
                    /* we were in a word, and now we're not 
                     * compute word hashes */
                   /* for (int i=0; i < word_buf_end; i++) {
                        putchar(word_buf[i]);
                    }
                    putchar('\n');*/
                    //printf("hashing word length: %d\n", word_buf_end);
                    uint32_t hash = murmur3_32(
                            word_buf, word_buf_end-1, MURMUR_SEED);
                    uint32_t bigram_hash = hash ^ prev_hash;

                    /* BEGIN SKETCH INC */

                   // printf("hashes: %d, %d\n", hash, bigram_hash);

                   // printf("inc sketch\n");
                    int hash_idx = 0;
                    for (int i = 0; i < hash_array_width-1; i += 2) {

                        uint32_t a = hash_functions[i];
                    //    printf("a: %d\n", a);
                        uint32_t b = hash_functions[i+1];
                    //    printf("b: %d\n", b);

                        uint32_t column = (a * bigram_hash + b) % BIG_PRIME % w;
                        uint32_t array_offset = w * hash_idx + column;

                        sketch_arrays[array_offset]++;
                        /*printf("%d %d %d %d %d\n",
                                sketch_arrays[array_offset], array_offset, w, hash_idx, column);*/
                        
                        column = (a * hash + b) % BIG_PRIME % w;
                        array_offset = w * hash_idx + column;

                        sketch_arrays[array_offset]++;

                      //  printf("hash idx: %d\n");
                        hash_idx++;

                    }
                    
                    /* END SKETCH INC */

                    prev_hash = hash;
                }
                in_word = 0;

                word_buf_end = 0;
                if (inchar == '\n') {
                    line_count++;
                    if (line_count % 1000000 == 0) {
                        printf("%" PRIu64 "\n", line_count);
                    }
                    if (line_count % 1100000000 == 0) {
                        fsync(sketch_fd);
                    }
                    prev_hash = 0;
                }
                break;
        }

        word_buf_end++;
       // printf("%d\n", word_buf_end);

    }

    return 0;

}
