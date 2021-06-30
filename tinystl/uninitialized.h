#pragma once

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "typetraits.h"
#include "util.h"

namespace mystl {
    /*****************************************************************************************/
    // copy_n
    // 把 [first, first + n)区间上的元素拷贝到 [result, result + n)上
    // 返回一个 pair 分别指向拷贝结束的尾部
    /*****************************************************************************************/
    template <class InputIter, class Size, class OutputIter>
    mystl::pair<InputIter, OutputIter>
    unchecked_copy_n(InputIter first, Size n, OutputIter result, mystl::input_iterator_tag)
    {
        for (; n > 0; --n, ++first, ++result)
        {
            *result = *first;
        }
        return mystl::pair<InputIter, OutputIter>(first, result);
    }

    template <class RandomIter, class Size, class OutputIter>
    mystl::pair<RandomIter, OutputIter>
    unchecked_copy_n(RandomIter first, Size n, OutputIter result, 
                    mystl::random_access_iterator_tag)
    {
        auto last = first + n;
        return mystl::pair<RandomIter, OutputIter>(last, mystl::copy(first, last, result));
    }

    template <class InputIter, class Size, class OutputIter>
    mystl::pair<InputIter, OutputIter> 
    copy_n(InputIter first, Size n, OutputIter result)
    {
        return unchecked_copy_n(first, n, result, iterator_category(first));
    }

    /*
     * uninitialized_copy
     * 复制 [first, last) 上到 result 起始的位置
     */
    template <class InputIter, class ForwardIter>
    ForwardIter __uninitialized_copy(InputIter first, InputIter last, ForwardIter result, std::true_type)
    {
        return mystl::copy(first, last, result);
    }

    template <class InputIter, class ForwardIter>
    ForwardIter __uninitialized_copy(InputIter first, InputIter last, ForwardIter result, std::false_type)
    {
        auto cur = result;
        try {
            for (; first != last; ++first, ++cur) {
                mystl::construct(&*cur, *first);
            }
        }
        catch (...) {
            for (; result != cur; ++result) {
                mystl::destroy(&*result);
            }
        }

        return cur;
    }

    template <class InputIter, class ForwardIter>
    ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
    {
        return mystl::__uninitialized_copy(first, last, result,
                                            std::is_trivially_copy_assignable<
                                            typename iterator_traits<ForwardIter>::
                                            value_type>{});
    }

    /*
     * uninitialized copy n
     * 拷贝 [first, first + n) 上的内容复制到 result 起始的空间，返回复制结束的位置
     */
    template <class InputIter, class Size, class ForwardIter>
    ForwardIter unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::true_type)
    {
        return mystl::copy_n(first, n, result).second;
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type)
    {
        auto cur = result;
        try
        {
            for (; n > 0; --n, ++cur, ++first)
            {
            mystl::construct(&*cur, *first);
            }
        }
        catch (...)
        {
            for (; result != cur; ++result)
            mystl::destroy(&*result);
        }
        return cur;
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result)
    {
        return mystl::unchecked_uninit_copy_n(first, n, result,
                                             std::is_trivially_copy_assignable<
                                             typename iterator_traits<InputIter>::
                                             value_type>{});
    }

    /*
     * uninitialized_fill
     * 在 [first, last) 区间内填充元素值
     */
    template <class ForwardIter, class T>
    void __uninitialized_fill(ForwardIter first, ForwardIter last, const T &value, std::true_type)
    {
        mystl::fill(first, last, value);
    }

    template <class ForwardIter, class T>
    void __uninitialized_fill(ForwardIter first, ForwardIter last, const T &value, std::false_type)
    {
        auto cur = first;
        try {
            for (; cur != last; ++cur) {
                mystl::construct(&*cur, value);
            }
        }
        catch (...) {
            for (; first != cur; ++first) 
                mystl::destroy(&*first);
        }
    }

    template <class ForwardIter, class T>
    void uninitialized_fill(ForwardIter first, ForwardIter last, const T &value)
    {
        mystl::__uninitialized_fill(first, last, value,
                                    std::is_trivially_copy_assignable<
                                    typename iterator_traits<ForwardIter>::
                                    value_type>{});
    }

    /*
     * uninitialized_fill_n
     * 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
    */
    template <class ForwardIter, class Size, class T>
    ForwardIter 
    unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::true_type)
    {
        return mystl::fill_n(first, n, value);
    }

    template <class ForwardIter, class Size, class T>
    ForwardIter 
    unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::false_type)
    {
        auto cur = first;
        try
        {
            for (; n > 0; --n, ++cur)
            {
            mystl::construct(&*cur, value);
            }
        }
        catch (...)
        {
            for (; first != cur; ++first)
            mystl::destroy(&*first);
        }
        return cur;
    }

    template <class ForwardIter, class Size, class T>
    ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value)
    {
        return mystl::unchecked_uninit_fill_n(first, n, value, 
                                                std::is_trivially_copy_assignable<
                                                typename iterator_traits<ForwardIter>::
                                                value_type>{});
    }

    /*
     * uinitialized move
     * 把 [first, last) 上的内容移动到 result 起始处的空间
     */
    template <class InputIter, class ForwardIter>
    ForwardIter __uninitialized_move(InputIter first, InputIter last, ForwardIter result, std::true_type)
    {
        return mystl::move(first, last, result);
    }

    template <class InputIter, class ForwardIter>
    ForwardIter __uninitialized_move(InputIter first, InputIter last, ForwardIter result, std::false_type)
    {
        ForwardIter cur = result;
        try {
            for (; first != last;  ++first, ++cur) {
                mystl::construct(&*cur, mystl::move(*first));
            }
        }
        catch (...) {
            mystl::destroy(result, cur);
        }

        return cur;
    }

    template <class InputIter, class ForwardIter>
    ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result)
    {
        return mystl::__uninitialized_move(first, last, result,
                                           std::is_trivially_move_assignable<
                                           typename iterator_traits<InputIter>::
                                           value_type>{});
    }

    /*
     * uninitialized_move_n
     * 移动 [first, first + n) 的内容到 result 为起始的空间
     */
    template <class InputIter, class Size, class ForwardIter>
    ForwardIter unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::true_type)
    {
        return mystl::move(first, first + n, result);
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::false_type)
    {
        auto cur = result;
        try
        {
            for (; n > 0; --n, ++first, ++cur)
            {
                mystl::construct(&*cur, mystl::move(*first));
            }
        }
        catch (...)
        {
            for (; result != cur; ++result)
                mystl::destroy(&*result);
            throw;
        }
        return cur;
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
    {
        return mystl::unchecked_uninit_move_n(first, n, result,
                                             std::is_trivially_move_assignable<
                                             typename iterator_traits<InputIter>::
                                             value_type>{});
    }
};