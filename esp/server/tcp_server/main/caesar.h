#ifndef CAESAR_H
#define CAESAR_H
#include <stddef.h>
#include <stdint.h>

/* El primer byte del mensaje es el shift (0..25) */
size_t caesar_encrypt_bytes(const char* plaintext, uint8_t shift,
                            uint8_t* out, size_t out_cap);
size_t caesar_decrypt_bytes(const uint8_t* in, size_t in_len,
                            char* out, size_t out_cap);

#endif
