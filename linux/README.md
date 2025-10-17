# Linux build & run

```bash
mkdir -p build && cd build
cmake ..
cmake --build . --config Release

# Terminal A:
./tcp_server 0.0.0.0 3333

# Terminal B:
./tcp_client <IP_SERVER> 3333 "Hello123"
cat > linux/caesar.h <<'EOF'
#ifndef CAESAR_H
#define CAESAR_H
#include <stddef.h>
#include <stdint.h>

/* API mínima: el primer byte del mensaje es el shift (0..25) */
size_t caesar_encrypt_bytes(const char* plaintext, uint8_t shift,
                            uint8_t* out, size_t out_cap);
size_t caesar_decrypt_bytes(const uint8_t* in, size_t in_len,
                            char* out, size_t out_cap);

#endif
