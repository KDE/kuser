/* 
 * Cryptographic API.
 *
 * MD4 Message Digest Algorithm (RFC1320).
 *
 * Implementation derived from Andrew Tridgell and Steve French's
 * CIFS MD4 implementation, and the cryptoapi implementation
 * originally based on the public domain implementation written
 * by Colin Plumb in 1993.
 *
 * Copyright (c) Andrew Tridgell 1997-1998.
 * Modified by Steve French (sfrench@us.ibm.com) 2002
 * Copyright (c) Cryptoapi developers.
 * Copyright (c) 2002 David S. Miller (davem@redhat.com)
 * Copyright (c) 2002 James Morris <jmorris@intercode.com.au>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef _MD4_H_
#define _MD4_H_

#include <qglobal.h>

#define MD4_DIGEST_SIZE  16
#define MD4_HMAC_BLOCK_SIZE  64
#define MD4_BLOCK_WORDS  16
#define MD4_HASH_WORDS  4


struct md4_ctx {
  Q_UINT32 hash[MD4_HASH_WORDS];
  Q_UINT32 block[MD4_BLOCK_WORDS];
  Q_UINT64 byte_count;
};

void md4_init(void *ctx);
void md4_update(void *ctx, const Q_UINT8 *data, unsigned int len);
void md4_final(void* ctx, Q_UINT8 *out);

#endif
