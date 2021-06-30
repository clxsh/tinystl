#pragma once

#include <new>
#include "typetraits.h"
#include "iterator.h"
#include "util.h"

/* ref: https://blog.csdn.net/lx627776548/article/details/51888789 */
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100) // unused variable
#endif

namespace mystl {
 
    // 使用 placement new, 构造对象
    template <class Ty>
    void construct(Ty *ptr)
    {
        ::new (ptr) Ty();
    }

    template <class Ty1, class Ty2>
    void construct(Ty1 *ptr, const Ty2 &value)
    {
        ::new (ptr) Ty1(value);
    }

    template <class Ty, class... Args>
    void construct(Ty *ptr, Args&&... args)
    {
        ::new (ptr) Ty(mystl::forward<Args>(args)...);      // to be continued
    }

    // destroy, 析构对象
    template <class Ty>
    void destroy_one(Ty *, std::true_type) { }

    template <class Ty>
    void destroy_one(Ty * pointer, std::false_type)
    {
        if (pointer != nullptr) {
            pointer->~Ty();
        }
    }

    template <class ForwardIter>
    void destroy_aux(ForwardIter , ForwardIter , std::true_type) { }

    template <class ForwardIter>
    void destroy_aux(ForwardIter first, ForwardIter last, std::false_type)
    {
        for (; first != last; ++first) {
            destroy(&*first);
        }
    }

    template <class Ty>
    void destroy(Ty *pointer)
    {
        destroy_one(pointer, std::is_trivially_destructible<Ty>{});
    }

    template <class ForwardIter>
    void destroy(ForwardIter first, ForwardIter last)
    {
        destroy_aux(first, last, std::is_trivially_destructible<
                     typename iterator_traits<ForwardIter>::value_type>{});
    }

    inline void destroy(char*, char*) {}
    inline void destroy(int*, int*) {}
    inline void destroy(long*, long*) {}
    inline void destroy(float*, float*) {}
    inline void destroy(double*, double*) {}

#ifdef __STL_HAS_WCHAR_T
inline void _Destroy(wchar_t*, wchar_t*) {}
#endif /* __STL_HAS_WCHAR_T */
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif