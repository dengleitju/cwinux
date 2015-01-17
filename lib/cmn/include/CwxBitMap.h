#ifndef __CWX_BIT_MAP_H__
#define __CWX_BIT_MAP_H__

/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxBitMap.h
@brief 定义了CwxBitMap类，其功能是实现最大2^32个位的设置、检查操作
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxBitMap
@brief 对最多2^32个位进行设置、检查的操作，所有操作为静态的inline操作。
*/
class CWX_API CwxBitMap
{
public:
    /**
    @brief 检查bitMap指向的内存中，第bit位是否设置为1。
    @param [in] bitMap 要检查的位所在的内存
    @param [in] bit 要检查的位
    @return true：设置为1；false：没有设置为1
    */
    inline static bool isEnable(void* bitMap, CWX_UINT32 bit)
    {
        return ((((char*)bitMap)[bit>>3]) & (1<<(bit&0x07))) != 0;
    }
    /**
    @brief 将bitMap指向的内存的第bit位设置为1。
    @param [in] bitMap 要设置的位所在的内存
    @param [in] bit 要设置的位
    @return void
    */
    inline static void setBit(void* bitMap, CWX_UINT32 bit)
    {
        (((char*)bitMap)[bit>>3]) |= (1<<(bit&0x07));
    }
    /**
    @brief 将bitMap指向的内存的第bit位清空为0。
    @param [in] bitMap 要设置的位所在的内存
    @param [in] bit 要清空的位
    @return void
    */
    inline static void clrBit(void* bitMap, CWX_UINT32 bit)
    {
        (((char*)bitMap)[bit>>3]) &=~(1<<(bit&0x07));
    }
private:
    ///私有构造函数，禁止实例化
    CwxBitMap(){}
    ///私有析构函数
    ~CwxBitMap(){}
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif
