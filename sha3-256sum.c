#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include <nettle/sha3.h>

#define BUFLEN 256

uint8_t buf[BUFLEN];

#define mkdigest(name, sha3_size, sha3_ctx, sha3_init, sha3_update, sha3_digest) \
  uint8_t* name(int fd) { \
    struct sha3_ctx ctx; \
    sha3_init(&ctx); \
    int size; \
    while ((size = read(fd, buf, BUFLEN)) > 0) { \
      sha3_update(&ctx, size, buf); \
    } \
    static uint8_t digest[sha3_size]; \
    sha3_digest(&ctx, sha3_size, digest); \
    return digest; \
  }

#define mkdigest_(size, length) \
  mkdigest(digest_ ## size, length, sha3_ ## size ## _ctx, sha3_ ## size ## _init, sha3_ ## size ## _update, sha3_ ## size ## _digest)

mkdigest_(224, 28)
mkdigest_(256, 32)
mkdigest_(384, 48)
mkdigest_(512, 64)

void printhex(size_t len, uint8_t *buf) {
  for (size_t i = 0; i < len; i++) {
    printf("%02x", buf[i]);
  }
}

int main(int argc, char *argv[]) {
  int status = 0;
  uint8_t* (*digest)(int) = digest_256;
  size_t len = 32;
  if (strcmp(basename(argv[0]), "sha3-224sum") == 0) {
    digest = digest_224;
    len = 28;
  } else if (strcmp(basename(argv[0]), "sha3-384sum") == 0) {
    digest = digest_384;
    len = 48;
  } else if (strcmp(basename(argv[0]), "sha3-512sum") == 0) {
    digest = digest_512;
    len = 64;
  }
  if (argc == 1) {
    printhex(len, digest(0));
    printf("\n");
  } else {
    for (int arg = 1; arg < argc; arg++) {
      int fd = strcmp(argv[arg], "-") == 0 ? 0 : open(argv[arg], O_RDONLY);
      if (fd == -1) {
        error(0, errno, "%s", argv[arg]);
        status = 1;
        continue;
      }
      printhex(len, digest(fd));
      printf(" %s\n", argv[arg]);
      if (fd) {
        close(fd);
      }
    }
  }
  return status;
}
