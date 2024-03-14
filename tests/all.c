#include <walzer.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BLOCK_SIZE 1<<24

int main(int argc, char *argv[]) {
    walzer_t *ctx = malloc(sizeof(walzer_t));
    walzer_init(ctx);

    char *buf = malloc(BLOCK_SIZE);
    while (true) {
        int read = fread(buf, 1, BLOCK_SIZE, stdin);
        if (read == 0) break;

        walzer_update(ctx, buf, read);
    }

    uint8_t digest1[32];
    walzer_digest(ctx, &digest1);
    for (int i = 0; i < 32; i++) printf("%02x", digest1[i]); printf("\n");

    free(buf);
    free(ctx);
}
