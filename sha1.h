/*
 * Cryptographic API.
 *
 * SHA1 Secure Hash Algorithm.
 *
 * Derived from cryptoapi implementation, adapted for in-place
 * scatterlist interface.  Originally based on the public domain
 * implementation written by Steve Reid.
 *
 * Copyright (c) Alan Smithee.
 * Copyright (c) Andrew McDonald <andrew@mcdonald.org.uk>
 * Copyright (c) Jean-Francois Dive <jef@linuxbe.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 */

#ifndef _SHA1_H_
#define _SHA1_H_

#include <qglobal.h>

struct sha1_ctx {
        Q_UINT64 count;
        Q_UINT32 state[5];
        Q_UINT8 buffer[64];
};

void sha1_init(void *ctx);
void sha1_update(void *ctx, const Q_UINT8 *data, unsigned int len);
void sha1_final(void* ctx, Q_UINT8 *out);

#endif
