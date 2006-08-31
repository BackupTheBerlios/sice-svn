/* Copyright (C) 2006  Movial Oy
 * authors:     ville.syrjala@movial.fi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,MA 02111-1307 USA 16
 */

/* HMAC-SHA1 implementation based on RFC 2104 and RFC 3174 */

#include "sha1.h"
#include <stdint.h>
#include <string.h>

typedef struct {
	uint8_t buffer[64];
	uint64_t len;
	uint32_t h[5];
} sha1_context_t;

static void sha1_init(sha1_context_t *ctx);
static void sha1_update(sha1_context_t *ctx, const void *in, int len);
static void sha1_final(sha1_context_t *ctx, void *out);
static void sha1_process(sha1_context_t *ctx, const uint8_t *in);

void hmac_sha1(const void *key, int key_len,
	       const void *msg, int msg_len,
	       void *digest)
{
	sha1_context_t ctx;
	uint8_t ipad[64], opad[64], new_key[20];
	int i;

	/* Key too long? */
	if (key_len > 64) {
		sha1_init(&ctx);
		sha1_update(&ctx, key, key_len);
		sha1_final(&ctx, new_key);

		key = new_key;
		key_len = 20;
	}

	memset(opad, 0, 64);
	memcpy(opad, key, key_len);

	memset(ipad, 0, 64);
	memcpy(ipad, key, key_len);

	for (i = 0; i < 64; i++) {
		ipad[i] ^= 0x36;
		opad[i] ^= 0x5C;
	}

	sha1_init(&ctx);
	sha1_update(&ctx, ipad, 64);
	sha1_update(&ctx, msg, msg_len);
	sha1_final(&ctx, digest);

	sha1_init(&ctx);
	sha1_update(&ctx, opad, 64);
	sha1_update(&ctx, digest, 20);
	sha1_final(&ctx, digest);
}

static void sha1_init(sha1_context_t *ctx)
{
	ctx->h[0] = 0x67452301;
	ctx->h[1] = 0xEFCDAB89;
	ctx->h[2] = 0x98BADCFE;
	ctx->h[3] = 0x10325476;
	ctx->h[4] = 0xC3D2E1F0;
	ctx->len = 0;
}

static void sha1_update(sha1_context_t *ctx, const void *in, int len)
{
	const uint8_t *data = in;
	int partial;

	/* existing partial chunk? */
	partial = ctx->len & 63;

	ctx->len += len;

	/* fill the last partial chunk */
	if (partial && len + partial > 63) {
		int rem = 64 - partial;

		memcpy(ctx->buffer + partial, data, rem);

		sha1_process(ctx, (const uint8_t *) ctx->buffer);

		data += rem;
		len -= rem;

		/* next partial chunk goes to the beginning of the buffer */
		partial = 0;
	}

	/* process full chunks */
	while (len > 63) {
		sha1_process(ctx, (const uint8_t *) data);
		len -= 64;
		data += 64;
	}

	/* copy remaining partial chunk to buffer */
	memcpy(ctx->buffer + partial, data, len);
}

#ifdef WORDS_BIGENDIAN
#define swap32(a) (a)
#define swap64(a) (a)
#else
#define swap32(a) ((((a) & 0xFF000000U) >> 24) | \
		   (((a) & 0x00FF0000U) >>  8) | \
		   (((a) & 0x0000FF00U) <<  8) | \
		   (((a) & 0x000000FFU) << 24))

#define swap64(a) ((((a) & 0xFF00000000000000ULL) >> 56) | \
		   (((a) & 0x00FF000000000000ULL) >> 40) | \
		   (((a) & 0x0000FF0000000000ULL) >> 24) | \
		   (((a) & 0x000000FF00000000ULL) >>  8) | \
		   (((a) & 0x00000000FF000000ULL) <<  8) | \
		   (((a) & 0x0000000000FF0000ULL) << 24) | \
		   (((a) & 0x000000000000FF00ULL) << 40) | \
		   (((a) & 0x00000000000000FFULL) << 56))
#endif

static void sha1_final(sha1_context_t *ctx, void *out)
{
	const uint8_t pad[64] = { 0x80, };
	uint64_t msglen = swap64(ctx->len << 3);
	uint32_t *data = out;
	int len, i;

	/* check how much padding is required */
	len = 56 - (ctx->len & 63);
	/* enough space in current chunk? */
	if (len < 1)
		len += 64;

	sha1_update(ctx, pad, len);

	sha1_update(ctx, &msglen, 8);

	for (i = 0; i < 5; i++)
		data[i] = swap32(ctx->h[i]);
}

#define rol32(a, b) ((a) << (b) | ((a) >> (32 - (b))))

#if 0
#define f0(b, c, d) (((b) & (c)) | ((~(b)) & (d)))
#define f1(b, c, d) ((b) ^ (c) ^ (d))
#define f2(b, c, d) (((b) & (c)) | ((b) & (d)) | ((c) & (d)))
#else
#define f0(b, c, d) ((d) ^ ((b) & ((c) ^ (d))))
#define f1(b, c, d) ((b) ^ (c) ^ (d))
#define f2(b, c, d) (((b) & (c)) | ((d) & ((b) | (c))))
#endif

static void sha1_process(sha1_context_t *ctx, const uint8_t *in)
{
	const uint32_t k[] = {
		0x5A827999,
		0x6ED9EBA1,
		0x8F1BBCDC,
		0xCA62C1D6,
	};
	uint32_t w[80], temp, a, b, c, d, e;
	int i, j;

	for (i = 0, j = 0; i < 16; i++, j += 4)
		w[i] = (in[j] << 24) | (in[j+1] << 16) |
			(in[j+2] << 8) | in[j+3];
	for (; i < 80; i++)
		w[i] = rol32(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);

	a = ctx->h[0];
	b = ctx->h[1];
	c = ctx->h[2];
	d = ctx->h[3];
	e = ctx->h[4];

	for (i = 0; i < 20; i++) {
		temp = rol32(a, 5) + f0(b, c, d) + e + w[i] + k[0];

		e = d;
		d = c;
		c = rol32(b, 30);
		b = a;
		a = temp;
	}
	for (; i < 40; i++) {
		temp = rol32(a, 5) + f1(b, c, d) + e + w[i] + k[1];

		e = d;
		d = c;
		c = rol32(b, 30);
		b = a;
		a = temp;
	}
	for (; i < 60; i++) {
		temp = rol32(a, 5) + f2(b, c, d) + e + w[i] + k[2];

		e = d;
		d = c;
		c = rol32(b, 30);
		b = a;
		a = temp;
	}
	for (; i < 80; i++) {
		temp = rol32(a, 5) + f1(b, c, d) + e + w[i] + k[3];

		e = d;
		d = c;
		c = rol32(b, 30);
		b = a;
		a = temp;
	}

	ctx->h[0] += a;
	ctx->h[1] += b;
	ctx->h[2] += c;
	ctx->h[3] += d;
	ctx->h[4] += e;
}
