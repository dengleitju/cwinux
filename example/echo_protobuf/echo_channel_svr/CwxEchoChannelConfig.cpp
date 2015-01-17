#include "CwxEchoChannelConfig.h"

int CwxEchoChannelConfig::loadConfig(string const & strConfFile){
    CwxIniParse parser;
    string value;
    //解析配置文件
    if (false == parser.load(strConfFile)){
		snprintf(m_szError, 2047, "Failure to Load conf file:%s", strConfFile.c_str());
        return -1;
    }
    //load workdir
    if (!parser.getAttr("channel_svr", "workdir", value)|| !value.length()){
        snprintf(m_szError, 2047, "Must set [channel_svr:workdir].");
        return -1;
    }
	if ('/' != value[value.length()-1]) value +="/";
    m_strWorkDir = value;

    //load unix
    if (!parser.getAttr("channel_svr", "unix", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [channel_svr:unix].");
        return -1;
    }
    m_strUnixPathFile = value;

    // load query thread num
    if (!parser.getAttr("channel_svr", "thread_num", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [channel_svr:thread_num].");
        return -1;
    }
    m_unThreadNum = strtoul(value.c_str(), NULL, 10);

    //load listen
    if (!parser.getAttr("channel_svr", "listen", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [channel_svr:listen].");
        return -1;
    }
    m_listen.setHostName(value);
    if (!parser.getAttr("channel_svr", "port", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [channel_svr:port].");
        return -1;
    }
    m_listen.setPort(strtoul(value.c_str(), NULL, 10));
    return 0;
}

void CwxEchoChannelConfig::outputConfig(string & strConfig){
    char szBuf[32];
	strConfig.clear();	
	strConfig += "*****************BEGIN CONFIG *******************";
    strConfig += "\nworkdir= " ;
    strConfig += m_strWorkDir;
    strConfig += "\nthread_num=";
    sprintf(szBuf, "%u", m_unThreadNum);
    strConfig += szBuf;
	strConfig += "\nlisten: ip=";
    strConfig += m_listen.getHostName();
    strConfig += " port=";
    sprintf(szBuf, "%u", m_listen.getPort());
    strConfig += szBuf;   		
    strConfig += "\n*****************END   CONFIG *******************\n";   
}
