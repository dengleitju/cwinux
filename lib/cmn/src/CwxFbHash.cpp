#include  "CwxFbHash.h"

CWINUX_BEGIN_NAMESPACE

CWX_UINT64 CwxFbHash::twangMix64(CWX_UINT64 key){
    return cwx_fb_hash_twang_mix64(key);
}

/*
* Thomas Wang downscaling hash function
*/
CWX_UINT32 CwxFbHash::twang32From64(CWX_UINT64 key) {
    return cwx_fb_hash_twang_32from64(key);
}

/*
* Robert Jenkins' reversible 32 bit mix hash function
*/
CWX_UINT32 CwxFbHash::jenkinsRevMix32(CWX_UINT32 key) {
    return cwx_fb_hash_jenkins_rev_mix32(key);
}

/*
* Fowler / Noll / Vo (FNV) Hash
*     http://www.isthe.com/chongo/tech/comp/fnv/
*/
CWX_UINT32 CwxFbHash::fnv32(const char* s, CWX_UINT32 hash){
    return cwx_fb_hash_fnv32(s, hash);
}

CWX_UINT32 CwxFbHash::fnv32(const void* buf,
                        int n,
                        CWX_UINT32 hash) 
{
    return cwx_fb_hash_fnv32_buf(buf, n, hash);
}

CWX_UINT64 CwxFbHash::fnv64(const char* s,
                        CWX_UINT64 hash)
{
    return cwx_fb_hash_fnv64(s, hash);
}

CWX_UINT64 CwxFbHash::fnv64(const void* buf,
                        int n,
                        CWX_UINT64 hash)
{
    return cwx_fb_hash_fnv64_buf(buf, n, hash);
}

/*
* Paul Hsieh: http://www.azillionmonkeys.com/qed/hash.html
*/
CWX_UINT32 CwxFbHash::hsiehHash32Buf(const void* buf, int len) {
    return cwx_fb_hash_hsieh_hash32_buf(buf, len);
};


CWINUX_END_NAMESPACE


