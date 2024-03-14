#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t S[64];
    int index;
} walzer_t;

void walzer_init(walzer_t *ctx);
void walzer_update(walzer_t *ctx, void *data, size_t length);
void walzer_digest(walzer_t *ctx, void *digest);
