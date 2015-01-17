#include "cwx_config.h"
#include "cwx_stream.h"
#include "cwx_msg_header.h"
#include "cwx_package.h"

/*
 * Apache magic module declaration.
 */
module AP_MODULE_DECLARE_DATA cwinux_module;

void cwx_log(char *fmt, ...){
    va_list args;
    char timestamp[32];
    char buf[2048];
    time_t t;
    int pid;
    pid = (int) getpid();
    time(&t);
    strftime(timestamp, sizeof(timestamp), "[%m/%d/%Y:%H:%M:%S %z]", localtime(&t));
#if APR_HAS_THREADS
    snprintf(buf, 2047, "%s %d_%ld: ", timestamp, pid, (long int) apr_os_thread_current());
#else
    snprintf(buf, 2047, "%s %d: ", timestamp, pid);
#endif
    va_start(args, fmt);
    vsnprintf(buf + strlen(buf), 2047 - strlen(buf), fmt, args);
    va_end(args);
    fputs(buf, stderr);
    fflush(stderr);
}

static apr_thread_mutex_t * cwx_create_lock(apr_pool_t *p){
    apr_thread_mutex_t *lock = 0;
    apr_thread_mutex_create(&lock, APR_THREAD_MUTEX_DEFAULT, p);
    return lock;
}

static void cwx_trim(char* value){
    size_t len = strlen(value);
    size_t i;
    for (i=len; i>0; i--){//trim right
        if (value[i-1] != ' ') break;
        value[i-1] = 0x00;
    }
    len = strlen(value);
    for (i=0; i<len; i++) if (value[i] != ' ') break;
    if (i) memmove(value, value+i, len - i);
}

static void cwx_tolower(char* szValue){
    while(*szValue){
        *szValue = apr_tolower(*szValue);
        szValue++;
    }
}

static int cwx_lock(apr_thread_mutex_t *lock){
    if (lock) apr_thread_mutex_lock(lock);
    return 1;
}

static void cwx_unlock(apr_thread_mutex_t *lock){
    if (lock) apr_thread_mutex_unlock(lock);
}

static void cwx_reset_user_config(struct CWX_USER_CONFIG* pConfig){
    pConfig->m_show = -1;
    pConfig->m_delay = -1;
    pConfig->m_persistant = -1;
    pConfig->m_query_timeout = -1;
    pConfig->m_conn_timeout = -1;
    pConfig->m_reply_timeout = -1;
    pConfig->m_restore_timeout = -1;
    pConfig->m_min_idle_conn = -1;
    pConfig->m_max_idle_conn = -1;

}

static void cwx_merge_def_user_config(struct CWX_USER_CONFIG* pConfig, struct CWX_USER_CONFIG const* pMerge){
    if (-1 == pConfig->m_show) pConfig->m_show = (pMerge->m_show != -1)?pMerge->m_show:CWX_DEF_SHOW;
    if (-1 == pConfig->m_delay) pConfig->m_delay = (pMerge->m_delay != -1)?pMerge->m_delay:CWX_DEF_DELAY;
    if (-1 == pConfig->m_persistant) pConfig->m_persistant = (pMerge->m_persistant != -1)?pMerge->m_persistant:CWX_DEF_PERSISTANT;
    if (-1 == pConfig->m_query_timeout) pConfig->m_query_timeout = (pMerge->m_query_timeout != -1)?pMerge->m_query_timeout:CWX_DEF_QUERY_TIMEOUT;
    if (-1 == pConfig->m_conn_timeout) pConfig->m_conn_timeout = (pMerge->m_conn_timeout != -1)?pMerge->m_conn_timeout:CWX_DEF_CONN_TIMEOUT;
    if (-1 == pConfig->m_reply_timeout) pConfig->m_reply_timeout = (pMerge->m_reply_timeout != -1)?pMerge->m_reply_timeout:CWX_DEF_REPLY_TIMEOUT;
    if (-1 == pConfig->m_restore_timeout) pConfig->m_restore_timeout = (pMerge->m_restore_timeout != -1)?pMerge->m_restore_timeout:CWX_DEF_RESTORE_TIMEOUT;
    if (-1 == pConfig->m_min_idle_conn) pConfig->m_min_idle_conn = (pMerge->m_min_idle_conn != -1)?pMerge->m_min_idle_conn:CWX_DEF_MIN_IDLE_CONN;
    if (-1 == pConfig->m_max_idle_conn) pConfig->m_max_idle_conn = (pMerge->m_max_idle_conn != -1)?pMerge->m_max_idle_conn:CWX_DEF_MAX_IDLE_CONN;
}

static void cwx_init_def_user_config(struct CWX_USER_CONFIG* pConfig){
    pConfig->m_show = CWX_DEF_SHOW;
    pConfig->m_delay = CWX_DEF_DELAY;
    pConfig->m_persistant = CWX_DEF_PERSISTANT;
    pConfig->m_query_timeout = CWX_DEF_QUERY_TIMEOUT;
    pConfig->m_conn_timeout = CWX_DEF_CONN_TIMEOUT;
    pConfig->m_reply_timeout = CWX_DEF_REPLY_TIMEOUT;
    pConfig->m_restore_timeout = CWX_DEF_RESTORE_TIMEOUT;
    pConfig->m_min_idle_conn = CWX_DEF_MIN_IDLE_CONN;
    pConfig->m_max_idle_conn = CWX_DEF_MAX_IDLE_CONN;
}

static bool cwx_set_user_config(struct CWX_USER_CONFIG* pConfig, char const* szValue){
    char const* name_end = strchr(szValue, ':');
    char* value_start = NULL;
    unsigned int  name_len = 0;
    if (!name_end || (szValue == name_end)) return false;
    value_start = name_end + 1;
    name_len = name_end - szValue;
    if ((name_len == strlen("show")) && (strncasecmp(szValue, "show", name_len)==0)){
        if (strcasecmp("true", value_start) == 0){
            pConfig->m_show = 1;
        }else{
            pConfig->m_show = 0;
        }
        return true;
    }else if ((name_len == strlen("delay")) && (strncasecmp(szValue, "delay", name_len)==0)){
        if (strcasecmp("true", value_start) == 0){
            pConfig->m_delay = 1;
        }else{
            pConfig->m_delay = 0;
        }
        return true;
    }else if ((name_len == strlen("persistent")) && (strncasecmp(szValue, "persistent", name_len)==0)){
        if (strcasecmp("true", value_start) == 0){
            pConfig->m_persistant = 1;
        }else{
            pConfig->m_persistant = 0;
        }
        return true;
    }else if ((name_len == strlen("query")) && (strncasecmp(szValue, "query", name_len)==0)){
        if (atoi(value_start) > 0){
            pConfig->m_query_timeout = atoi(value_start);
            return true;
        }
    }else if ((name_len == strlen("connect")) && (strncasecmp(szValue, "connect", name_len)==0)){
        if (atoi(value_start) > 0){
            pConfig->m_conn_timeout = atoi(value_start);
            return true;
        }
    }else if ((name_len == strlen("reply")) && (strncasecmp(szValue, "reply", name_len)==0)){
        if (atoi(value_start) > 0){
            pConfig->m_reply_timeout = atoi(value_start);
            return true;
        }
    }else if ((name_len == strlen("restore")) && (strncasecmp(szValue, "restore", name_len)==0)){
        if (atoi(value_start) > 0){
            pConfig->m_restore_timeout = atoi(value_start);
            return true;
        }
    }else if ((name_len == strlen("min-idle")) && (strncasecmp(szValue, "min-idle", name_len)==0)){
        if (atoi(value_start) > 0){
            pConfig->m_min_idle_conn = atoi(value_start);
            return true;
        }
    }else if ((name_len == strlen("max-idle")) && (strncasecmp(szValue, "max-idle", name_len)==0)){
        if (atoi(value_start) > 0){
            pConfig->m_max_idle_conn = atoi(value_start);
            return true;
        }
    }
    return false;
}

static void * cwx_create_server_config(apr_pool_t *p, server_rec *s){
    CWX_CONFIG*  config = (CWX_CONFIG *) apr_pcalloc(p, sizeof(CWX_CONFIG));

    config->m_pool = p;
    config->m_lock = cwx_create_lock(config->m_pool);
    config->m_svr_num = 0;
    cwx_init_def_user_config(&config->m_user_config);
    config->m_key_item.m_bkey_hash = NULL;
    config->m_key_item.m_ex_bkey_hash = NULL;
    config->m_key_item.m_header = NULL;
    config->m_svr_hash = apr_hash_make(config->m_pool);
    config->m_hosts = NULL;
    config->m_host_num = 0;
    config->m_balance = NULL;
    config->m_header = NULL;
    config->m_init = false;
    return (void *) config;
}

static void * cwx_merge_server_config(apr_pool_t *p, void *basev, void *overridesv){
    CWX_CONFIG *base = (CWX_CONFIG *) basev;
    CWX_CONFIG *overrides = (CWX_CONFIG *) overridesv;
    if (!overrides || !overrides->m_hosts) return base;
    return overrides;
}

static CWX_CONFIG* cwx_get_server_config(server_rec *s){
    return (CWX_CONFIG *) ap_get_module_config(s->module_config, &cwinux_module);
}

///cwinux-service svr_name [show:true/false] [delay:true/false] [persistent:true/false] [query:timeout]  [connect:timeout]  [reply:timeout]  [restore:timeout] [min-idle:value] [max-idle:value]
static const char* cwx_svr_command(cmd_parms *cmd, void *pconfig,  char* arg){
    char szBuf[512];
    const char* line = arg;
    char* value = NULL;
    CWX_SERVICE* svr = NULL;
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config)  return 0;
    if (NULL == arg) return "must set cwinux-service's arg: service-name";
    //get svr_name
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "Must set service's name by format: svr_name";
    }
    if (apr_hash_get(config->m_svr_hash, value, APR_HASH_KEY_STRING)){//exist
        snprintf(szBuf, 511, "svr name[%s] is duplicationg.", value);
        return apr_pstrdup(config->m_pool, szBuf);
    }
    svr = (CWX_SERVICE *) apr_pcalloc(config->m_pool, sizeof(CWX_SERVICE));
    svr->m_svr_name = value;
    svr->m_config = config;
    svr->m_hosts = NULL;
    svr->m_host_num = 0;
    svr->m_key_item.m_bkey_hash = NULL;
    svr->m_key_item.m_ex_bkey_hash = NULL;
    svr->m_key_item.m_header = NULL;
    svr->m_uri_key_hash = NULL;
    cwx_reset_user_config(&svr->m_user_config);
    value = ap_getword_conf(config->m_pool, &line);
    while(value && strlen(value)){
        if (!cwx_set_user_config(&svr->m_user_config, value)){
            snprintf(szBuf, 511, "Invalid cwinux-service arg:%s", value);
            return apr_pstrdup(config->m_pool, szBuf);
        }
        value = ap_getword_conf(config->m_pool, &line);
    }
    apr_hash_set(config->m_svr_hash, svr->m_svr_name, APR_HASH_KEY_STRING, svr);
    config->m_svr_num++;
    return 0;
}

static const char* cwx_host_command(cmd_parms *cmd, void *pconfig,  char* arg){
    const char* line;
    struct hostent *hostent = 0;
    char* value= NULL;
    CWX_HOST* host=NULL;
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (! config) return 0;
    if (NULL == arg)  return "must set cwinux-host's arg: type_name seq ip port";
    host = (CWX_HOST *) apr_pcalloc(config->m_pool, sizeof(CWX_HOST));
    host->m_config = config;
    memset(host, 0x00, sizeof(CWX_HOST));
    line = arg;
    //get svr_name
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "must set host's svr_name by format: svr_name seq ip port";
    }
    host->m_svr_name = value;
    //get seq
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "must set host's seq by format: svr_name seq ip port";
    }
    host->m_index = strtol(value, NULL, 0);
    //get ip
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "must set host's ip by format: svr_name seq ip port";
    }
    host->m_ip = value;
    //get port
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "must set host's port by format: svr_name seq ip port";
    }
    host->m_port = strtol(value, NULL, 0);
    cwx_reset_user_config(&host->m_user_config);
    host->m_is_valid = true;
    host->m_lock = cwx_create_lock(config->m_pool);
    host->m_idle_socket_head = NULL;
    host->m_idle_socket_num = 0;
    host->m_used_socket_num = 0;
    host->m_service = NULL;
    host->m_config = config;
    host->m_next = config->m_hosts;
    config->m_hosts = host;
    config->m_host_num++;
    hostent = gethostbyname(host->m_ip);
    if (hostent && hostent->h_addr) {
        memcpy(&host->m_host, hostent->h_addr, sizeof(struct in_addr));
    }else{
        char msg[512];
        sprintf(msg, "Failure to get host[%s]'s address information", host->m_ip);
        return apr_pstrdup(config->m_pool, msg);
    }

    value = ap_getword_conf(config->m_pool, &line);
    while(value && strlen(value)){
        if (!cwx_set_user_config(&host->m_user_config, value)){
            char msg[512];
            snprintf(msg, 511, "Invalid cwinux-service arg:%s", value);
            return apr_pstrdup(config->m_pool, msg);
        }
        value = ap_getword_conf(config->m_pool, &line);
    }

    return 0;
}

static const char* cwx_balance_command(cmd_parms *cmd, void *pconfig,  char* arg){
    const char* line;
    char* value= NULL;
    CWX_SVR_MSG_KEY* svr_msg=NULL;
    CWX_KEY_VALUE*  key_value=NULL;
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config) return 0;
    if (NULL == arg)  return "must set cwinux-balance's arg: svr_name msg_type [-] key...";
    svr_msg = (CWX_SVR_MSG_KEY *) apr_pcalloc(config->m_pool, sizeof(CWX_SVR_MSG_KEY));
    line = arg;
    //get svr_name
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "must set balance's svr_name by format: svr_name msg_type [-] key...";
    }
    svr_msg->m_svr_name = value;
    //get msg_type
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "must set balance's msg_type by format: svr_name msg_type [-] key...";
    }
    svr_msg->m_msg_type = value;
    //get [-]
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "must set balance's [-] or key by format: svr_name msg_type [-] key...";
    }
    if ('-' == value[0]){
        svr_msg->m_exclude = true;
        value = ap_getword_conf(config->m_pool, &line);
        if (!value || !strlen(value)){
            return "must set balance's key by format: svr_name msg_type [-] key...";
        }
    }else{
        svr_msg->m_exclude = false;
    }
    svr_msg->m_key_value = NULL;

    while(value && strlen(value)){
        key_value = (CWX_KEY_VALUE*) apr_pcalloc(config->m_pool, sizeof(CWX_KEY_VALUE));
        key_value->m_key = value;
        cwx_tolower(key_value->m_key);
        key_value->m_lowerkey = key_value->m_key;
        key_value->m_value = NULL;
        key_value->m_next = svr_msg->m_key_value;
        svr_msg->m_key_value = key_value;
        value = ap_getword_conf(config->m_pool, &line);
    }
    svr_msg->m_next = config->m_balance;
    config->m_balance = svr_msg;
    return 0;
}

static const char* cwx_header_command(cmd_parms *cmd, void *pconfig,  char* arg){
    char szMsg[512];
    const char* line;
    char* value= NULL;
    CWX_SVR_MSG_KEY* svr_msg=NULL;
    CWX_KEY_VALUE*  key_value=NULL;
    CWX_CONFIG *config = cwx_get_server_config(cmd->server);
    char * head_data = NULL;
    if (!config) return 0;
    if (NULL == arg)  return "must set cwinux-header's arg: svr_name msg_type header:value header:value ...";
    svr_msg = (CWX_SVR_MSG_KEY *) apr_pcalloc(config->m_pool, sizeof(CWX_SVR_MSG_KEY));
    line = arg;
    //get svr_name
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "must set cwinux-header's svr_name: svr_name msg_type header:value header:value ...";
    }
    svr_msg->m_svr_name = value;
    //get msg_type
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "must set cwinux-header's msg_type: svr_name msg_type header:value header:value ...";
    }
    svr_msg->m_msg_type = value;
    value = ap_getword_conf(config->m_pool, &line);
    if (!value || !strlen(value)){
        return "must set cwinux-header's header:value: svr_name msg_type header:value header:value ...";
    }
    svr_msg->m_key_value = NULL;

    while(value && strlen(value)){
        head_data = (char*)strchr(value, ':');
        if (!head_data || (head_data==value)){
            snprintf(szMsg, 511, "Invalid cwinux-header's header:data's format:%s", value);
            return apr_pstrdup(config->m_pool, szMsg);
        }
        head_data[0] = 0x00;
        head_data++;
        key_value = (CWX_KEY_VALUE*) apr_pcalloc(config->m_pool, sizeof(CWX_KEY_VALUE));
        key_value->m_key = value;
        key_value->m_value = head_data;
        key_value->m_next = svr_msg->m_key_value;
        svr_msg->m_key_value = key_value;
        value = ap_getword_conf(config->m_pool, &line);
    }
    svr_msg->m_next = config->m_header;
    config->m_header = svr_msg;
    return 0;
}

static const char* cwx_show_command(cmd_parms *cmd,  void *pconfig, char * def){
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config)  return 0;
    config->m_user_config.m_show = def?(strcmp(def, "true")==0?1:0):0;
    return 0;
}

static const char* cwx_delay_command(cmd_parms *cmd,  void *pconfig, char * def){
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config)  return 0;
    config->m_user_config.m_delay = def?(strcmp(def, "true")==0?1:0):1;
    return 0;
}

static const char* cwx_persistent_command(cmd_parms *cmd,  void *pconfig, char * def){
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config)  return 0;
    config->m_user_config.m_persistant = def?(strcmp(def, "true")==0?1:0):1;
    return 0;
}
static const char* cwx_query_timeout_command(cmd_parms *cmd,  void *pconfig, char * def){
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config)  return 0;
    if (atoi(def)>0) config->m_user_config.m_query_timeout = atoi(def);
    return 0;
}
static const char* cwx_conn_timeout_command(cmd_parms *cmd,  void *pconfig, char * def){
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config)  return 0;
    if (atoi(def)>0) config->m_user_config.m_conn_timeout = atoi(def);
    return 0;
}
static const char* cwx_reply_timeout_command(cmd_parms *cmd,  void *pconfig, char * def){
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config)  return 0;
    if (atoi(def)>0) config->m_user_config.m_reply_timeout = atoi(def);
    return 0;
}
static const char* cwx_restore_time_command(cmd_parms *cmd,  void *pconfig, char * def){
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config)  return 0;
    if (atoi(def)>0) config->m_user_config.m_restore_timeout = atoi(def);
    return 0;
}
static const char* cwx_min_idle_conn_command(cmd_parms *cmd,  void *pconfig, char * def){
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config)  return 0;
    if (atoi(def)>0) config->m_user_config.m_min_idle_conn = atoi(def);
    return 0;
}
static const char* cwx_max_idle_conn_command(cmd_parms *cmd,  void *pconfig, char * def){
    CWX_CONFIG *config = cwx_get_server_config(cmd->server); 
    if (!config)  return 0;
    if (atoi(def)>0) config->m_user_config.m_max_idle_conn = atoi(def);
    return 0;
}

static int  cwx_host_sort_func(const void *item1, const void *item2){
    CWX_HOST** host1 = (CWX_HOST**) item1;
    CWX_HOST** host2 = (CWX_HOST**) item2;
    return (*host1)->m_index - (*host2)->m_index;
}

static void cwx_init_run_conf(CWX_CONFIG *config){
    apr_hash_index_t *hi = NULL;
    void *val = NULL;
    CWX_SERVICE* svr=NULL;
    int i = 0;
    int j = 0;
    CWX_KEY_ITEM*   key_item=NULL;
    CWX_HOST* host = NULL;
    CWX_SVR_MSG_KEY*   balance=NULL;
    CWX_KEY_VALUE*     key_value=NULL;
    CWX_SVR_MSG_KEY*   header = NULL;
    apr_hash_t*        hash_obj = NULL;
    //init sui
    cwx_lock(config->m_lock);//get lock
    if (config->m_init){
        cwx_unlock(config->m_lock);
        return;
    }
    ///init service's config and host
    for (hi = apr_hash_first(config->m_pool, config->m_svr_hash); hi; hi = apr_hash_next(hi)) {
        apr_hash_this(hi, NULL, NULL, &val);
        svr = (CWX_SERVICE *)val;
        cwx_merge_def_user_config(&svr->m_user_config, &config->m_user_config);
        svr->m_host_num = 0;
        host = config->m_hosts;
        while(host){
            if (strcmp(host->m_svr_name, svr->m_svr_name) == 0){
                svr->m_host_num++;
            }
            host=host->m_next;
        }
        if (svr->m_host_num > 0){
            svr->m_hosts = (CWX_HOST**)apr_pcalloc(config->m_pool, sizeof(CWX_HOST*) * svr->m_host_num);
            svr->m_host_num = 0;
            host = config->m_hosts;
            while(host){
                if (strcmp(host->m_svr_name, svr->m_svr_name) == 0){
                    svr->m_hosts[svr->m_host_num] = host;
                    host->m_service = svr;
                    svr->m_host_num++;
                    cwx_merge_def_user_config(&host->m_user_config, &svr->m_user_config);
                }
                host = host->m_next;
            }
            ///sort host by index
            qsort(svr->m_hosts, svr->m_host_num, sizeof(CWX_HOST*), cwx_host_sort_func);
        }else{
            svr->m_hosts = NULL;
        }        
    }
    ///init banlance
    balance = config->m_balance;
    while(balance){
        hash_obj = NULL;
        if ((strcmp("*", balance->m_svr_name) == 0) && (strcmp("*", balance->m_msg_type)==0)){//global
            if (balance->m_exclude && !config->m_key_item.m_ex_bkey_hash){
                config->m_key_item.m_ex_bkey_hash = apr_hash_make(config->m_pool);
                hash_obj = config->m_key_item.m_ex_bkey_hash;
            }else if (!balance->m_exclude && !config->m_key_item.m_bkey_hash){
                config->m_key_item.m_bkey_hash = apr_hash_make(config->m_pool);
                hash_obj = config->m_key_item.m_bkey_hash;
            }
        }else if ((strcmp("*", balance->m_svr_name) != 0) && (strcmp("*", balance->m_msg_type)==0)){//svr def
            svr = (CWX_SERVICE*)apr_hash_get(config->m_svr_hash, balance->m_svr_name, APR_HASH_KEY_STRING);
            if (svr){
                if (balance->m_exclude && !svr->m_key_item.m_ex_bkey_hash){
                    svr->m_key_item.m_ex_bkey_hash = apr_hash_make(config->m_pool);
                    hash_obj = svr->m_key_item.m_ex_bkey_hash;
                }else if (!balance->m_exclude && !svr->m_key_item.m_bkey_hash){
                    svr->m_key_item.m_bkey_hash = apr_hash_make(config->m_pool);
                    hash_obj = svr->m_key_item.m_bkey_hash;
                }
            }
        }else if ((strcmp("*", balance->m_svr_name) != 0) && (strcmp("*", balance->m_msg_type)!=0)){//svr msg
            svr = (CWX_SERVICE*)apr_hash_get(config->m_svr_hash, balance->m_svr_name, APR_HASH_KEY_STRING);
            if (svr){
                if (!svr->m_uri_key_hash){
                    svr->m_uri_key_hash = apr_hash_make(config->m_pool);
                    key_item = NULL;
                }else{
                    key_item = (CWX_KEY_ITEM*)apr_hash_get(svr->m_uri_key_hash, balance->m_msg_type, APR_HASH_KEY_STRING);
                }
                if (!key_item){
                    key_item = (CWX_KEY_ITEM*)apr_pcalloc(config->m_pool, sizeof(CWX_KEY_ITEM));
                    key_item->m_bkey_hash = NULL;
                    key_item->m_ex_bkey_hash = NULL;
                    key_item->m_header = NULL;
                    apr_hash_set(svr->m_uri_key_hash, balance->m_msg_type, APR_HASH_KEY_STRING, key_item);
                }
                if (balance->m_exclude && !key_item->m_ex_bkey_hash){
                    key_item->m_ex_bkey_hash = apr_hash_make(config->m_pool);
                    hash_obj = key_item->m_ex_bkey_hash;
                }else if (!balance->m_exclude && !key_item->m_bkey_hash){
                    key_item->m_bkey_hash = apr_hash_make(config->m_pool);
                    hash_obj = key_item->m_bkey_hash;
                }
            }
        }
        if (hash_obj){
            key_value = balance->m_key_value;
            while(key_value){
                apr_hash_set(hash_obj, key_value->m_key, APR_HASH_KEY_STRING, key_value);
                key_value = key_value->m_next;
            }
        }
        balance = balance->m_next;
    }

    ///init header
    header = config->m_header;
    while(header){
        hash_obj = NULL;
        if ((strcmp("*", header->m_svr_name) == 0) && (strcmp("*", header->m_msg_type)==0)){//global
            config->m_key_item.m_header =  header->m_key_value;
        }else if ((strcmp("*", header->m_svr_name) != 0) && (strcmp("*", header->m_msg_type)==0)){//svr def
            svr = (CWX_SERVICE*)apr_hash_get(config->m_svr_hash, header->m_svr_name, APR_HASH_KEY_STRING);
            if (svr) svr->m_key_item.m_header = header->m_key_value;
        }else if ((strcmp("*", header->m_svr_name) != 0) && (strcmp("*", header->m_msg_type)!=0)){//svr msg
            svr = (CWX_SERVICE*)apr_hash_get(config->m_svr_hash, header->m_svr_name, APR_HASH_KEY_STRING);
            if (svr){
                if (!svr->m_uri_key_hash){
                    svr->m_uri_key_hash = apr_hash_make(config->m_pool);
                    key_item = NULL;
                }else{
                    key_item = (CWX_KEY_ITEM*)apr_hash_get(svr->m_uri_key_hash, header->m_msg_type, APR_HASH_KEY_STRING);
                }
                if (!key_item){
                    key_item = (CWX_KEY_ITEM*)apr_pcalloc(config->m_pool, sizeof(CWX_KEY_ITEM));
                    key_item->m_bkey_hash = NULL;
                    key_item->m_ex_bkey_hash = NULL;
                    key_item->m_header = NULL;
                    apr_hash_set(svr->m_uri_key_hash, header->m_msg_type, APR_HASH_KEY_STRING, key_item);
                }
                key_item->m_header = header->m_key_value;
            }
        }
        header = header->m_next;
    }

    config->m_init = true;
    cwx_unlock(config->m_lock);
}

static void cwx_fetch_conn_user_config(CWX_CONNECT* conn){
    //fetch url's config
    CWX_KEY_ITEM* key_item = NULL;
    if (conn->m_service->m_uri_key_hash){
        key_item = (CWX_KEY_ITEM*)apr_hash_get(conn->m_service->m_uri_key_hash, conn->m_msg_type, APR_HASH_KEY_STRING);
    }
    if (key_item){
        conn->m_key_item.m_bkey_hash = key_item->m_bkey_hash;
        conn->m_key_item.m_ex_bkey_hash = key_item->m_ex_bkey_hash;
        conn->m_key_item.m_header = key_item->m_header;
    }else{
        conn->m_key_item.m_bkey_hash = NULL;
        conn->m_key_item.m_ex_bkey_hash = NULL;
        conn->m_key_item.m_header = NULL;
    }
    if (!conn->m_key_item.m_bkey_hash && !conn->m_key_item.m_ex_bkey_hash){
        if (conn->m_service->m_key_item.m_bkey_hash || conn->m_service->m_key_item.m_ex_bkey_hash){
            conn->m_key_item.m_bkey_hash = conn->m_service->m_key_item.m_bkey_hash;
            conn->m_key_item.m_ex_bkey_hash = conn->m_service->m_key_item.m_ex_bkey_hash;
        }else{
            conn->m_key_item.m_bkey_hash = conn->m_config->m_key_item.m_bkey_hash;
            conn->m_key_item.m_ex_bkey_hash = conn->m_config->m_key_item.m_ex_bkey_hash;
        }
    }
    if (!conn->m_key_item.m_header){
        if (conn->m_service->m_key_item.m_header){
            conn->m_key_item.m_header= conn->m_service->m_key_item.m_header;
        }else{
            conn->m_key_item.m_header= conn->m_config->m_key_item.m_header;
        }
    }
}



static char* cwx_read_post(request_rec *r){
    char *data=0;
    const char *type;
    char buf[4096];
    int rc = OK;
    if(r->method_number != M_POST) return 0;

    type = (char*)apr_table_get(r->headers_in, "Content-Type");
    if(strcasecmp(type, "application/x-www-form-urlencoded") != 0) return 0;

    if (ap_should_client_block(r)) {
        int rsize, len_read, rpos=0;
        long length = r->remaining;
        data = (char*)apr_pcalloc(r->pool, length + 1);
        while ((len_read =
            ap_get_client_block(r, buf, sizeof(buf))) > 0) { 
                if (rpos + len_read > length) {
                    rsize = length - rpos;
                }else {
                    rsize = len_read;
                }
                memcpy(data + rpos, buf, rsize);
                rpos += rsize;
        }
        data[rpos] = 0x00;
    }
    return data;

}

static void cwx_parse_args(CWX_CONNECT* conn){
    char *begin = NULL;
    char const *mid = NULL;
    char const *end =NULL;
    char* args = conn->m_args;
    CWX_KEY_VALUE* key_value;
    unsigned int key_len = 0;
    unsigned int data_len = 0;
    conn->m_args_head = NULL;
    conn->m_args_tail =NULL;
    if (!args) return;
    
    begin = args;
    mid = strchr(begin, '=');
    while(mid){
        key_value = (CWX_KEY_VALUE*)apr_pcalloc(conn->m_request->pool, sizeof(CWX_KEY_VALUE));
        //get key
        while(*begin == '&') begin++;
        key_len = mid - begin;
        key_value->m_key = (char*)apr_pcalloc(conn->m_request->pool, key_len+1);
        key_value->m_lowerkey = (char*)apr_pcalloc(conn->m_request->pool, key_len+1);
        memcpy(key_value->m_key, begin, key_len);
        key_value->m_key[key_len] = 0x00;
        ap_unescape_url(key_value->m_key);
        cwx_trim(key_value->m_key);
        strcpy(key_value->m_lowerkey, key_value->m_key);
        cwx_tolower(key_value->m_lowerkey);
        //get data
        mid++;
        end = strchr(mid, '&');
        if (0 == end)
            data_len = strlen(mid);
        else
            data_len = end - mid;
        key_value->m_value = (char*)apr_pcalloc(conn->m_request->pool, data_len+1);
        memcpy(key_value->m_value, mid, data_len);
        key_value->m_value[data_len] = 0x00;
        ap_unescape_url(key_value->m_value);
        //add to the 
        key_value->m_next = NULL;
        if (!conn->m_args_head){
            conn->m_args_head = conn->m_args_tail = key_value;
        }else{
            conn->m_args_tail->m_next = key_value;
            conn->m_args_tail = key_value;
        }
        if (end){
            begin = end+1;
            mid = strchr(begin, '=');
        }else{
            break;
        }
    }
}

static void cwx_fetch_idle_conn(CWX_CONNECT* conn){
    conn->m_socket = NULL;
    if (conn->m_host){
        if (conn->m_host->m_idle_socket_head){//free socket
            conn->m_socket = conn->m_host->m_idle_socket_head;
            conn->m_host->m_idle_socket_head = conn->m_host->m_idle_socket_head->m_next;
            conn->m_socket->m_next = NULL;
            conn->m_host->m_idle_socket_num --;
            conn->m_host->m_used_socket_num ++;
        }
    }
}

static void cwx_create_socket(CWX_CONNECT* conn){
    //create new socket
    int fd = cwx_stream_connect(conn, conn->m_host->m_user_config.m_conn_timeout);
    if (-1 == fd){
        cwx_lock(conn->m_host->m_lock);
        conn->m_host->m_is_valid = false;
        conn->m_host->m_invalid_time = cwx_time_of_day();
        cwx_unlock(conn->m_host->m_lock);
        return;
    }
    conn->m_socket = (CWX_SOCKET*)malloc(sizeof(CWX_SOCKET));
    memset(conn->m_socket, 0x00, sizeof(CWX_SOCKET));
    conn->m_socket->m_fd = fd;
    conn->m_socket->m_next = NULL;

    cwx_lock(conn->m_host->m_lock);
    conn->m_host->m_is_valid = true;
    conn->m_host->m_used_socket_num ++;
    cwx_unlock(conn->m_host->m_lock);
}


static void cwx_get_def_conn(CWX_CONNECT* conn){
    int hashcode = 0;
    CWX_HOST* host=0;
    size_t i=0;
    size_t len;
    bool balance_key = true;
    char* value = NULL;

    CWX_KEY_VALUE* key_value = conn->m_args_head;
    if (key_value){
        balance_key = true;
        if (conn->m_key_item.m_bkey_hash){
            if (!apr_hash_get(conn->m_key_item.m_bkey_hash, key_value->m_lowerkey, APR_HASH_KEY_STRING)){
                balance_key = false;
            }
        }else if (conn->m_key_item.m_ex_bkey_hash){
            if (apr_hash_get(conn->m_key_item.m_ex_bkey_hash, key_value->m_lowerkey, APR_HASH_KEY_STRING)){
                balance_key = false;
            }
        }
        if (balance_key){
            value = key_value->m_lowerkey;
            while(*value){
               hashcode += (unsigned char)(*value);
               value++;
            }
        }
    }
    if (!hashcode){
        hashcode = abs(rand());
    }
    conn->m_def_host_index = hashcode%conn->m_service->m_host_num;
    conn->m_cur_host_index = conn->m_def_host_index;
    
    host = conn->m_service->m_hosts[conn->m_cur_host_index];
    conn->m_host = NULL;
    conn->m_socket = NULL;
    //Get lock
    cwx_lock(host->m_lock);
    if (!host->m_is_valid){
        if ((cwx_time_of_day() - host->m_invalid_time)/1000 > host->m_user_config.m_restore_timeout){
           conn->m_host = host;
        }
    }else{
        conn->m_host = host;
    }
    if (conn->m_host && (1 == conn->m_host->m_user_config.m_persistant)){
        cwx_fetch_idle_conn(conn);
    }
    //free lock
    cwx_unlock(host->m_lock);

    if (!conn->m_socket && conn->m_host){
        cwx_create_socket(conn);
        if (!conn->m_socket) conn->m_host = NULL;
    }
}

static void cwx_get_back_host_conn(CWX_CONNECT *conn){
    int cur_index = conn->m_cur_host_index;
    CWX_HOST* host = 0;
    conn->m_host = NULL;
    conn->m_socket = NULL;
    while(1){
        cur_index++;
        if (cur_index >= conn->m_service->m_host_num) cur_index = 0;
        if (cur_index == conn->m_def_host_index){
            return;
        }
        host = conn->m_service->m_hosts[cur_index];
        cwx_lock(host->m_lock);
        if (!host->m_is_valid){
            if ((cwx_time_of_day() - host->m_invalid_time)/1000 > host->m_user_config.m_restore_timeout){
                conn->m_host = host;
            }
        }else{
            conn->m_host = host;
        }
        cwx_unlock(host->m_lock);
        if (conn->m_host){
            cwx_create_socket(conn);
            if (!conn->m_socket){
                conn->m_host = NULL;
            }else{
                break;
            }
        }
    }
}

static void cwx_free_socket(CWX_CONNECT* conn){
    if (conn->m_host && conn->m_socket){
        //lock
        cwx_lock(conn->m_host->m_lock);
        if (-1 == conn->m_socket->m_fd){//it's closed
            free(conn->m_socket);
        }else{
            if ((conn->m_host->m_idle_socket_num > conn->m_host->m_user_config.m_max_idle_conn) ||
                (1 != conn->m_host->m_user_config.m_persistant))
            {
                cwx_stream_close(conn->m_socket->m_fd);
                free(conn->m_socket);
            }else{
                conn->m_socket->m_next = conn->m_host->m_idle_socket_head;
                conn->m_host->m_idle_socket_head = conn->m_socket;
                conn->m_host->m_idle_socket_num++;
            }
        }
        conn->m_host->m_used_socket_num -- ;
        //free lock.
        cwx_unlock(conn->m_host->m_lock);
        conn->m_socket = NULL;
        conn->m_host = NULL;
    }
}

static char* cwx_get_cookie(request_rec *r){
    return (char*)apr_table_get(r->headers_in, "Cookie");
}

static char* cwx_get_ip(request_rec *r){
#if AP_SERVER_MINORVERSION_NUMBER >=4
    return r->connection->client_ip;
#else
    return r->connection->remote_ip;
#endif
}

static void cwx_set_header(request_rec* r, char const* key, char const* value){
    apr_table_set(r->headers_out, key, value);
}

static void cwx_output_header(CWX_CONNECT* conn, CWX_UINT32 delay){
    CWX_KEY_VALUE* key_value = conn->m_key_item.m_header;
    if (1 == conn->m_host->m_user_config.m_delay){
        char szDelay[32];
        sprintf(szDelay, "%u", delay);
        cwx_set_header(conn->m_request, "cwinux-delay", szDelay);
    }
    while(key_value){
        cwx_set_header(conn->m_request, key_value->m_key, key_value->m_value);
        key_value = key_value->m_next;
    }
}

static int cwx_request(CWX_CONNECT* conn){
    char  szHead[CWINUX_MSG_HEAD_LEN + 1];
    CWX_MSG_HEADER_S msg_head;
    CWX_KEY_VALUE_ITEM_S item;
    CWX_KEY_VALUE*  key_value;
    CWX_UINT64 begin, end, now;
    char* pvalue;
    char* snd_buf=0;
    int buf_size = 0;
    int pos;
    int len;
    int ret;
    int http_code = OK;
    CWX_UINT32 delay = 0;

    pos = CWINUX_MSG_HEAD_LEN;
    buf_size = conn->m_args?strlen(conn->m_args)+ 2048:2048;
    snd_buf = (char*)apr_pcalloc(conn->m_request->pool, buf_size);
    //get ip
    pvalue = cwx_get_ip(conn->m_request);
    if (NULL == pvalue) pvalue="255.255.255.255";
    //append ip
    if (-1 == (len = cwx_append_key(snd_buf + pos, buf_size - pos, "cwinux_ip", pvalue, strlen(pvalue), false))){
        CWX_ERR(("%s:%d:cwx_request(): Failure to invoke cwx_append_key() to append cwinux_ip\n", __FILE__, __LINE__));
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    pos += len;
    //append url
    if (-1 == (len = cwx_append_key(snd_buf + pos, buf_size - pos, "cwinux_url", conn->m_request->uri, strlen(conn->m_request->uri), false))){
        CWX_ERR(("%s:%d:cwx_request(): Failure to invoke cwx_append_key() to append cwinux_url\n", __FILE__, __LINE__));
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    pos += len;

    key_value = conn->m_args_head;
    while(key_value){
        if ((strcmp(key_value->m_key, "cwinux_ip") != 0) && (strcmp(key_value->m_key, "cwinux_url") != 0)){
            //append arg
            if (-1 == (len = cwx_append_key(snd_buf + pos, buf_size - pos, key_value->m_key, key_value->m_value, strlen(key_value->m_value), false))){
                CWX_ERR(("%s:%d:cwx_request(): Failure to invoke cwx_append_key() to append arg key\n", __FILE__, __LINE__));
                return HTTP_INTERNAL_SERVER_ERROR;
            }
            pos += len;
        }
        key_value = key_value->m_next;
    }
    msg_head.m_ucAttr = 0;
    msg_head.m_ucVersion = 0;
    msg_head.m_uiDataLen = pos - CWINUX_MSG_HEAD_LEN;
    msg_head.m_uiTaskId = 0;
    msg_head.m_unMsgType = atoi(conn->m_msg_type);
    cwx_pack_head(&msg_head, snd_buf);

    //send msg
    begin = (CWX_UINT64)conn->m_request->request_time/1000;
    end = begin + conn->m_host->m_user_config.m_query_timeout;
    now = cwx_time_of_day()/1000;

    if (now >= end)  return HTTP_REQUEST_TIME_OUT;

    ret = cwx_stream_write(conn, snd_buf, pos, (CWX_UINT32)(end - now));
    if (-1 == ret){
        cwx_stream_close(conn->m_socket->m_fd);
        conn->m_socket->m_fd = -1;
        return HTTP_PRECONDITION_FAILED;
    }else if(0 == ret){
        cwx_stream_close(conn->m_socket->m_fd);
        conn->m_socket->m_fd = -1;
        return HTTP_REQUEST_TIME_OUT;
    }
    //recv msg in following

    //recv head first
    now = cwx_time_of_day()/1000;
    if (now >= end)  return HTTP_REQUEST_TIME_OUT;
    ret = cwx_stream_read(conn, szHead, CWINUX_MSG_HEAD_LEN, end - now);
    if (-1 == ret){
        cwx_stream_close(conn->m_socket->m_fd);
        conn->m_socket->m_fd = -1;
        return HTTP_BAD_REQUEST;
    }else if(0 == ret){
        cwx_stream_close(conn->m_socket->m_fd);
        conn->m_socket->m_fd = -1;
        return HTTP_REQUEST_TIME_OUT;
    }
    if (!cwx_unpack_head(szHead, &msg_head)){
        cwx_stream_close(conn->m_socket->m_fd);
        conn->m_socket->m_fd = -1;
        CWX_ERR(("%s:%d:cwx_request(): Failure to invoke cwx_unpack_head().\n", __FILE__, __LINE__));
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    //recv data
    char* recv_buf = (char*)apr_pcalloc(conn->m_request->pool, msg_head.m_uiDataLen);
    now = cwx_time_of_day()/1000;
    if (now >= end)  return HTTP_REQUEST_TIME_OUT;
    ret = cwx_stream_read(conn,
        recv_buf,
        msg_head.m_uiDataLen,
        end-now);
    if (-1 == ret){
        cwx_stream_close(conn->m_socket->m_fd);
        conn->m_socket->m_fd = -1;
        return HTTP_PRECONDITION_FAILED;
    }else if(0 == ret){
        cwx_stream_close(conn->m_socket->m_fd);
        conn->m_socket->m_fd = -1;
        return HTTP_REQUEST_TIME_OUT;
    }
    now = cwx_time_of_day()/1000;
    delay = now - (CWX_UINT64)conn->m_request->request_time/1000;
    if (1 != conn->m_host->m_user_config.m_show){
        //get code
        ret =  cwx_get_key_by_name(recv_buf, msg_head.m_uiDataLen, "code", &item);
        if (0>ret){
            CWX_ERR(("%s:%d:cwx_request(): handle=[%s] Failure to invoke cwx_get_key_by_name(\"code\").\n",
                __FILE__, __LINE__, conn->m_request->handler));
            return HTTP_INTERNAL_SERVER_ERROR;
        }else if (0 == ret){
            CWX_ERR(("%s:%d:cwx_request(): handle=[%s] no [code] key in reply package.\n",
                __FILE__, __LINE__, conn->m_request->handler));
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        ret = atoi(item.m_szData);
        if (0 > ret){
            http_code = HTTP_BAD_REQUEST;
        }else if (200 > ret){
            http_code = OK;
        }
        if (OK != http_code){
            CWX_ERR(("%s:%d:cwx_request(): handle=[%s] Query failure, code=%d.\n",
                __FILE__, __LINE__, conn->m_request->handler, ret));
        }
        //get content-type
        ret = cwx_get_key_by_name(recv_buf, msg_head.m_uiDataLen, "content-type", &item);
        if (ret>0){
            conn->m_request->content_type = item.m_szData;
        }
        //get content
        ret = cwx_get_key_by_name(recv_buf, msg_head.m_uiDataLen, "content", &item);
        if (0>ret){
            CWX_ERR(("%s:%d:cwx_request(): handle=[%s] Failure to invoke cwx_get_key_by_name(\"content\").\n",
                __FILE__, __LINE__, conn->m_request->handler));
            return HTTP_INTERNAL_SERVER_ERROR;
        }else if (0 == ret){
            CWX_ERR(("%s:%d:cwx_request(): handle=[%s] no [content] key in reply package.\n",
                __FILE__, __LINE__, conn->m_request->handler));
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        cwx_output_header(conn, delay);
        ap_rwrite(item.m_szData, item.m_uiDataLen, conn->m_request);
        return http_code;
    }
    //show the package
    if (!cwx_is_valid_package(recv_buf, msg_head.m_uiDataLen)){
        cwx_output_header(conn, delay);
        ap_rwrite(recv_buf, msg_head.m_uiDataLen, conn->m_request);
        return http_code;
    }

    char* out_buf = (char*)apr_pcalloc(conn->m_request->pool, msg_head.m_uiDataLen + 2048);
    CWX_UINT32 data_size = 0;
    len = 0;
    while(len < msg_head.m_uiDataLen){
        if (0>= cwx_get_key_by_index(recv_buf + len, msg_head.m_uiDataLen - len, 0, &item)) break;
        memcpy(out_buf +data_size, item.m_szKey, item.m_unKeyLen);
        data_size+=item.m_unKeyLen;
        out_buf[data_size++] = '\n';
        data_size += sprintf(out_buf+data_size, "%u\n", item.m_uiDataLen);
        memcpy(out_buf +data_size, item.m_szData, item.m_uiDataLen);
        data_size+=item.m_uiDataLen;
        out_buf[data_size++] = '\n';
        len += cwx_get_kv_len(item.m_unKeyLen, item.m_uiDataLen);
    }
    out_buf[data_size]=0x00;
    cwx_output_header(conn, delay);
    ap_rwrite(out_buf, data_size, conn->m_request);
    return http_code;
}

/**
 * Look at the request to see if Resin should handle it.
 */
static int cwx_dispatch(request_rec *r){
    CWX_CONFIG *config = cwx_get_server_config(r->server); 
    unsigned int now = (unsigned int) (r->request_time / 1000000);
    int len;
    char const* szTmp;
    CWX_SERVICE* svr = NULL;
    int ret = OK;
    CWX_CONNECT  conn;
    int code = DECLINED;

    //no config
    if (config == NULL || ! r->uri || !r->handler || ((r->method_number != M_GET)&&(r->method_number != M_POST)) ) return code;

    //no mine
    if ( 0 != memcmp(r->handler, "cwinux_", strlen("cwinux_"))) return code;
	//init the config
    if (!config->m_init) cwx_init_run_conf(config);
    //get svr name
    szTmp = strchr(r->handler + strlen("cwinux_"), '_');
    if (NULL == szTmp) return code;
    len = szTmp - (r->handler + strlen("cwinux_"));
    conn.m_svr_name = (char*)apr_pcalloc(r->pool, len + 1);
    memcpy(conn.m_svr_name, r->handler + strlen("cwinux_"), len);
    conn.m_svr_name[len] = 0x00;
    //get msg type
    szTmp++;
    len = strlen(szTmp);
    conn.m_msg_type = (char*)apr_pcalloc(r->pool, len + 1);
    memcpy(conn.m_msg_type, szTmp, len);
    conn.m_msg_type[len] = 0x00;
    //get svr object
    svr = (CWX_SERVICE*)apr_hash_get(config->m_svr_hash, conn.m_svr_name, APR_HASH_KEY_STRING);
    if (!svr) return code; //no service
    if (!svr->m_host_num) return HTTP_SERVICE_UNAVAILABLE; //no host

    conn.m_config = config;
    conn.m_service = svr;
    conn.m_pool = r->pool;
    conn.m_request = r;
    conn.m_host = NULL;
    conn.m_socket = NULL;
    conn.m_args_head = NULL;
    conn.m_args_tail = NULL;
    conn.m_def_host_index = 0;
    conn.m_cur_host_index = 0;
    ///install user's config
    cwx_fetch_conn_user_config(&conn);

    ///set the chunk type
    ret = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR);
    if (OK != ret) {
        CWX_ERR(("%s:%d:cwx_request(): Failure to invoke ap_setup_client_block().\n",__FILE__, __LINE__));
        return ret;
    }
    if (r->method_number == M_GET){
        conn.m_args = r->args;
    }else if (r->method_number == M_POST){
        conn.m_args = cwx_read_post(r);
        if (!conn.m_args) return DECLINED;
    }else{
        return DECLINED;
    }

    ///parse url's args
    cwx_parse_args(&conn);
    ///create the default connection
    cwx_get_def_conn(&conn);
    ///set default content type
    r->content_type = "text/html"; 
    while(1){
        //get ui host to query
        if (!conn.m_socket) cwx_get_back_host_conn(&conn);
        //no available host
        if (!conn.m_socket) return HTTP_SERVICE_UNAVAILABLE;
        //query
        code =  cwx_request(&conn);
        //free idle connect
        cwx_free_socket(&conn);
        //success, break;
        if (HTTP_PRECONDITION_FAILED != code) break;
        //using other.
        if ((time(NULL) - now > svr->m_user_config.m_query_timeout/2000)) break;
    }
    return code;
}

/*
 * Only needed configuration is pointer to resin.conf
 */
static const command_rec cwx_commands[] = {
    AP_INIT_RAW_ARGS("cwinux-service", cwx_svr_command, NULL,
    RSRC_CONF, "Configures service."),
    AP_INIT_RAW_ARGS("cwinux-host", cwx_host_command, NULL,
    RSRC_CONF, "Configures host."),
    AP_INIT_RAW_ARGS("cwinux-balance", cwx_balance_command, NULL,
    RSRC_CONF, "Configures balance."),
    AP_INIT_RAW_ARGS("cwinux-header", cwx_header_command, NULL,
    RSRC_CONF, "Configures header."),
    AP_INIT_TAKE1("cwinux-show", cwx_show_command, NULL,
    RSRC_CONF, "Configures default show."),
    AP_INIT_TAKE1("cwinux-delay", cwx_delay_command, NULL,
    RSRC_CONF, "Configures default delay."),
    AP_INIT_TAKE1("cwinux-persistent", cwx_persistent_command, NULL,
    RSRC_CONF, "Configures default persistent."),
    AP_INIT_TAKE1("cwinux-query-timeout", cwx_query_timeout_command, NULL,
    RSRC_CONF, "Configures default query-timeout."),
    AP_INIT_TAKE1("cwinux-conn-timeout", cwx_conn_timeout_command, NULL,
    RSRC_CONF, "Configures default conn-timeout."),
    AP_INIT_TAKE1("cwinux-reply-timeout", cwx_reply_timeout_command, NULL,
    RSRC_CONF, "Configures default reply-timeout."),
    AP_INIT_TAKE1("cwinux-restore-time", cwx_restore_time_command, NULL,
    RSRC_CONF, "Configures default restore-time."),
    AP_INIT_TAKE1("cwinux-min-idle-conn", cwx_min_idle_conn_command, NULL,
    RSRC_CONF, "Configures default min-idle-conn."),
    AP_INIT_TAKE1("cwinux-max-idle-conn", cwx_max_idle_conn_command, NULL,
    RSRC_CONF, "Configures default max-idle-conn."),
    {NULL}
};

static int cwx_prefork_post_config(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *dummy, server_rec *ptemp){
    ap_add_version_component(p, CWX_VERSION);
    return OK;
}

static void cwx_register_hooks(apr_pool_t *p){
    ap_hook_post_config(cwx_prefork_post_config, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_handler(cwx_dispatch, NULL, NULL, APR_HOOK_FIRST);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA cwinux_module = {
    STANDARD20_MODULE_STUFF, 
    NULL,    /* create per-dir    config structures */
    NULL,     /* merge  per-dir    config structures */
    cwx_create_server_config, /* create per-server config structures */
    cwx_merge_server_config,  /* merge  per-server config structures */
    cwx_commands,          /* table of config file commands       */
    cwx_register_hooks     /* register hooks                      */
};
