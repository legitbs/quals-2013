/* ed2k.c - an implementation of EDonkey 2000 Hash Algorithm.
 *
 * Copyright: 2006-2012 Aleksey Kravchenko <rhash.admin@gmail.com>
 *
 * Permission is hereby granted,  free of charge,  to any person  obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction,  including without limitation
 * the rights to  use, copy, modify,  merge, publish, distribute, sublicense,
 * and/or sell copies  of  the Software,  and to permit  persons  to whom the
 * Software is furnished to do so.
 *
 * This program  is  distributed  in  the  hope  that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  Use this program  at  your own risk!
 *
 * This file implements eMule-compatible version of algorithm.
 * Note that eDonkey and eMule ed2k hashes are different only for
 * files containing exactly multiple of 9728000 bytes.
 *
 * The file data is divided into full chunks of 9500 KiB (9728000 bytes) plus
 * a remainder chunk, and a separate 128-bit MD4 hash is computed for each.
 * If the file length is an exact multiple of 9500 KiB, the remainder zero
 * size chunk is still used at the end of the hash list. The ed2k hash is
 * computed by concatenating the chunks' MD4 hashes in order and hashing the
 * result using MD4. Although, if the file is composed of a single non-full
 * chunk, its MD4 hash is returned with no further modifications.
 *
 * See http://en.wikipedia.org/wiki/EDonkey_network for algorithm description.
 */

#include "common.h"

#define IS_ALIGNED_32(p) (0 == (3 & ((const char*)(p) - (const char*)0)))
#define IS_ALIGNED_64(p) (0 == (7 & ((const char*)(p) - (const char*)0)))

#define IS_LITTLE_ENDIAN 1
# define be32_copy(to, index, from, length) rhash_u32_swap_copy((to), (index), (from), (length))
# define le32_copy(to, index, from, length) memcpy((to) + (index), (from), (length))
# define be64_copy(to, index, from, length) rhash_u64_swap_copy((to), (index), (from), (length))
# define le64_copy(to, index, from, length) memcpy((to) + (index), (from), (length))

/* ROTL/ROTR macros rotate a 32/64-bit word left/right by n bits */
#define ROTL32(dword, n) ((dword) << (n) ^ ((dword) >> (32 - (n))))
#define ROTR32(dword, n) ((dword) >> (n) ^ ((dword) << (32 - (n))))
#define ROTL64(qword, n) ((qword) << (n) ^ ((qword) >> (64 - (n))))
#define ROTR64(qword, n) ((qword) >> (n) ^ ((qword) << (64 - (n))))

#define md4_block_size 64
#define md4_hash_size  16

/* algorithm context */
typedef struct md4_ctx
{
    unsigned hash[4];  /* 128-bit algorithm internal hashing state */
    unsigned message[md4_block_size / 4]; /* 512-bit buffer for leftovers */
    uint64_t length;   /* number of processed bytes */
} md4_ctx;

void rhash_md4_init(md4_ctx *ctx)
{
    ctx->length = 0;

    /* initialize state */
    ctx->hash[0] = 0x67452301;
    ctx->hash[1] = 0xefcdab89;
    ctx->hash[2] = 0x98badcfe;
    ctx->hash[3] = 0x10325476;
}

/* First, define three auxiliary functions that each take as input
 * three 32-bit words and returns a 32-bit word.
 *  F(x,y,z) = XY v not(X) Z = ((Y xor Z) X) xor Z (the last form is faster)
 *  G(X,Y,Z) = XY v XZ v YZ
 *  H(X,Y,Z) = X xor Y xor Z */

#define MD4_F(x, y, z) ((((y) ^ (z)) & (x)) ^ (z))
#define MD4_G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define MD4_H(x, y, z) ((x) ^ (y) ^ (z))

/* transformations for rounds 1, 2, and 3. */
#define MD4_ROUND1(a, b, c, d, x, s) { \
    (a) += MD4_F((b), (c), (d)) + (x); \
    (a) = ROTL32((a), (s)); \
}
#define MD4_ROUND2(a, b, c, d, x, s) { \
    (a) += MD4_G((b), (c), (d)) + (x) + 0x5a827999; \
    (a) = ROTL32((a), (s)); \
}
#define MD4_ROUND3(a, b, c, d, x, s) { \
    (a) += MD4_H((b), (c), (d)) + (x) + 0x6ed9eba1; \
    (a) = ROTL32((a), (s)); \
}

/**
 * The core transformation. Process a 512-bit block.
 * The function has been taken from RFC 1320 with little changes.
 *
 * @param state algorithm state
 * @param x the message block to process
 */
static void rhash_md4_process_block(unsigned state[4], const unsigned* x)
{
    register unsigned a, b, c, d;
    a = state[0], b = state[1], c = state[2], d = state[3];

    MD4_ROUND1(a, b, c, d, x[ 0],  3);
    MD4_ROUND1(d, a, b, c, x[ 1],  7);
    MD4_ROUND1(c, d, a, b, x[ 2], 11);
    MD4_ROUND1(b, c, d, a, x[ 3], 19);
    MD4_ROUND1(a, b, c, d, x[ 4],  3);
    MD4_ROUND1(d, a, b, c, x[ 5],  7);
    MD4_ROUND1(c, d, a, b, x[ 6], 11);
    MD4_ROUND1(b, c, d, a, x[ 7], 19);
    MD4_ROUND1(a, b, c, d, x[ 8],  3);
    MD4_ROUND1(d, a, b, c, x[ 9],  7);
    MD4_ROUND1(c, d, a, b, x[10], 11);
    MD4_ROUND1(b, c, d, a, x[11], 19);
    MD4_ROUND1(a, b, c, d, x[12],  3);
    MD4_ROUND1(d, a, b, c, x[13],  7);
    MD4_ROUND1(c, d, a, b, x[14], 11);
    MD4_ROUND1(b, c, d, a, x[15], 19);

    MD4_ROUND2(a, b, c, d, x[ 0],  3);
    MD4_ROUND2(d, a, b, c, x[ 4],  5);
    MD4_ROUND2(c, d, a, b, x[ 8],  9);
    MD4_ROUND2(b, c, d, a, x[12], 13);
    MD4_ROUND2(a, b, c, d, x[ 1],  3);
    MD4_ROUND2(d, a, b, c, x[ 5],  5);
    MD4_ROUND2(c, d, a, b, x[ 9],  9);
    MD4_ROUND2(b, c, d, a, x[13], 13);
    MD4_ROUND2(a, b, c, d, x[ 2],  3);
    MD4_ROUND2(d, a, b, c, x[ 6],  5);
    MD4_ROUND2(c, d, a, b, x[10],  9);
    MD4_ROUND2(b, c, d, a, x[14], 13);
    MD4_ROUND2(a, b, c, d, x[ 3],  3);
    MD4_ROUND2(d, a, b, c, x[ 7],  5);
    MD4_ROUND2(c, d, a, b, x[11],  9);
    MD4_ROUND2(b, c, d, a, x[15], 13);

    MD4_ROUND3(a, b, c, d, x[ 0],  3);
    MD4_ROUND3(d, a, b, c, x[ 8],  9);
    MD4_ROUND3(c, d, a, b, x[ 4], 11);
    MD4_ROUND3(b, c, d, a, x[12], 15);
    MD4_ROUND3(a, b, c, d, x[ 2],  3);
    MD4_ROUND3(d, a, b, c, x[10],  9);
    MD4_ROUND3(c, d, a, b, x[ 6], 11);
    MD4_ROUND3(b, c, d, a, x[14], 15);
    MD4_ROUND3(a, b, c, d, x[ 1],  3);
    MD4_ROUND3(d, a, b, c, x[ 9],  9);
    MD4_ROUND3(c, d, a, b, x[ 5], 11);
    MD4_ROUND3(b, c, d, a, x[13], 15);
    MD4_ROUND3(a, b, c, d, x[ 3],  3);
    MD4_ROUND3(d, a, b, c, x[11],  9);
    MD4_ROUND3(c, d, a, b, x[ 7], 11);
    MD4_ROUND3(b, c, d, a, x[15], 15);

    state[0] += a, state[1] += b, state[2] += c, state[3] += d;
}

/**
 * Calculate message hash.
 * Can be called repeatedly with chunks of the message to be hashed.
 *
 * @param ctx the algorithm context containing current hashing state
 * @param msg message chunk
 * @param size length of the message chunk
 */
void rhash_md4_update(md4_ctx *ctx, const unsigned char* msg, size_t size)
{
    unsigned index = (unsigned)ctx->length & 63;
    ctx->length += size;

    /* fill partial block */
    if(index) {
        unsigned left = md4_block_size - index;
        le32_copy((char*)ctx->message, index, msg, (size < left ? size : left));
        if(size < left) return;

        /* process partial block */
        rhash_md4_process_block(ctx->hash, ctx->message);
        msg  += left;
        size -= left;
    }
    while(size >= md4_block_size) {
        unsigned* aligned_message_block;
        if(IS_LITTLE_ENDIAN && IS_ALIGNED_32(msg)) {
            /* the most common case is processing a 32-bit aligned message
            on a little-endian CPU without copying it */
            aligned_message_block = (unsigned*)msg;
        } else {
            le32_copy(ctx->message, 0, msg, md4_block_size);
            aligned_message_block = ctx->message;
        }

        rhash_md4_process_block(ctx->hash, aligned_message_block);
        msg  += md4_block_size;
        size -= md4_block_size;
    }
    if(size) {
        /* save leftovers */
        le32_copy(ctx->message, 0, msg, size);
    }
}

/**
 * Store calculated hash into the given array.
 *
 * @param ctx the algorithm context containing current hashing state
 * @param result calculated hash in binary form
 */
void rhash_md4_final(md4_ctx *ctx, unsigned char result[16])
{
    unsigned index = ((unsigned)ctx->length & 63) >> 2;
    unsigned shift = ((unsigned)ctx->length & 3) * 8;

    /* pad message and run for last block */

    /* append the byte 0x80 to the message */
    ctx->message[index]   &= ~(0xFFFFFFFF << shift);
    ctx->message[index++] ^= 0x80 << shift;

    /* if no room left in the message to store 64-bit message length */
    if(index > 14) {
        /* then fill the rest with zeros and process it */
        while(index < 16) {
            ctx->message[index++] = 0;
        }
        rhash_md4_process_block(ctx->hash, ctx->message);
        index = 0;
    }
    while(index < 14) {
        ctx->message[index++] = 0;
    }
    ctx->message[14] = (unsigned)(ctx->length << 3);
    ctx->message[15] = (unsigned)(ctx->length >> 29);
    rhash_md4_process_block(ctx->hash, ctx->message);

    if(result) le32_copy(result, 0, &ctx->hash, 16);
}

typedef struct ed2k_ctx
{
    md4_ctx md4_context_inner; /* md4 context to hash file blocks */
    md4_ctx md4_context;       /* md4 context to hash block hashes */
    int not_emule;             /* flag: 0 for emule ed2k algorithm */
} ed2k_ctx;

/* hash functions */

void rhash_ed2k_init(ed2k_ctx *ctx);
void rhash_ed2k_update(ed2k_ctx *ctx, const unsigned char* msg, size_t size);
void rhash_ed2k_final(ed2k_ctx *ctx, unsigned char result[16]);

#define ED2K_CHUNK_SIZE 9728000

/**
 * Initialize context before calculaing hash.
 *
 * @param ctx context to initialize
 */
void rhash_ed2k_init(ed2k_ctx *ctx)
{
    rhash_md4_init(&ctx->md4_context);
    rhash_md4_init(&ctx->md4_context_inner);
    ctx->not_emule = 0;
}

/**
 * Calculate message hash.
 * Can be called repeatedly with chunks of the message to be hashed.
 *
 * @param ctx the algorithm context containing current hashing state
 * @param msg message chunk
 * @param size length of the message chunk
 */
void rhash_ed2k_update(ed2k_ctx *ctx, const unsigned char* msg, size_t size)
{
    unsigned char chunk_md4_hash[16];
    unsigned blockleft = ED2K_CHUNK_SIZE - (unsigned)ctx->md4_context_inner.length;

    /* note: eMule-compatible algorithm hashes by md4_inner
    * the messages which sizes are multiple of 9728000
    * and then processes obtained hash by external md4 */

    while( size >= blockleft )
    {
        if(size == blockleft && ctx->not_emule) break;

        /* if internal ed2k chunk is full, then finalize it */
        rhash_md4_update(&ctx->md4_context_inner, msg, blockleft);
        msg += blockleft;
        size -= blockleft;
        blockleft = ED2K_CHUNK_SIZE;

        /* just finished an ed2k chunk, updating md4_external context */
        rhash_md4_final(&ctx->md4_context_inner, chunk_md4_hash);
        rhash_md4_update(&ctx->md4_context, chunk_md4_hash, 16);
        rhash_md4_init(&ctx->md4_context_inner);
    }

    if(size) {
        /* hash leftovers */
        rhash_md4_update(&ctx->md4_context_inner, msg, size);
    }
}

/**
 * Store calculated hash into the given array.
 *
 * @param ctx the algorithm context containing current hashing state
 * @param result calculated hash in binary form
 */
void rhash_ed2k_final(ed2k_ctx *ctx, unsigned char result[16])
{
    /* check if hashed message size is greater or equal to ED2K_CHUNK_SIZE */
    if( ctx->md4_context.length ) {

        /* note: weird eMule algorithm always processes the inner
         * md4 context, no matter if it contains data or is empty */

        /* if any data are left in the md4_context_inner */
        if( (size_t)ctx->md4_context_inner.length > 0 || !ctx->not_emule)
        {
            /* emule algorithm processes aditional block, even if it's empty */
            unsigned char md4_digest_inner[16];
            rhash_md4_final(&ctx->md4_context_inner, md4_digest_inner);
            rhash_md4_update(&ctx->md4_context, md4_digest_inner, 16);
        }
        /* first call final to flush md4 buffer and finalize the hash value */
        rhash_md4_final(&ctx->md4_context, result);
        /* store the calculated ed2k hash in the md4_context_inner.hash */
        memcpy(&ctx->md4_context_inner.hash, &ctx->md4_context.hash, md4_hash_size);
    } else {
        /* return just the message MD4 hash */
        if(result) rhash_md4_final(&ctx->md4_context_inner, result);
    }
}

void ed2k( const unsigned char *input, size_t ilen, unsigned char output[16] )
{
    ed2k_ctx ctx;

    rhash_ed2k_init(&ctx);
    rhash_ed2k_update( &ctx, input, ilen );
    rhash_ed2k_final( &ctx, output);

    return;
}
