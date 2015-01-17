#include "cwx_config.h"

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>
#include "cwx_msg_header.h"
#include "cwx_package.h"

static ngx_flag_t  ngx_http_cwinux_enabled = 0;

static void *ngx_http_cwinux_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_cwinux_merge_loc_conf(ngx_conf_t *cf,
                                          void *parent, void *child);
static char *ngx_http_cwinux_pass(ngx_conf_t *cf, ngx_command_t *cmd,
                                void *conf);
static char *ngx_http_cwinux_content_type(ngx_conf_t *cf, ngx_command_t *cmd,
                                  void *conf);
static char *ngx_http_cwinux_show(ngx_conf_t *cf, ngx_command_t *cmd,
                                  void *conf);
static char *ngx_http_cwinux_delay(ngx_conf_t *cf, ngx_command_t *cmd,
                                  void *conf);

static char *ngx_http_cwinux_upstream_max_fails_unsupported(ngx_conf_t *cf,
                                                          ngx_command_t *cmd, void *conf);
static char *ngx_http_cwinux_upstream_fail_timeout_unsupported(ngx_conf_t *cf,
                                                             ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_cwinux_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_cwinux_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_cwinux_create_request(ngx_http_request_t *r);
static ngx_int_t ngx_http_cwinux_process_header(ngx_http_request_t *r);
static ngx_int_t ngx_http_cwinux_empty_filter_init(void *data);
static ngx_int_t ngx_http_cwinux_empty_filter(void *data, ssize_t bytes);
static ngx_int_t ngx_http_cwinux_reinit_request(ngx_http_request_t *r);
static void ngx_http_cwinux_abort_request(ngx_http_request_t *r);
static void ngx_http_cwinux_finalize_request(ngx_http_request_t *r, ngx_int_t rc);

static ngx_str_t  ngx_http_cwinux_hide_headers[] = {
    ngx_null_string
};

typedef struct {
    ngx_http_request_t        *request;
    CWX_MSG_HEADER_S          msg_head;
    bool                     header;
    size_t                   recv_pos;
    char*                    data;
    bool                     finish;
    char*                    out_buf;
    size_t                   out_buf_len;
    struct timeval           start_time;

} ngx_http_cwinux_ctx_t;

static ngx_conf_bitmask_t  ngx_http_cwinux_next_upstream_masks[] = {
    { ngx_string("error"), NGX_HTTP_UPSTREAM_FT_ERROR },
    { ngx_string("timeout"), NGX_HTTP_UPSTREAM_FT_TIMEOUT },
    { ngx_string("invalid_response"), NGX_HTTP_UPSTREAM_FT_INVALID_HEADER },
    { ngx_string("not_found"), NGX_HTTP_UPSTREAM_FT_HTTP_404 },
    { ngx_string("off"), NGX_HTTP_UPSTREAM_FT_OFF },
    { ngx_null_string, 0 }
};


static ngx_command_t  ngx_http_cwinux_commands[] = {
    { ngx_string("cwinux_pass"),
    NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE1,
    ngx_http_cwinux_pass,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },

    { ngx_string("cwinux_content_type"),
    NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE1,
    ngx_http_cwinux_content_type,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },

    { ngx_string("cwinux_show"),
    NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_NOARGS,
    ngx_http_cwinux_show,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },

    { ngx_string("cwinux_delay"),
    NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_NOARGS,
    ngx_http_cwinux_delay,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },


    { ngx_string("cwinux_connect_timeout"),
    NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
    ngx_conf_set_msec_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_cwinux_loc_conf_t, upstream.connect_timeout),
    NULL },

    { ngx_string("cwinux_send_timeout"),
    NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
    ngx_conf_set_msec_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_cwinux_loc_conf_t, upstream.send_timeout),
    NULL },

    { ngx_string("cwinux_read_timeout"),
    NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
    ngx_conf_set_msec_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_cwinux_loc_conf_t, upstream.read_timeout),
    NULL },

    { ngx_string("cwinux_buffer_size"),
    NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
    ngx_conf_set_size_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_cwinux_loc_conf_t, upstream.buffer_size),
    NULL },


    { ngx_string("cwinux_next_upstream"),
    NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
    ngx_conf_set_bitmask_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_cwinux_loc_conf_t, upstream.next_upstream),
    &ngx_http_cwinux_next_upstream_masks },

    { ngx_string("cwinux_upstream_max_fails"),
    NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
    ngx_http_cwinux_upstream_max_fails_unsupported,
    0,
    0,
    NULL },

    { ngx_string("cwinux_upstream_fail_timeout"),
    NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
    ngx_http_cwinux_upstream_fail_timeout_unsupported,
    0,
    0,
    NULL },

    ngx_null_command
};


static ngx_http_module_t  ngx_http_cwinux_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_cwinux_init,                    /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_cwinux_create_loc_conf,    /* create location configration */
    ngx_http_cwinux_merge_loc_conf      /* merge location configration */
};


ngx_module_t  ngx_http_cwinux_module = {
    NGX_MODULE_V1,
    &ngx_http_cwinux_module_ctx,        /* module context */
    ngx_http_cwinux_commands,           /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_str_t* ngx_http_cwinux_merge_bufs(ngx_chain_t* bufs, ngx_pool_t* pool){
    size_t len = 0;
    ngx_str_t* str = ngx_pcalloc(pool, sizeof(ngx_str_t));
    ngx_chain_t* cur_bufs = bufs;
    while (cur_bufs){
        len += cur_bufs->buf->last - cur_bufs->buf->pos;
        cur_bufs = cur_bufs->next;
    }
    str->len = len;
    str->data = ngx_pcalloc(pool, len + 1);
    cur_bufs = bufs;
    len = 0;
    while (cur_bufs){
        memcpy(str->data + len, cur_bufs->buf->pos, cur_bufs->buf->last - cur_bufs->buf->pos);
        len += cur_bufs->buf->last - cur_bufs->buf->pos;
        cur_bufs = cur_bufs->next;
    }
    return str;
}

static char const* ngx_cwinux_strchr(char const* str, size_t len, char ch){
    size_t i = 0;
    while((i<len)&&(str[i]!=ch)) i++;
    return i<len?&str[i]:NULL;
}

static void ngx_cwinux_trim(char* value){
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


static CWX_KEY_VALUE* ngx_http_cwinux_parse_args(ngx_str_t* args, ngx_pool_t* pool)
{
    CWX_KEY_VALUE* head = NULL;
    CWX_KEY_VALUE* tail = NULL;
    char const *begin = NULL;
    char const *mid = NULL;
    char const *end =NULL;
    CWX_KEY_VALUE* key_value = NULL;
    unsigned int key_len = 0;
    unsigned int data_len = 0;
    u_char* src, *dst;
    size_t len = args->len;
    begin = (char*)args->data;
    mid = ngx_cwinux_strchr(begin, len, '=');
    while(mid){
        key_value = (CWX_KEY_VALUE*)ngx_pcalloc(pool, sizeof(CWX_KEY_VALUE));
        //get key
        while(*begin == '&') begin++;
        key_len = mid - begin;
        key_value->m_key = (char*)ngx_pcalloc(pool, key_len+1);
        key_value->m_lowerkey = (char*)ngx_pcalloc(pool, key_len+1);
        memcpy(key_value->m_key, begin, key_len);
        key_value->m_key[key_len] = 0x00;
        src = (u_char*)key_value->m_key;
        dst = (u_char*)key_value->m_key;
        ngx_unescape_uri(&dst, &src, key_len, 0);
        *dst = 0;
        ngx_cwinux_trim(key_value->m_key);
        ngx_strlow((u_char*)key_value->m_lowerkey, (u_char*)key_value->m_key, strlen(key_value->m_key));
        //get data
        mid++;
        end = strchr(mid, '&');
        if (0 == end)
            data_len = args->len - ((u_char*)mid - args->data);
        else
            data_len = end - mid;
        key_value->m_value = (char*)ngx_pcalloc(pool, data_len+1);
        memcpy(key_value->m_value, mid, data_len);
        key_value->m_value[data_len] = 0x00;
        src = (u_char*)key_value->m_value;
        dst = (u_char*)key_value->m_value;
        ngx_unescape_uri(&dst, &src, data_len, 0);
        *dst = 0;
        //add to the 
        key_value->m_next = NULL;
        if (!head){
            head = tail = key_value;
        }else{
            tail->m_next = key_value;
            tail = key_value;
        }
        if (end){
            len -= (end+1 - begin);
            begin = end+1;
            mid = ngx_cwinux_strchr(begin, len, '=');
        }else{
            break;
        }
    }
    return head;

}


static ngx_int_t ngx_http_cwinux_create_request(ngx_http_request_t *r)
{
    ngx_http_cwinux_ctx_t            *ctx;
    ngx_http_cwinux_loc_conf_t       *mlcf;
    ngx_str_t args;
    CWX_KEY_VALUE* keys;
    size_t buf_size;
    size_t pos=CWINUX_MSG_HEAD_LEN;
    int len;
    CWX_MSG_HEADER_S msg_head;
    ngx_buf_t                      *b;
    ngx_chain_t                    *cl;
    ngx_str_t* pargs = &args;
    if (r->method == NGX_HTTP_POST){
        if (r->request_body && r->request_body->bufs){
            if (r->request_body->bufs->next){
                pargs = ngx_http_cwinux_merge_bufs(r->request_body->bufs, r->pool);
            }else{
                args.data = r->request_body->bufs->buf->pos;
                args.len = r->request_body->bufs->buf->last - r->request_body->bufs->buf->pos;
            }
        }
    }else{
        pargs = &r->args;
    }
    keys = ngx_http_cwinux_parse_args(pargs, r->pool);

    buf_size = pargs->len + 2048;
    b = ngx_create_temp_buf(r->pool, buf_size);
    if (b == NULL) {
        return NGX_ERROR;
    }
    //append ip
    if (-1 == (len = cwx_append_key((char*)b->pos + pos, buf_size - pos, "cwinux_ip", "0.0.0.0", strlen("0.0.0.0"), false))){
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
            "Failure to invoke cwx_append_key() to append cwinux_ip");
        return NGX_ERROR;
    }
    pos += len;
    //append url
    if (-1 == (len = cwx_append_key((char*)b->pos + pos, buf_size - pos, "cwinux_url", (char*)r->uri.data, r->uri.len, false))){
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
            "Failure to invoke cwx_append_key() to append cwinux_url");
        return NGX_ERROR;
    }
    pos += len;
    while(keys){
        if ((strcmp(keys->m_key, "cwinux_ip") != 0) && (strcmp(keys->m_key, "cwinux_url") != 0)){
            //append arg
            if (-1 == (len = cwx_append_key((char*)b->pos + pos, buf_size - pos, keys->m_key, keys->m_value, strlen(keys->m_value), false))){
                ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                    "Failure to invoke cwx_append_key() to append %s", keys->m_key);
                return NGX_ERROR;
            }
            pos += len;
        }
        keys = keys->m_next;
    }
    msg_head.m_ucAttr = 0;
    msg_head.m_ucVersion = 0;
    msg_head.m_uiDataLen = pos - CWINUX_MSG_HEAD_LEN;
    msg_head.m_uiTaskId = 0;
    msg_head.m_unMsgType = 1;
    cwx_pack_head(&msg_head, (char*)b->pos);
    b->last = b->pos + pos;


    mlcf = ngx_http_get_module_loc_conf(r, ngx_http_cwinux_module);
    ctx = ngx_http_get_module_ctx(r, ngx_http_cwinux_module);

    cl = ngx_alloc_chain_link(r->pool);
    if (cl == NULL) {
        return NGX_ERROR;
    }
    cl->buf = b;
    cl->next = NULL;
    r->upstream->request_bufs = cl;

    return NGX_OK;
}


void ngx_http_cwinux_set_module_enabled() {
    ngx_http_cwinux_enabled = 1;
}


static void *ngx_http_cwinux_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_cwinux_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_cwinux_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    conf->content_type.data=(u_char*)"text/html";
    conf->content_type.len = strlen("text/html");
    conf->delay = false;
    conf->show = false;
    conf->upstream.connect_timeout = NGX_CONF_UNSET_MSEC;
    conf->upstream.send_timeout = NGX_CONF_UNSET_MSEC;
    conf->upstream.read_timeout = NGX_CONF_UNSET_MSEC;
    conf->upstream.buffer_size = NGX_CONF_UNSET_SIZE;
    /* the hardcoded values */
    conf->upstream.cyclic_temp_file = 0;
    conf->upstream.buffering = 0;
    conf->upstream.ignore_client_abort = 0;
    conf->upstream.send_lowat = 0;
    conf->upstream.bufs.num = 0;
    conf->upstream.busy_buffers_size = 0;
    conf->upstream.max_temp_file_size = 0;
    conf->upstream.temp_file_write_size = 0;
    conf->upstream.intercept_errors = 1;
    conf->upstream.intercept_404 = 1;
    conf->upstream.pass_request_headers = 0;
    conf->upstream.pass_request_body = 0;
    conf->upstream.hide_headers = NGX_CONF_UNSET_PTR;
    conf->upstream.pass_headers = NGX_CONF_UNSET_PTR;
    return conf;
}


static char * ngx_http_cwinux_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_cwinux_loc_conf_t *prev = parent;
    ngx_http_cwinux_loc_conf_t *conf = child;
    ngx_hash_init_t               hash;
    ngx_str_t                    *h;


    ngx_conf_merge_msec_value(conf->upstream.connect_timeout,
        prev->upstream.connect_timeout, 100);

    ngx_conf_merge_msec_value(conf->upstream.send_timeout,
        prev->upstream.send_timeout, 100);

    ngx_conf_merge_msec_value(conf->upstream.read_timeout,
        prev->upstream.read_timeout, 1000);

    ngx_conf_merge_size_value(conf->upstream.buffer_size,
        prev->upstream.buffer_size,
        (size_t) ngx_pagesize);

    ngx_conf_merge_bitmask_value(conf->upstream.next_upstream,
        prev->upstream.next_upstream,
        (NGX_CONF_BITMASK_SET
        |NGX_HTTP_UPSTREAM_FT_ERROR
        |NGX_HTTP_UPSTREAM_FT_TIMEOUT));

    if (conf->upstream.next_upstream & NGX_HTTP_UPSTREAM_FT_OFF) {
        conf->upstream.next_upstream = NGX_CONF_BITMASK_SET
            |NGX_HTTP_UPSTREAM_FT_OFF;
    }
    hash.max_size = 512;
    hash.bucket_size = ngx_align(64, ngx_cacheline_size);
    hash.name = "cwinux_hide_headers_hash";
    h = ngx_http_cwinux_hide_headers;

    if (ngx_http_upstream_hide_headers_hash(cf, &conf->upstream,
        &prev->upstream, h, &hash)
        != NGX_OK)
    {
        return NGX_CONF_ERROR;
    }

    if (conf->upstream.upstream == NULL) {
        conf->upstream.upstream = prev->upstream.upstream;
    }


    return NGX_CONF_OK;
}


static char * ngx_http_cwinux_pass(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_cwinux_loc_conf_t *mlcf = conf;
    ngx_str_t                 *value;
    ngx_url_t                  u;
    ngx_http_core_loc_conf_t  *clcf;

    if (mlcf->upstream.upstream) {
        return "is duplicate";
    }

    value = cf->args->elts;

    ngx_memzero(&u, sizeof(ngx_url_t));

    u.url = value[1];
    u.no_resolve = 1;

    mlcf->upstream.upstream = ngx_http_upstream_add(cf, &u, 0);
    if (mlcf->upstream.upstream == NULL) {
        return NGX_CONF_ERROR;
    }

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    clcf->handler = ngx_http_cwinux_handler;

    if (clcf->name.data[clcf->name.len - 1] == '/') {
        clcf->auto_redirect = 1;
    }

    ngx_http_cwinux_set_module_enabled();

    return NGX_CONF_OK;
}


static char * ngx_http_cwinux_content_type(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_cwinux_loc_conf_t *mlcf = conf;
    ngx_str_t                 *value;
    value = cf->args->elts;
    mlcf->content_type=value[1];
    return NGX_CONF_OK;
}
static char * ngx_http_cwinux_show(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_cwinux_loc_conf_t *mlcf = conf;
    mlcf->show = true;
    return NGX_CONF_OK;
}

static char * ngx_http_cwinux_delay(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_cwinux_loc_conf_t *mlcf = conf;
    mlcf->delay = true;
    return NGX_CONF_OK;
}



static char * ngx_http_cwinux_upstream_max_fails_unsupported(ngx_conf_t *cf,
                                             ngx_command_t *cmd, void *conf)
{
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
        "\"cwinux_upstream_max_fails\" is not supported, "
        "use the \"max_fails\" parameter of the \"server\" directive ",
        "inside the \"upstream\" block");
    return NGX_CONF_ERROR;
}


static char * ngx_http_cwinux_upstream_fail_timeout_unsupported(ngx_conf_t *cf,
                                                ngx_command_t *cmd, void *conf)
{
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
        "\"cwinux_upstream_fail_timeout\" is not supported, "
        "use the \"fail_timeout\" parameter of the \"server\" directive ",
        "inside the \"upstream\" block");

    return NGX_CONF_ERROR;
}


static ngx_int_t ngx_http_cwinux_init(ngx_conf_t *cf)
{
    if (!ngx_http_cwinux_enabled) {
        return NGX_OK;
    }
    return NGX_OK;
}

static ngx_int_t ngx_http_cwinux_handler(ngx_http_request_t *r)
{
    ngx_int_t                       rc;
    ngx_http_upstream_t            *u;
    ngx_http_cwinux_ctx_t            *ctx;
    ngx_http_cwinux_loc_conf_t       *mlcf;

    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_POST))){
        ngx_log_error(NGX_LOG_ALERT, r->connection->log, 0,
            "ngx_cwinux: method %V doesn't support ",
            &r->method_name);
        return NGX_HTTP_BAD_REQUEST;
    }
    mlcf = ngx_http_get_module_loc_conf(r, ngx_http_cwinux_module);

    if (ngx_http_set_content_type(r) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

#if defined(nginx_version) && \
    ((nginx_version >= 7063 && nginx_version < 8000) \
    || nginx_version >= 8007)

    if (ngx_http_upstream_create(r) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    u = r->upstream;

#else /* 0.7.x < 0.7.63, 0.8.x < 0.8.7 */

    u = ngx_pcalloc(r->pool, sizeof(ngx_http_upstream_t));
    if (u == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    u->peer.log = r->connection->log;
    u->peer.log_error = NGX_ERROR_ERR;
#  if (NGX_THREADS)
    u->peer.lock = &r->connection->lock;
#  endif

    r->upstream = u;

#endif

    u->schema.len = sizeof("cwinux://") - 1;
    u->schema.data = (u_char *) "cwinux://";

    u->output.tag = (ngx_buf_tag_t) &ngx_http_cwinux_module;

    u->conf = &mlcf->upstream;

    ctx = ngx_palloc(r->pool, sizeof(ngx_http_cwinux_ctx_t));
    if (ctx == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    ngx_gettimeofday(&ctx->start_time);
    ctx->header = false;
    ctx->finish = false;
    ctx->request = r;
    ngx_http_set_ctx(r, ctx, ngx_http_cwinux_module);

    u->create_request = ngx_http_cwinux_create_request;
    u->process_header = ngx_http_cwinux_process_header;

    u->input_filter_init = ngx_http_cwinux_empty_filter_init;
    u->input_filter = ngx_http_cwinux_empty_filter;

    u->reinit_request = ngx_http_cwinux_reinit_request;
    u->abort_request = ngx_http_cwinux_abort_request;
    u->finalize_request = ngx_http_cwinux_finalize_request;

    u->input_filter_ctx = ctx;

    if (NGX_HTTP_POST == r->method){
        rc = ngx_http_read_client_request_body(r, ngx_http_upstream_init);

        if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
            return rc;
        }

        return NGX_DONE;
    }
    rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK) {
        return rc;
    }

#if defined(nginx_version) && nginx_version >= 8011
    r->main->count++;
#endif

    ngx_http_upstream_init(r);
    return NGX_DONE;
}



static ngx_int_t ngx_http_cwinux_process_header(ngx_http_request_t *r)
{
    ngx_http_upstream_t       *u;
    ngx_http_cwinux_ctx_t  *ctx;
    ngx_http_cwinux_loc_conf_t       *mlcf;
    size_t     recv_data_len;
    size_t     left_data_len;
    size_t     copy_data_len;
    size_t len;
    int        http_code = 200;
    CWX_KEY_VALUE_ITEM_S item;
    int  ret;
    u = r->upstream;
    mlcf = ngx_http_get_module_loc_conf(r, ngx_http_cwinux_module);
    ctx = ngx_http_get_module_ctx(r, ngx_http_cwinux_module);
    if (!ctx->header){
        if (u->buffer.last - u->buffer.pos <CWINUX_MSG_HEAD_LEN){
            return NGX_AGAIN;
        }
        if (!cwx_unpack_head((char*)u->buffer.pos, &ctx->msg_head)){
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                "service sent invalid header");
            return NGX_HTTP_UPSTREAM_INVALID_HEADER;
        }
        ctx->header = true;
        ctx->recv_pos = 0;
        ctx->data = ngx_palloc(r->pool, ctx->msg_head.m_uiDataLen);
        recv_data_len = u->buffer.last - u->buffer.pos - CWINUX_MSG_HEAD_LEN;
        left_data_len = ctx->msg_head.m_uiDataLen - ctx->recv_pos;
        copy_data_len = left_data_len > recv_data_len?recv_data_len:left_data_len;
        if (left_data_len && recv_data_len){
            memcpy(ctx->data + ctx->recv_pos, u->buffer.pos + CWINUX_MSG_HEAD_LEN, copy_data_len);
            ctx->recv_pos += copy_data_len;
        }
        u->buffer.last = 0;
        if (ctx->recv_pos < ctx->msg_head.m_uiDataLen) return NGX_AGAIN;
    }else{
        recv_data_len = u->buffer.last - u->buffer.pos;
        left_data_len = ctx->msg_head.m_uiDataLen - ctx->recv_pos;
        copy_data_len = left_data_len >recv_data_len?recv_data_len:left_data_len;
        memcpy(ctx->data + ctx->recv_pos, u->buffer.pos, copy_data_len);
        ctx->recv_pos += copy_data_len;
        u->buffer.last = 0;
        if (ctx->recv_pos < ctx->msg_head.m_uiDataLen) return NGX_AGAIN;
    }
    ctx->request->headers_out.content_type=mlcf->content_type;
    ctx->request->headers_out.content_type_len=mlcf->content_type.len;
    if (mlcf->show){
        //show the package
        if (!cwx_is_valid_package(ctx->data, ctx->msg_head.m_uiDataLen)){
            ctx->out_buf = ctx->data;
            ctx->out_buf_len = ctx->msg_head.m_uiDataLen;
        }else{
            ctx->out_buf = ngx_palloc(ctx->request->pool, ctx->msg_head.m_uiDataLen * 2);
            ctx->out_buf_len = 0;
            len = 0;
            while(len < ctx->msg_head.m_uiDataLen){
                if (0>= cwx_get_key_by_index(ctx->data + len, ctx->msg_head.m_uiDataLen - len, 0, &item)) break;
                memcpy(ctx->out_buf + ctx->out_buf_len, item.m_szKey, item.m_unKeyLen);
                ctx->out_buf_len+=item.m_unKeyLen;
                ctx->out_buf[ctx->out_buf_len++] = '\n';
                ctx->out_buf_len += sprintf(ctx->out_buf+ctx->out_buf_len, "%u\n", item.m_uiDataLen);
                memcpy(ctx->out_buf + ctx->out_buf_len, item.m_szData, item.m_uiDataLen);
                ctx->out_buf_len+=item.m_uiDataLen;
                ctx->out_buf[ctx->out_buf_len++] = '\n';
                len += cwx_get_kv_len(item.m_unKeyLen, item.m_uiDataLen);
            }
            ctx->out_buf[ctx->out_buf_len]=0x00;
        }
    }else{
        ctx->out_buf = " ";
        ctx->out_buf_len = 1;
        //get code
        ret =  cwx_get_key_by_name(ctx->data, ctx->msg_head.m_uiDataLen, "code", &item);
        if (0>ret){
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                " Failure to invoke cwx_get_key_by_name(code)");
            return NGX_HTTP_UPSTREAM_FT_INVALID_HEADER;
        }else if (0 == ret){
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                " no [code] key in reply package.");
            return NGX_HTTP_UPSTREAM_FT_INVALID_HEADER;
        }
        ret = atoi(item.m_szData);
        if (0 > ret){
            http_code = NGX_HTTP_BAD_REQUEST;
        }else if (200 > ret){
            http_code = NGX_HTTP_OK;
        }
        if (NGX_HTTP_OK != http_code){
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                " Query failure, code=%d.", ret);
        }
        //get content-type
        ret = cwx_get_key_by_name(ctx->data, ctx->msg_head.m_uiDataLen, "content-type", &item);
        if (ret>0){
            ctx->request->headers_out.content_type.data=ngx_palloc(ctx->request->pool,item.m_uiDataLen);
            memcpy(ctx->request->headers_out.content_type.data, item.m_szData, item.m_uiDataLen);
            ctx->request->headers_out.content_type.len=item.m_uiDataLen;
            ctx->request->headers_out.content_type_len=item.m_uiDataLen;
        }
        //get content
        ret = cwx_get_key_by_name(ctx->data, ctx->msg_head.m_uiDataLen, "content", &item);
        if (0>ret){
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                " Failure to invoke cwx_get_key_by_name(content)");
            return NGX_HTTP_UPSTREAM_FT_INVALID_HEADER;
        }else if (0 == ret){
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                " no [content] key in reply package.");
            return NGX_HTTP_UPSTREAM_FT_INVALID_HEADER;
        }
        ctx->out_buf = ngx_palloc(ctx->request->pool, item.m_uiDataLen);
        memcpy(ctx->out_buf, item.m_szData, item.m_uiDataLen);
        ctx->out_buf_len = item.m_uiDataLen;
    }
    if (mlcf->delay){
        struct timeval  tp;
        ngx_gettimeofday(&tp);
        size_t delay= (tp.tv_sec - ctx->start_time.tv_sec)*1000 + (tp.tv_usec - ctx->start_time.tv_usec)/1000;
        ngx_table_elt_t   *h;
        h = ngx_list_push(&u->headers_in.headers);
        if (h) {
            char szBuf[32];
            sprintf(szBuf, "%u", delay);
            h->key.len = strlen("cwinux_delay");
            h->key.data = (u_char*)"cwinux_delay";
            h->value.len = strlen(szBuf);
            h->value.data = ngx_palloc(ctx->request->pool, h->value.len + 1);
            strcpy((char*)h->value.data, szBuf);
            h->lowcase_key = h->key.data;
        }
    }

    r->headers_out.content_length_n = ctx->out_buf_len;
    r->headers_out.content_length_n = ctx->out_buf_len;
    u->headers_in.status_n = http_code;
    u->state->status = http_code;
    return NGX_OK;
}

static ngx_int_t ngx_http_cwinux_empty_filter_init(void *data)
{
    ngx_http_cwinux_ctx_t  *ctx = data;
    ngx_http_upstream_t  *u;

    u = ctx->request->upstream;
    ctx->finish = false;

    /* to persuade ngx_http_upstream_keepalive (if any)
    to cache the connection if the status is neither
    200 nor 404. */
    if (u->headers_in.status_n == NGX_HTTP_CREATED) {
        u->headers_in.status_n = NGX_HTTP_OK;
    }

    return NGX_OK;
}

static ngx_int_t ngx_http_cwinux_empty_filter(void *data, ssize_t bytes)
{
    ngx_http_cwinux_ctx_t  *ctx = data;
    ngx_chain_t          *cl, **ll;
    ngx_http_upstream_t  *u;

    u = ctx->request->upstream;
    if (ctx->finish){
        u->length = 0;
        return NGX_OK;
    }


    for (cl = u->out_bufs, ll = &u->out_bufs; cl; cl = cl->next) {
        ll = &cl->next;
    }
    cl = ngx_chain_get_free_buf(ctx->request->pool, &u->free_bufs);
    if (cl == NULL) {
        return NGX_ERROR;
    }
    cl->buf->flush = 1;
    cl->buf->memory = 1;
    *ll = cl;
    cl->buf->pos = (u_char*)ctx->out_buf;
    cl->buf->last = cl->buf->pos + ctx->out_buf_len;
    cl->buf->tag = u->output.tag;
    u->length = 0;
    ctx->finish = true;
    return NGX_OK;
}

static ngx_int_t ngx_http_cwinux_reinit_request(ngx_http_request_t *r)
{
    return NGX_OK;
}


static void ngx_http_cwinux_abort_request(ngx_http_request_t *r)
{
    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
        "abort http cwinux request");
    return;
}


static void ngx_http_cwinux_finalize_request(ngx_http_request_t *r, ngx_int_t rc)
{
    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
        "finalize http cwinux request");
    return;
}

