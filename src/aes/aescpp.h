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
 Issue 01/08/2005

 This file contains the definitions required to use AES (Rijndael) in C++.
*/

#ifndef _AESCPP_H
#define _AESCPP_H

#include "aes.h"

#if defined( AES_ENCRYPT )

const unsigned aes_keybits = 256;
const unsigned aes_blocksize = AES_BLOCK_SIZE;

class AESencrypt
{
public:
    aes_encrypt_ctx cx[1];
    AESencrypt(void) { gen_tabs(); };
#ifdef  AES_128
    AESencrypt(const unsigned char key[])
        {   aes_encrypt_key128(key, cx); }
    aes_rval key128(const unsigned char key[])
        {   return aes_encrypt_key128(key, cx); }
#endif
#ifdef  AES_192
    aes_rval key192(const unsigned char key[])
        {   return aes_encrypt_key192(key, cx); }
#endif
#ifdef  AES_256
    aes_rval key256(const unsigned char key[])
        {   return aes_encrypt_key256(key, cx); }
#endif
#ifdef  AES_VAR
    aes_rval key(const unsigned char key[], int key_len)
        {   return aes_encrypt_key(key, key_len, cx); }
#endif
    aes_rval encrypt(const unsigned char in[], unsigned char out[]) const
        {   return aes_encrypt(in, out, cx);  }
#ifndef AES_MODES
    aes_rval ecb_encrypt(const unsigned char in[], unsigned char out[], int nb) const
        {   while(nb--)
            {   aes_encrypt(in, out, cx), in += AES_BLOCK_SIZE, out += AES_BLOCK_SIZE; }
        }
#endif
#ifdef AES_MODES
    aes_rval mode_reset(void)   { return aes_mode_reset(cx); }

    aes_rval ecb_encrypt(const unsigned char in[], unsigned char out[], int nb) const
        {   return aes_ecb_encrypt(in, out, nb, cx);  }

    aes_rval cbc_encrypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[]) const
        {   return aes_cbc_encrypt(in, out, nb, iv, cx);  }

    aes_rval cfb_encrypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[])
        {   return aes_cfb_encrypt(in, out, nb, iv, cx);  }

    aes_rval cfb_decrypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[])
        {   return aes_cfb_decrypt(in, out, nb, iv, cx);  }

    aes_rval ofb_crypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[])
        {   return aes_ofb_crypt(in, out, nb, iv, cx);  }

    typedef void ctr_fn(unsigned char ctr[]);

    aes_rval ctr_crypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[], ctr_fn cf)
        {   return aes_ctr_crypt(in, out, nb, iv, cf, cx);  }

#endif

};

#endif

#if defined( AES_DECRYPT )

class AESdecrypt
{
public:
    aes_decrypt_ctx cx[1];
    AESdecrypt(void) { gen_tabs(); };
#ifdef  AES_128
    AESdecrypt(const unsigned char key[])
            { aes_decrypt_key128(key, cx); }
    aes_rval key128(const unsigned char key[])
            { return aes_decrypt_key128(key, cx); }
#endif
#ifdef  AES_192
    aes_rval key192(const unsigned char key[])
            { return aes_decrypt_key192(key, cx); }
#endif
#ifdef  AES_256
    aes_rval key256(const unsigned char key[])
            { return aes_decrypt_key256(key, cx); }
#endif
#ifdef  AES_VAR
    aes_rval key(const unsigned char key[], int key_len)
            { return aes_decrypt_key(key, key_len, cx); }
#endif
    aes_rval decrypt(const unsigned char in[], unsigned char out[]) const
        {   return aes_decrypt(in, out, cx);  }
#ifndef AES_MODES
    aes_rval ecb_decrypt(const unsigned char in[], unsigned char out[], int nb) const
        {   while(nb--)
            {   aes_decrypt(in, out, cx), in += AES_BLOCK_SIZE, out += AES_BLOCK_SIZE; }
        }
#endif
#ifdef AES_MODES

    aes_rval ecb_decrypt(const unsigned char in[], unsigned char out[], int nb) const
        {   return aes_ecb_decrypt(in, out, nb, cx);  }

    aes_rval cbc_decrypt(const unsigned char in[], unsigned char out[], int nb, 
                                    unsigned char iv[]) const
        {   return aes_cbc_decrypt(in, out, nb, iv, cx);  }
#endif
};

#endif

#endif
