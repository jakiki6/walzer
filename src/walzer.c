#include "walzer.h"

#define ROTL(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
#define QR(a, b, c, d) (            \
    a += b,  d ^= a,  d = ROTL(d,16),   \
    c += d,  b ^= c,  b = ROTL(b,12),   \
    a += b,  d ^= a,  d = ROTL(d, 8),   \
    c += d,  b ^= c,  b = ROTL(b, 7))
#define ROUNDS 20

static void chacha20_core(uint32_t *io) {
    uint32_t x[16];
    for (int i = 0; i < 16; ++i) {
        x[i] = io[i];
    }   

    for (int i = 0; i < ROUNDS; i += 2) {
        QR(x[0], x[4], x[ 8], x[12]);
        QR(x[1], x[5], x[ 9], x[13]);
        QR(x[2], x[6], x[10], x[14]);
        QR(x[3], x[7], x[11], x[15]);
        QR(x[0], x[5], x[10], x[15]);
        QR(x[1], x[6], x[11], x[12]);
        QR(x[2], x[7], x[ 8], x[13]);
        QR(x[3], x[4], x[ 9], x[14]);
    }   
    for (int i = 0; i < 16; ++i) {
        io[i] = x[i] + io[i];
    }   
}

static char *init_const = "initial 32-bytes of walzer state";
void walzer_init(walzer_t *ctx) {
    memcpy(&ctx->S, init_const, 32);
    ctx->index = 0;
}

void walzer_update(walzer_t *ctx, void *_data, size_t length) {
    uint8_t *data = (uint8_t *) _data;

    while (length) {
        memcpy(&ctx->S + 32 + (ctx->index & 0x1f), data, length >= 32 - (ctx->index & 0x1f) ? 32 - (ctx->index & 0x1f) : length);
        int nindex = length >= 32 - (ctx->index & 0x1f) ? 32 - (ctx->index & 0x1f) : length;

        if (length >= 32 - (ctx->index & 0x1f)) {
            length -= 32 - (ctx->index & 0x1f);
        } else {
            length = 0;
        }

        if ((ctx->index & 0x1f) == 0) {
            chacha20_core((uint32_t *) &ctx->S);
            memcpy(&ctx->S, &ctx->S + 32, 32);
        }

        ctx->index = nindex;
    }
}

void walzer_digest(walzer_t *ctx, void *_digest) {
    uint8_t *digest = (uint8_t *) _digest;

    uint8_t S[64];
    memcpy(&S, &ctx->S, 64);
    memset(&S + 32 + (ctx->index & 0x1f), 0, 32 - (ctx->index & 0x1f));
    chacha20_core((uint32_t *) &S);
    memcpy(&S, &S[32], 32);
    memset((uint8_t *) &S[32], 0, 32);
    *((uint64_t *) (&S[32])) = ctx->index;
    chacha20_core((uint32_t *) &S);
    memcpy(digest, &S, 32);
}
