#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
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
#define MAX_LINE_WORDS 131072

#define EPSILON 0.005
#define DELTA 10E-7f
#define EXP_1 2.7182818284590451

int main(int argc, char **argv) {
    char *output_fname;
    unsigned char *sketch_buffer;
    int sketch_fd;

    int max_line = MAX_LINE;
    unsigned char input_line[MAX_LINE];
    int bytes_read;

    int line_count = 0;

    /* number of hash functions */
    uint64_t d = (uint64_t) ceil(EXP_1 / EPSILON);
    size_t hash_array_width = d * 2;

    uint32_t hash_functions[hash_array_width];

    /* initialize hash functions */

    for (int i=0; i < hash_array_width; i+=2) {
        hash_functions[i] = (uint32_t) rand() * (BIG_PRIME - 1);
        hash_functions[i+1] = (uint32_t) rand() * (BIG_PRIME - 1);
    }

    /* width of array */
    uint64_t w = (uint64_t) ceil(log(1 / DELTA));

    /* set up output file */
    if ((sketch_fd = open(output_fname, O_RDWR)) == -1) {
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

    sketch_buffer = mmap(0, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, sketch_fd, 0);

    uint64_t *file_d;
    uint64_t *file_w;
    file_d = (uint64_t *) sketch_buffer;
    sketch_buffer += sizeof(uint64_t);
    file_w = (uint64_t *) sketch_buffer;
    sketch_buffer += sizeof(uint64_t);

    *file_d = d;
    *file_w = w;

    uint32_t *sketch_arrays;
    sketch_arrays = (uint32_t *) sketch_buffer;

    /* write out hash function parameters */
    for (int i=0; i < d*2; i++) {

        sketch_arrays[i] = hash_functions[i];

    }

    sketch_arrays += sizeof(uint32_t) * (d * 2);

    while(1) {
        bytes_read = getline(&input_line, &max_line, stdin);
        if (bytes_read < 1) break;

        int in_word = -1;
        int word_start = 0;
        int word_end = 0;
        uint32_t prev_hash = 0;
        
        while (word_end < bytes_read) {

            switch (input_line[word_end]) {

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
                case 'w':
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
                    if (in_word == 0) {
                        /* We weren't in a word before,
                         * so this is the start of a new word */
                        word_start = word_end;
                    }
                    in_word = 1;

                default:
                    /* This is not part of a word */
                    if (in_word == 1) {
                        /* we were in a word, and now we're not 
                         * compute word hashes */
                        int word_length = word_end - word_start - 1;
                        uint32_t hash = murmur3_32(
                                input_line+word_start, word_length, MURMUR_SEED);
                        uint32_t bigram_hash = hash ^ prev_hash;

                        /* BEGIN SKETCH INC */

                        for (int i = 0; i < hash_array_width; i += 2) {

                            uint32_t a = hash_functions[i];
                            uint32_t b = hash_functions[i+1];

                            uint32_t column = (a * bigram_hash + b) % BIG_PRIME % w;
                            uint32_t array_offset = w * i + column;

                            sketch_arrays[array_offset]++;
                            
                            column = (a * hash + b) % BIG_PRIME % w;
                            array_offset = w * i + column;

                            sketch_arrays[array_offset]++;

                        }
                        

                        /* END SKETCH INC */

                        prev_hash = hash;
                    }
                    in_word = 0;
            }

            word_end++;
        }


        line_count++;
        if (line_count % 10000 == 0) {
            printf("%d\n", line_count);
        }
    }

    return 0;

}
