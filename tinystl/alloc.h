#pragma once

#include <new>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
// #include <iostream>

namespace mystl {
    
    class malloc_alloc {
    private:
        // static void * oom_malloc(size_t);
        // static void * oom_realloc(void *, size_t);
        // static void (* __malloc_alloc_oom_handler) ();

    public:
        static void * allocate(size_t n)
        {
            void * result = malloc(n);
            // if (nullptr == result) result = oom_malloc(n);

            return result;
        }

        static void deallocate(void *p, size_t)
        {
            free(p);
        }

        static void * reallocate(void *p, size_t /* old_sz */, size_t new_sz)
        {
            void * result = realloc(p, new_sz);
            // if (nullptr == result) result == oom_realloc(p, new_sz);

            return result;
        }
    };

    /*
        共用体：FreeList
        采用链表管理较小的内存块
    */
    union obj {
        union obj *next;
        char data[1];
    };

    enum {
        ALIGN128 = 8,
        ALIGN256 = 16,
        ALIGN512 = 32,
        ALIGN1024 = 64,
        ALIGN2048 = 128,
        ALIGN4096 = 256
    };

    enum { __MAX_BYTES = 128 };
    enum { __NFREELISTS = __MAX_BYTES / ALIGN128 };

    class alloc {
    private:
        static char *start_free;                       // 内存池起始位置
        static char *end_free;                         // 内存池结束为止
        static size_t heap_size;                       // 额外申请的空间大小
        static obj *free_list[__NFREELISTS];           // 16 个 free lists

    private:
        static size_t round_up(size_t bytes)           // 向上取整
        {
            return ((bytes + ALIGN128 - 1) & ~(ALIGN128 - 1));
        }

        static size_t freelist_index(size_t bytes)
        {
            return (((bytes + ALIGN128 - 1) / ALIGN128) - 1);
        }

        static void * refill(size_t n);
        static char * chunk_alloc(size_t size, int &nobjs);       // 配置 nobjs 个 size 大小的区块

    public:
        static void * allocate(size_t n);
        static void  deallocate(void *p, size_t n);
        static void * reallocate(void *p, size_t old_size, size_t new_size);

    };

    char  *alloc::start_free = nullptr;
    char  *alloc::end_free   = nullptr;
    size_t alloc::heap_size  = 0;

    obj *alloc::free_list[__NFREELISTS] = {
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr
    };

    void * alloc::allocate(size_t n)
    {
        obj * volatile *my_free_list;       // 指示 *my_free_list 是 volatile 的. 用来对 free_list 进行正确的读取和修改
                                            // 与 const 类似, volatile/const 指示其后的部分是 volatile/const 的.
                                            // 若 obj volatile **my_free_list, 则表示 **my_free_list 是 volatile 的
        obj *result;

        if (n > static_cast<size_t>(__MAX_BYTES)) {
            return malloc_alloc::allocate(n);
        }

        // my_free_list = free_list[freelist_index(n)];      // buggy
        my_free_list = free_list + freelist_index(n);
        result = *my_free_list;
        if (nullptr == result) {
            void *r = refill(round_up(n));
            return r;
        }
        *my_free_list = result->next;

        return result;
    }

    void alloc::deallocate(void *p, size_t n)
    {
        obj *q = (obj *)p;
        obj * volatile *my_free_list;

        if (n > (size_t)__MAX_BYTES) {
            malloc_alloc::deallocate(p, n);
            return;
        }

        my_free_list = free_list + freelist_index(n);
        q->next = *my_free_list;
        *my_free_list = q;
    }

    inline void * alloc::reallocate(void *p, size_t old_sz, size_t new_sz)
    {
        deallocate(p, old_sz);
        p = allocate(new_sz);
        return p;
    }

    void * alloc::refill(size_t size)
    {
        int nobjs = 20;
        char *chunk = chunk_alloc(size, nobjs);
        obj * volatile *my_free_list;
        obj *result;
        obj *current_obj, *next_obj;
        int i;

        // 取出第一个 obj 作为分配的结果返回, 其余 obj 添加到 freelist
        if (nobjs == 1) return chunk;
        my_free_list = free_list + freelist_index(size);
        result = (obj *)chunk;
        *my_free_list = next_obj = (obj *)(chunk + size);

        for (i = 1; ; ++i) {
            current_obj = next_obj;
            next_obj = (obj *)((char *)next_obj + size);
            if (nobjs - 1 == i) {
                current_obj->next = nullptr;
                break;
            }
            else {
                current_obj->next = next_obj;
            }
        }

        return result;
    }

    char * alloc::chunk_alloc(size_t size, int &nobjs)
    {
        char *result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;

        // 内存池空间满足 size * nobjs 所需的空间
        if (bytes_left >= total_bytes) {
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        // 内存池至少能满足一个区块的大小
        else if (bytes_left >= size) {
            nobjs = bytes_left / size;
            total_bytes = nobjs * size;
            result = start_free;
            start_free += total_bytes;
            
            return result;
        }
        else {
            // 零头入库
            if (bytes_left > 0){
                obj * volatile *my_free_list = free_list + freelist_index(bytes_left);
                ((obj *)start_free)->next = *my_free_list;
                *my_free_list = (obj *)start_free;
            }

            size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);
            start_free = (char *)malloc(bytes_to_get);
            if (nullptr == start_free) {
                // 尝试从比 size 大的区块中获得空间
                obj * volatile *my_free_list, *p;
                for (size_t i = size; i <= __MAX_BYTES; i += ALIGN128) {
                    my_free_list = free_list + freelist_index(i);
                    p = *my_free_list;
                    if (nullptr != p) {
                        *my_free_list = p->next;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        return chunk_alloc(size, nobjs);
                    }
                }    
                end_free = nullptr;
                throw std::bad_alloc();
            }
            end_free = start_free + bytes_to_get;
            heap_size += bytes_to_get;

            return chunk_alloc(size, nobjs);
        }
    }
};
