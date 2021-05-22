#include <new>
#include <stddef.h>
#include <limits.h>

namespace mystl {
    template <class T>
    inline T* allocate(ptrdiff_t size, T*)
    {
        set_new_handler(0);       // 设置 new 分配失败时的处理函数

        T *tmp = (T *)(::operator new((size_t)(size * sizeof(T))));        // 使用 operater new 单纯的分配内存空间
        if (tmp == 0) {
            std::cerr << "out of memory" << std::endl;
            exit(1);
        }

        return tmp;
    }

    template <class T>
    inline void deallocate(T *buffer)
    {
        ::operator delete(buffer);
    }

    /*
        简易套壳 allocator (标准版)
    */
    template <class T>
    class allocator {
    public:
        typedef T  value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        pointer allocate(size_type n)
        {
            return mystl::allocate((difference_type)n, (pointer)0);
        }

        void deallocate(pointer p) { mystl::deallocate(p); }
        pointer address(reference x) { return (pointer)&x; }
        const_pointer const_address(const_reference x)
        {
            return (const_pointer)&x;
        }
        size_type init_page_size()
        {
            return max(size_type(1), size_type(4096 / sizeof(T)));
        }
        size_type max_size() const
        {
            return max(size_type(1), size_type(UINT_MAX / sizeof(T)));
        }
    };

    class allocator<void> {
    public:
        typedef void* pointer;
    };
    
};