#include "CwxEchoClientConfig.h"

int CwxEchoClientConfig::loadConfig(string const & strConfFile){
    CwxIniParse parser;
    string value;
    //解析配置文件
    if (false == parser.load(strConfFile)){
		snprintf(m_szError, 2047, "Failure to Load conf file:%s", strConfFile.c_str());
        return -1;
    }
    //load workdir
    if (!parser.getAttr("client", "workdir", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [client:workdir].");
        return -1;
    }
	if ('/' != value[value.length()-1]) value +="/";
    m_strWorkDir = value;

    // load connect num
    if (!parser.getAttr("client", "conn_num", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [client:conn_num].");
        return -1;
    }
    m_unConnNum = strtoul(value.c_str(), NULL, 10);
    // load query conn type
    if (!parser.getAttr("client", "conn_type", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [client:conn_type].");
        return -1;
    }
    m_bTcp = strcasecmp("tcp", value.c_str())==0?true:false;

    // load query conn lasting
    if (!parser.getAttr("client", "conn_lasting", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [client:conn_lasting].");
        return -1;
    }
    m_bLasting = strcasecmp("1", value.c_str())==0?true:false;

    // data size
    if (!parser.getAttr("client", "data_size", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [client:data_size].");
        return -1;
    }
    m_uiDataSize =strtoul(value.c_str(), NULL, 10);
    //load listen
    if (!parser.getAttr("client", "listen", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [client:listen].");
        return -1;
    }
    m_listen.setHostName(value.c_str());
    if (!parser.getAttr("client", "port", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [client:port].");
        return -1;
    }
    m_listen.setPort(strtoul(value.c_str(), NULL, 10));

    //load svr_def:unix{path}
    if (!parser.getAttr("client","unix", value) || !value.length()){
        snprintf(m_szError, 2047, "Must set [client:unix].");
        return -1;
    }
    m_strUnixPathFile = value;

    return 0;
}

void CwxEchoClientConfig::outputConfig(string & strConfig){
    char szBuf[32];
	strConfig.clear();	
	strConfig += "*****************BEGIN CONFIG *******************";
    strConfig += "\nworkdir= " ;
    strConfig += m_strWorkDir;
    strConfig += "\nconn_num=";
    sprintf(szBuf, "%u", m_unConnNum);
    strConfig += szBuf;
	strConfig += "\nlisten: ip=";
    strConfig += m_listen.getHostName();
    strConfig += " port=";
    sprintf(szBuf, "%u", m_listen.getPort());
    strConfig += szBuf;   		
    strConfig += "\n*****************END   CONFIG *******************\n";   
}
