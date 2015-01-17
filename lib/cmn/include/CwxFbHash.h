#ifndef __CWX_FB_HASH_H__
#define __CWX_FB_HASH_H__

/*
* Copyright 2012 Facebook, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/**
@file CwxFbHash.h
@brief Facebook的folly库中的hash实现。
@version 0.1
@date 2012-16-12
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxCommon.h"

CWINUX_BEGIN_NAMESPACE

/*
* Thomas Wang 64 bit mix hash function
*/

inline CWX_UINT64 cwx_fb_hash_twang_mix64(CWX_UINT64 key){
    key = (~key) + (key << 21);
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8);
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4);
    key = key ^ (key >> 28);
    key = key + (key << 31);
    return key;
}

/*
* Thomas Wang downscaling hash function
*/

inline CWX_UINT32 cwx_fb_hash_twang_32from64(CWX_UINT64 key) {
    key = (~key) + (key << 18);
    key = key ^ (key >> 31);
    key = key * 21;
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);
    return (CWX_UINT32) key;
}

/*
* Robert Jenkins' reversible 32 bit mix hash function
*/
inline CWX_UINT32 cwx_fb_hash_jenkins_rev_mix32(CWX_UINT32 key) {
    key += (key << 12);
    key ^= (key >> 22);
    key += (key << 4);
    key ^= (key >> 9);
    key += (key << 10);
    key ^= (key >> 2);
    key += (key << 7);
    key += (key << 12);
    return key;
}

/*
* Fowler / Noll / Vo (FNV) Hash
*     http://www.isthe.com/chongo/tech/comp/fnv/
*/

const CWX_UINT32 CWX_FB_HASH_FNV_32_HASH_START = 216613626UL;
const CWX_UINT64 CWX_FB_HASH_FNV_64_HASH_START = 14695981039346656037ULL;

inline CWX_UINT32 cwx_fb_hash_fnv32(const char* s, CWX_UINT32 hash = CWX_FB_HASH_FNV_32_HASH_START){
    for (; *s; ++s) {
        hash += (hash << 1) + (hash << 4) + (hash << 7) +
            (hash << 8) + (hash << 24);
        hash ^= *s;
    }
    return hash;
}

inline CWX_UINT32 cwx_fb_hash_fnv32_buf(const void* buf,
                          int n,
                          CWX_UINT32 hash = CWX_FB_HASH_FNV_32_HASH_START) 
{
    const char* char_buf = (const char*)(buf);
    for (int i = 0; i < n; ++i) {
        hash += (hash << 1) + (hash << 4) + (hash << 7) +
            (hash << 8) + (hash << 24);
        hash ^= char_buf[i];
    }

    return hash;
}

inline CWX_UINT64 cwx_fb_hash_fnv64(const char* s,
                      CWX_UINT64 hash = CWX_FB_HASH_FNV_64_HASH_START)
{
    for (; *s; ++s) {
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) +
            (hash << 8) + (hash << 40);
        hash ^= *s;
    }
    return hash;
}

inline CWX_UINT64 cwx_fb_hash_fnv64_buf(const void* buf,
                          int n,
                          CWX_UINT64 hash = CWX_FB_HASH_FNV_64_HASH_START)
{
    const char* char_buf = (const char*)(buf);
    for (int i = 0; i < n; ++i) {
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) +
            (hash << 8) + (hash << 40);
        hash ^= char_buf[i];
    }
    return hash;
}

/*
* Paul Hsieh: http://www.azillionmonkeys.com/qed/hash.html
*/

#define CWX_FB_HASH_GET_16_BITS(d) (*((const CWX_UINT16*) (d)))

inline CWX_UINT32 cwx_fb_hash_hsieh_hash32_buf(const void* buf, int len) {
    const char* s = (const char*)(buf);
    CWX_UINT32 hash = len;
    CWX_UINT32 tmp;
    int rem;

    if (len <= 0 || buf == 0) {
        return 0;
    }

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += CWX_FB_HASH_GET_16_BITS (s);
        tmp    = (CWX_FB_HASH_GET_16_BITS (s+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        s  += 2*sizeof (CWX_UINT16);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
  case 3:
      hash += CWX_FB_HASH_GET_16_BITS(s);
      hash ^= hash << 16;
      hash ^= s[sizeof (CWX_UINT16)] << 18;
      hash += hash >> 11;
      break;
  case 2:
      hash += CWX_FB_HASH_GET_16_BITS(s);
      hash ^= hash << 11;
      hash += hash >> 17;
      break;
  case 1:
      hash += *s;
      hash ^= hash << 10;
      hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
};

#undef CWX_FB_HASH_GET_16_BITS




/**
@class CwxFbHash
@brief Facebook的hash累
*/
class CWX_API CwxFbHash
{
private:
    ///构造函数
    CwxFbHash(){}
    ///析构函数
    ~CwxFbHash(){}
public:

    static CWX_UINT64 twangMix64(CWX_UINT64 key);

    /*
    * Thomas Wang downscaling hash function
    */
    static CWX_UINT32 twang32From64(CWX_UINT64 key);

    /*
    * Robert Jenkins' reversible 32 bit mix hash function
    */
    static CWX_UINT32 jenkinsRevMix32(CWX_UINT32 key);

    /*
    * Fowler / Noll / Vo (FNV) Hash
    *     http://www.isthe.com/chongo/tech/comp/fnv/
    */
    static CWX_UINT32 fnv32(const char* s, CWX_UINT32 hash = CWX_FB_HASH_FNV_32_HASH_START);

    static CWX_UINT32 fnv32(const void* buf,
        int n,
        CWX_UINT32 hash = CWX_FB_HASH_FNV_32_HASH_START);

    static CWX_UINT64 fnv64(const char* s,
        CWX_UINT64 hash = CWX_FB_HASH_FNV_64_HASH_START);

    static CWX_UINT64 fnv64(const void* buf,
        int n,
        CWX_UINT64 hash = CWX_FB_HASH_FNV_64_HASH_START);

    /*
    * Paul Hsieh: http://www.azillionmonkeys.com/qed/hash.html
    */
    static CWX_UINT32 hsiehHash32Buf(const void* buf, int len);

};

CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif
