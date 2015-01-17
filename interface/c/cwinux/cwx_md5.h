/* $XJG: xslib/md5.h,v 1.4 2006/03/16 04:25:13 jiagui Exp $ */
/*
 * RFC 1321 compliant MD5 implementation
 *
 * Copyright (C) 2001-2003	 Christophe Devine
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __CWX_MD5_H__
#define __CWX_MD5_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t total[2];
	uint32_t state[4];
	unsigned char buffer[64];
} cwx_md5_context;

void cwx_md5_start(cwx_md5_context *ctx);
void cwx_md5_update(cwx_md5_context *ctx, const void *input, unsigned int length);
void cwx_md5_finish(cwx_md5_context *ctx, unsigned char digest[16]);

void cwx_md5_checksum(unsigned char digest[16], const void *input, unsigned int length);

#ifdef __cplusplus
}
#endif

#endif

