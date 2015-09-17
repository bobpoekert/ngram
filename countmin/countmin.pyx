import os
from libc.stdint cimport uint32_t, uint64_t, uintptr_t

cdef uint32_t MURMUR_SEED = 0xf00df00d
cdef uint64_t BIG_PRIME = 9223372036854775783L

cdef extern from "murmur.c":
    uint32_t murmur3_32(char *key, uint32_t len, uint32_t seed)

cdef extern from "sys/mman.h":
    void *mmap(void *addr, size_t len, int prot, int flags, int fd, size_t offset)
    int munmap(void *addr, size_t len)
    enum:
        PROT_READ
        MAP_FILE
        MAP_SHARED
        MAP_FAILED

cdef uint32_t murmur_hash_string(k):
    cdef char *k_chars = k
    cdef size_t k_len = len(k)

    return murmur3_32(k, k_len, MURMUR_SEED)

def murmur_hash(k):
    return murmur_hash_string(k)

cdef class Sketch:
    cdef int fd
    cdef unsigned char *buf
    cdef uint32_t *arrays
    cdef uint64_t d
    cdef uint64_t w
    cdef uint32_t *hash_functions
    cdef size_t file_size

    def get_d(self):
        return self.d

    def get_w(self):
        return self.w

    def get_fd(self):
        return self.fd

    def get_file_size(self):
        return self.file_size

    def __cinit__(self, fname):
        self.fd = os.open(fname, os.O_RDONLY)
        stat_result = os.stat(fname)
        self.file_size = stat_result.st_size
        if self.file_size < 1:
            raise IOError("File is empty")
        self.buf = <unsigned char *> mmap(NULL, self.file_size, PROT_READ, MAP_SHARED, self.fd, 0)
        if <int>self.buf == MAP_FAILED:
            raise IOError("Failed to mmap file")
        self.load_params()

    def __dealloc__(self):
        if self.buf is not NULL:
            munmap(self.buf, self.file_size)

    cdef load_params(self):
        cdef unsigned char *buf = self.buf
        self.d = (<uint64_t *>buf)[0]
        buf += 8
        self.w = (<uint64_t *>buf)[0]
        buf += 8

        self.hash_functions = <uint32_t *>buf
        buf += 4 * self.d * 2

        self.arrays = <uint32_t *>buf

    def get_count(self, k):
        if type(k) in (str, unicode):
            return self.get_hash_count(murmur_hash_string(k))
        else:
            h = 0
            for e in k:
                h = h ^ murmur_hash_string(e)
            return self.get_hash_count(h)

    def __getitem__(self, k):
        return self.get_count(k)

    cdef uint32_t get_hash_count(self, uint32_t hash_val):
        cdef size_t hash_func_indexes = self.d * 2
        cdef uint32_t res = 0
        cdef int started = 0
        cdef uint32_t a
        cdef uint32_t b
        cdef uint32_t column
        cdef uint32_t array_offset
        cdef uint32_t val
        cdef int i

        cdef uint32_t *hash_functions = self.hash_functions
        cdef uint32_t *arrays = self.arrays
        cdef uint64_t w = self.w

        cdef int row_index = 0
        for i from 0 <= i < hash_func_indexes-1 by 2:
            a = hash_functions[i]
            b = hash_functions[i+1]

            column = (a * hash_val + b) % BIG_PRIME % w
            array_offset = w * row_index + column

            val = arrays[array_offset]

            if started == 0:
                started = 1
                res = val
            elif val < res:
                res = val

            row_index += 1

        return res
