#ifndef __CWX_INI_PARSE_H__
#define __CWX_INI_PARSE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxIniParse.h
@brief 实现类似win.ini格式文件配置信息的加载。
@author cwinux@gmail.com
@version 0.1
@date 2011-09-13
@warning
@bug
*/
/***
1、文件以[xxxx]标示配置Section块，所有的key=value的配置，都必须位于某个[xxxx]的配置Section块下。
2、参数通关key=value的格式定义。其必须位于某个配置块下。
3、以#开头的行（无论全面都多少空格），认为是注释行，会被忽略。
4、空行被忽略。
5、key=value中，key的前置、后置空格会被忽略。key不能包含[=]；
   value的前置、后置空格会被忽略（\r与\n也会被忽略）
***/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"
#include "CwxFile.h"
#include "CwxCommon.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxIniParse
@brief win.ini格式的配置文件信息的解析与加载。
*/
class CWX_API CwxIniParse
{
public:
    ///构造函数，参数为XML的encode替换器，有外部进行管理
    CwxIniParse(){
        memset(m_szErrMsg, 0x00, 512);
    }
    ///析构函数
    ~CwxIniParse(){
		map<string, list<pair<string, string> >*>::iterator iter = m_attrs.begin();
		while(iter != m_attrs.end()){
			delete iter->second;
			iter++;
		}
	}
public:
    /**
    *@brief  加载配置文件.
    *@param [in] strFile 配置文件名。
    *@return false：失败；true：成功.
    */ 
    bool load(string const& strFile);
    /**
    *@brief  解析配置.
    *@param [in] strConf 配置信息。
    *@return false：失败；true：成功.
    */
    bool parse(string const& strConf);
    /**
    *@brief  获取配置中所有的Section。
    *@param [out] sections 配置中的section。
    *@return void.
    */ 
    void getSections(set<string>& sections) const;
    /**
    *@brief  获取一个section下的所有属性
    *@param [in] strSection section的名字。
    *@param [out] attr下的所有属性及其value。
    *@return false：section不存在；true：section存在.
    */ 
    bool getAttr(string const& strSection,
		list<pair<string, string> >& attr) const;
    /**
    *@brief  获取一个字符串型属性值.
	*@param [in] strSection section的名字。
	*@param [in] strAttr 属性名。
    *@param [out] strValue 属性值。
    *@return false：不存在；true：存在.
    */ 
    bool getAttr(string const& strSection,
        string const& strAttr,
		string& strValue) const;
	/**
	*@brief  是否存在指定的Section。
	*@param [int] strSection section的名字。
	*@return true：存在；false：不存在.
	*/ 
	bool isExistSection(string const& strSection){
		return m_attrs.find(strSection) != m_attrs.end();
	}

    ///返回错误信息
    char const* getErrMsg() const{ return m_szErrMsg;}
private:
	string							   m_strFile; ///<配置文件
	map<string, list<pair<string, string> >*>  m_attrs; ///<配置属性
    char m_szErrMsg[512];///<错误buf
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif
