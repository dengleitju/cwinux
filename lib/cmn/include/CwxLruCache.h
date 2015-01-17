#ifndef __CWX_LRU_CACHE_H__
#define __CWX_LRU_CACHE_H__
/*
版权声明：
    本软件遵循GNU LGPL (Lesser General Public License)（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://weibo.com/cwinux
*/

/**
@file CwxLruCache.h
@brief LRU Cache的模板类。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"
#include "CwxGlobalMacro.h"
#include "CwxMutexLock.h"
#include "CwxLockGuard.h"
#include "CwxTypePoolEx.h"

CWINUX_BEGIN_NAMESPACE

/**
@class CwxLruCacheKeyData
@brief LRU cache的数据对象定义。
*/
template<typename KEY, typename DATA>
class CwxLruCacheKeyData
{
public:
    CwxLruCacheKeyData(KEY key, DATA data)
        :m_prev(NULL),m_next(NULL),m_key(key), m_data(data)
    {
    }
public:
    CwxLruCacheKeyData<KEY, DATA>*     m_prev;///<用于LRU cache链表的前向指针
    CwxLruCacheKeyData<KEY, DATA>*     m_next;///<用于LRU cache链表的后向指针
    KEY                              m_key; ///<LRU cache的KEY
    DATA                             m_data;///<LRU cache的DATA
};

/**
@class CwxLruCacheKeyMethod
@brief 实现LRU cache的KEY的hash, equal及数据大小计算的函数对象。
*/
template <typename KEY, typename DAT, typename HASH, typename EQUAL, typename KSIZE, typename DSIZE>
class CwxLruCacheKeyMethod
{
public:
    ///KEY的hash对象方法
    size_t operator()(KEY const* item) const 
    {
        return m_hash(*item);
    }
    ///KEY的equal对象方法
    bool operator()(KEY const* item1, KEY const* item2) const
    {
        return m_equal(*item1, *item2);
    }
    ///key与data的sizeof方法
    size_t size(CwxLruCacheKeyData<KEY, DATA> const& item) const
    {
        return m_ksize(item.m_key) + m_dsize(item.m_data) + 2 * sizeof(void*);
    }
    ///key与data的sizeof方法
    size_t size(KEY const& key, DATA const& data) const
    {
        return m_ksize(key) + m_dsize(data) + 2 * sizeof(void*);
    }
private:
    HASH  m_hash;///<key的hash对象函数
    EQUAL m_equal;///<key的equal对象函数
    KSIZE m_ksize;///<key的sizeof对象函数
    DSIZE m_dsize;///<data的sizeof对象函数
};


/**
@class CwxLruCache
@brief LRU cache的模板类。各个模板参数的含义如下：
KEY: lru cache的Key类型
DATA：lru cache的Data数据类型
HASH：KEY hash-code计算的方法对象，默认为hash\<KEY\>
EQUAL： KEY 相等比较的方法对象，默认为equal_to\<KEY\>
KSIZE：key的内存大小的计算方法对象，默认为CwxSize(KEY)
DSIZE：data的内存大小的计算方法对象，默认为CwxSize(KEY)
*/

template<typename KEY, typename DATA, typename HASH=hash<KEY>, typename EQUAL=equal_to<KEY>, typename KSIZE=CwxSize<KEY>, typename DSIZE=CwxSize<DATA> >
class CwxLruCache
{
public:
    typedef CwxLruCacheKeyData<KEY, DATA>  _CACHE_DATA; ///< cache数据类型
    typedef CwxLruCacheKeyMethod<KEY, DATA, HASH, EQUAL, KSIZE, DSIZE> _FUNC; ///<方法类型
    typedef hash_map<KEY const*, _CACHE_DATA*, _FUNC, _FUNC> _MAP;///<key的索引类型
    typedef typename _MAP::iterator _MAP_ITERATOR;///<key的索引iterator
private:
    _CACHE_DATA*    m_chain_head; ///<lru key's chain head
    _CACHE_DATA*    m_chain_tail;///<lru key's chain tail
    unsigned long int       m_size; ///<used size
    CWX_UINT32      m_count; ///<cached key count
    unsigned long int      m_max_size; ///<max key size
    CwxTypePoolEx<_CACHE_DATA> m_kv_pool;///<mem pool
    _MAP            m_index; ///<key's index
    CwxMutexLock* 	 m_lock; ///<lru cache's lock
    _FUNC           m_func; ///<key's function object

public:
    /**
    @brief 构造函数，设置LRU CACHE的内存及KEY的可能最大数量。
    @param [in] size LRU CACHE的内存总量
    @param [in] count LRU CACHE的KEY的可能最大值
    @param [in] bLock 是否多线程安全，内部进行锁同步
    */
    CwxLruCache(unsigned long int size, CWX_UINT32 count, bool bLock=true)
        :m_max_size( size ), m_kv_pool(count/20), m_index(count * 1.2)
    {
        m_chain_head = NULL;
        m_chain_tail = NULL;
        m_size = 0;
        m_count =0;
        if (bLock)
        {
            m_lock = new CwxMutexLock();
        }
        else
        {
            m_lock = NULL;
        }
    }
    ///析构函数
    ~CwxLruCache()
    {
        this->clear();
        if (m_lock)  delete m_lock;
        m_lock = NULL;
    }
public:
    ///获取内存使用量
    inline unsigned long int size( void ) const
    {
        CwxMutexGuard lock(m_lock);
        return m_size; 
    }
    ///获取cache的key的数量
    inline CWX_UINT32 count( void ) const
    {
        CwxMutexGuard lock(this->m_lock);
        return m_count; 
    }
    ///获取内存池中对象的数量
    inline CWX_UINT32 poolCount() const
    {
        CwxMutexGuard lock(this->m_lock);
        return m_kv_pool.size(); 
    }
    ///获取索引中对象的数量
    inline CWX_UINT32 long mapSize()
    {
        CwxMutexGuard lock(this->m_lock);
        return m_index.size(); 
    }
    ///获取最大可使用内存的数量
    inline unsigned long int maxSize( void ) const 
    {
        CwxMutexGuard lock(this->m_lock);
        return m_max_size;
    }
    ///清空CACHE    
    void clear( void )
    {
        CwxMutexGuard lock(this->m_lock);
        m_index.clear();
        m_kv_pool.clear();
        m_chain_head = NULL;
        m_chain_tail = NULL;
        m_size = 0;
        m_count =0;
    }
    ///检查key在CACHE中是否存在
    inline bool exist(KEY const&key ) const 
    {
        CwxMutexGuard lock(this->m_lock);
        return m_index.find( &key ) != m_index.end();
    }
    ///将key移到LRU cache的开始位置，防止换出
    inline void touch(KEY const &key )
    {
        CwxMutexGuard lock(this->m_lock);
        _MAP_ITERATOR miter = m_index.find( &key );
        if( miter == m_index.end() ) return ;
        _touch(miter->second);
    }
    /**
    @brief 获取一个KEY的data。
    @param [in] key 要获取DATA的KEY
    @param [in] bTouch 若KEY存在，是否将此KEY的移到LRU CACHE的头。
    @return NULL：KEY不存在；否则为KEY的data
    */
    inline DATA* fetch(KEY const &key, bool bTouch = true )
    {
        CwxMutexGuard lock(this->m_lock);
        _MAP_ITERATOR miter = m_index.find( &key );
        if( miter == m_index.end() )  return NULL;
        if( bTouch) _touch( miter->second );
        return miter->second;
    }    
    /**
    @brief 往LRU CACHE中插入一个KEY。
    @param [in] key 要插入的KEY
    @param [in] data 要插入Key的data。
    @return void
    */
    inline void insert(KEY const &key, DATA &data )
    {
        size_t size=0;
        CwxMutexGuard lock(this->m_lock);
        _CACHE_DATA* pData = NULL;
        _MAP_ITERATOR miter = this->m_index.find( &key );
        if( miter != m_index.end() )
        {
            size = m_func.size(*miter->second);
            m_size =(m_size>size)?m_size-size:0;
            miter->second->m_data = data;
            miter->second->m_key = key;
            m_size += m_func.size(*miter->second);
            _touch(miter->second);
            return;
        }
        // Check to see if we need to remove an element due to exceeding max_size
        unsigned long size = m_func.size(key, data);
        while(m_size + size > m_max_size)
        {
            // Remove the last element.
            pData = m_chain_tail;
            miter = this->index_.find(&pData->m_key);
            this->_remove( miter);
            m_kv_pool.free(pData);
        }
        pData = m_kv_pool.malloc(_CACHE_DATA(key, data));
        m_size += m_func.size(key, data);
        m_count ++;        
        //add to list
        pData->m_prev = NULL;
        pData->m_next = m_chain_head;
        if (m_chain_head == NULL)
        {
            m_chain_head = m_chain_tail = pData;
        }
        else
        {
            m_chain_head->m_prev = pData;
            m_chain_head = pData;
        }
        //add to map
        this->m_index[&pData->m_key] = pData;
    }
    ///从LRU cache中删除一个KEY
    inline void remove( KEY const &key )
    {
        CwxMutexGuard lock(this->m_lock);
        _MAP_ITERATOR miter = m_index.find( &key );
        if( miter == index_.end() ) return;
        _CACHE_DATA* pData = miter->second;
        _remove(miter);
        m_kv_pool.free(pData);
    }
private:
    ///不带锁的touch操作
    inline void _touch(_CACHE_DATA* data )
    {
        if (data->m_prev == NULL) //the head
            return;
        if (data->m_next == NULL)
        {// the tail
            m_chain_tail = data->m_prev;
            m_chain_tail->m_next = NULL;
        }
        else
        {
            data->m_prev->m_next = data->m_next;
            data->m_next->m_prev = data->m_prev;
        }
        data->m_prev = NULL;
        data->m_next = m_chain_head;
        m_chain_head->m_prev = data;
        m_chain_head = data;
    }
    ///不带锁的remove操作
    inline void _remove(_MAP_ITERATOR& miter )
    {
        size_t size = 0;
        _CACHE_DATA* data = miter->second;
        m_index.erase(miter);
        size = m_func.size(data->m_key, data->m_data);
        this->m_size -= m_size>size?m_size-size:0;
        this->m_count --;
        if (data == m_chain_head)
        {
            if (data == m_chain_tail)
            {
                m_chain_head = m_chain_tail = NULL;
                this->m_size = 0;
                this->m_count = 0;
            }
            else
            {
                m_chain_head = data->m_next;
                m_chain_head->m_prev = NULL;
            }
        }
        else if (data == m_chain_tail)
        {
            m_chain_tail = data->m_prev;
            m_chain_tail->m_next = NULL;
        }else
        {
            data->m_prev->m_next = data->m_next;
            data->m_next->m_prev = data->m_prev;
        }
        data->m_prev=data->m_next = NULL;
    }

};


CWINUX_END_NAMESPACE
#include "CwxPost.h"
#endif

