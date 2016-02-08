/*
 ---------------------------------------------------------------------------
 Copyright (c) 2003, Dr Brian Gladman, Worcester, UK.   All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products
      built using this software without specific written permission.

 ALTERNATIVELY, provided that this notice is retained in full, this product
 may be distributed under the terms of the GNU General Public License (GPL),
 in which case the provisions of the GPL apply INSTEAD OF those given above.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue 1/08/2005

 This file contains the definitions required to use AES in C. See aesopt.h
 for optimisation details.
*/

#ifndef _AESM_H
#define _AESM_H

#if defined(__cplusplus)
extern "C"
{
#endif

void aes_ecb_encrypt_128(
    const unsigned char *ibuf, unsigned char *obuf, unsigned int no_of_blocks,
    const unsigned char *key);
void aes_ecb_encrypt_192(
    const unsigned char *ibuf, unsigned char *obuf, unsigned int no_of_blocks,
    const unsigned char *key);
void aes_ecb_encrypt_256(
    const unsigned char *ibuf, unsigned char *obuf, unsigned int no_of_blocks,
    const unsigned char *key);

void aes_cbc_encrypt_128(
    const unsigned char *ibuf, unsigned char *obuf, unsigned int no_of_blocks,
    const unsigned char *key, unsigned char *iv);

void aes_cbc_encrypt_192(
    const unsigned char *ibuf, unsigned char *obuf, unsigned int no_of_blocks,
    const unsigned char *key, unsigned char *iv);

void aes_cbc_encrypt_256(
    const unsigned char *ibuf, unsigned char *obuf, unsigned int no_of_blocks,
    const unsigned char *key, unsigned char *iv);

void aes_ctr_encrypt_128(
    const unsigned char *ibuf, unsigned char *obuf, unsigned int no_of_blocks,
    const unsigned char *key, unsigned char *ctr);

void aes_ctr_encrypt_192(
    const unsigned char *ibuf, unsigned char *obuf, unsigned int no_of_blocks,
    const unsigned char *key, unsigned char *ctr);

void aes_ctr_encrypt_256(
    const unsigned char *ibuf, unsigned char *obuf, unsigned int no_of_blocks,
    const unsigned char *key, unsigned char *ctr);

#if defined(__cplusplus)
}
#endif

#endif
