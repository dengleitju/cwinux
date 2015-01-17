#ifndef __CWX_ENCODE_XML_H__
#define __CWX_ENCODE_XML_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
*@file  CwxEncodeXml.h
*@brief 将XML的特殊字符进行转移的CwxEncodeXml对象的定义
*@author cwinux@gmail.com
*@version 0.1
*@date  2009-10-18
*@warning  无.
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxReplMachine.h"

CWINUX_BEGIN_NAMESPACE

/**
* @class CwxEncodeXml
*
* @brief 将XML的特殊字符进行转移，默认行为如下：
  1、【<】-->【&lt;】
  2、【>】-->【&gt;】
  3、【&】-->【&amp;】
  4、【'】-->【&#39;】
  5、【1~31】中，除了\\t,\\r,\\n外，全部替换为空，即删除。

 */

class CWX_API CwxEncodeXml
{
public:
    ///构造函数，默认为非GBK（即为UTF-8），进行编码替换的时候，大小写敏感。
    CwxEncodeXml(bool bGbk=false, bool bCaseSensive=true);
    ///析构函数
    ~CwxEncodeXml();
public:
    /**
    @brief 初始化xml特殊字符的替换。
    @param [in] append 除了默认的，新增加的替换字符。
    @param [in] remove 移除默认中的替换字符。
    @return true:添加成功；false：字符串编码错误
    */
    bool init(map<string, string> const& append = emptyXml, map<string, string> const& remove = emptyXml);
    /**
    @brief 对XML中的特殊字符进行替换，编码后的字符串以0x00结束。
    @param [in] szIn 需要进行替换的输入字符串。
    @param [in,out] uiInLen 传入szIn的字符串的长度，返回成功执行编码的字符串长度，
                            即便在成功的情况下，由于空间不足而只完成了部分转码。
    @param [in] szOut 输出的置换后字符串的buf。
    @param [in,out] uiOutLen 传入szOut的空间大小，返回XML编码后的字符串的长度。
    @return true：成功；false：失败。失败的原因就是由于字符编码的错误。
    */
    bool encode(char const* szIn, CWX_UINT32& uiInLen, char* szOut, CWX_UINT32& uiOutLen) const;
    ///返回是否GBK
    bool isGbk() const;
    ///返回是否大小写敏感
    bool isCaseSensive() const;
private:
    ///添加编码转移字符串，返回false表示添加的源字符串的编码错误
    bool addStr(char const* src, char const* dst, map<string, string> const& append, map<string, string> const& remove);
private:
    static  map<string, string> const emptyXml;
    CwxReplBase*    m_pRepl;
    bool           m_bGbk;
    bool           m_bCaseSensive;
};



CWINUX_END_NAMESPACE

#include "CwxEncodeXml.inl"
#include "CwxPost.h"

#endif
