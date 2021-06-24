#pragma once

#include <string.h>
#include "typetraits.h"
#include "iterator.h"
#include "util.h"

namespace mystl {
    /*
     * copy
     * 把 [first, last) 区间内的元素拷贝到 [result, result + (last - first))内
     */

    // input_iterator_tag 版本
    template <class InputIter, class OutputIter>
    OutputIter
    __copy_aux(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag)
    {
        for (; first != last; ++first, ++result) {
            *result = *first;
        }

        return result;
    }

    // random_access_iterator_tag 版本
    template<class RandomIter, class OutputIter>
    OutputIter
    __copy_aux(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag)
    {
        // 以 n 决定循环次数，速度较快
        for (auto n = last - first; n > 0; --n, ++first, ++result) {
            *result = *first;
        }

        return result;
    }

    template <class InputIter, class OutputIter>
    OutputIter
    __copy(InputIter first, InputIter last, OutputIter result)
    {
        return __copy_aux(fisrt, last, result, iterator_category(first));
    }

    // trivially copy assignable 特化版本 copy
    template <class Tp, class Up>
    typename std::enable_if<
        std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_copy_assignable<Up>::value,
        Up*>::type
    __copy(Tp *first, Tp *last, Up *result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
            std::memmove(result, first, n * sizeof(Up));

        return result + n;
    }

    template <class InputIter, class OutputIter>
    OutputIter copy(InputIter first, InputIter last, OutputIter result)
    {
        return __copy(first, last, result);
    }

    /*****************************************************************************************/
    // copy_backward
    // 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
    /*****************************************************************************************/
    // unchecked_copy_backward_cat 的 bidirectional_iterator_tag 版本
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
    {
        while (first != last)
            *--result = *--last;
        return result;
    }

    // unchecked_copy_backward_cat 的 random_access_iterator_tag 版本
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result, mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n)
            *--result = *--last;
        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 
    unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result)
    {
        return unchecked_copy_backward_cat(first, last, result,
                                            iterator_category(first));
    }

    // 为 trivially_copy_assignable 类型提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_copy_assignable<Up>::value,
    Up*>::type
    unchecked_copy_backward(Tp* first, Tp* last, Up* result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
        {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    //  将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 
    copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
    {
        return unchecked_copy_backward(first, last, result);
    }

    /*****************************************************************************************/
    // copy_n
    // 把 [first, first + n)区间上的元素拷贝到 [result, result + n)上
    // 返回一个 pair 分别指向拷贝结束的尾部
    /*****************************************************************************************/
    // template <class InputIter, class Size, class OutputIter>
    // mystl::pair<InputIter, OutputIter>
    // __copy_n(InputIter first, Size n, OutputIter result, mystl::input_iterator_tag)
    // {
    //     for (; n > 0; --n, ++first, ++result)
    //     {
    //         *result = *first;
    //     }
    //     return mystl::pair<InputIter, OutputIter>(first, result);
    // }

    // template <class RandomIter, class Size, class OutputIter>
    // mystl::pair<RandomIter, OutputIter>
    // __copy_n(RandomIter first, Size n, OutputIter result, 
    //                 mystl::random_access_iterator_tag)
    // {
    //     auto last = first + n;
    //     return mystl::pair<RandomIter, OutputIter>(last, mystl::copy(first, last, result));
    // }

    // template <class InputIter, class Size, class OutputIter>
    // mystl::pair<InputIter, OutputIter> 
    // copy_n(InputIter first, Size n, OutputIter result)
    // {
    //     return __copy_n(first, n, result, iterator_category(first));
    // }

    /*
     * fill_n
     * 从 first 位置开始填充 n 个值
     */
    template <class OutputIter, class Size, class T>
    OutputIter __fill_n(OutputIter first, Size n, const T& value)
    {
        for (; n > 0; --n, ++first)
        {
            *first = value;
        }
        return first;
    }

    // 为 one-byte 类型提供特化版本
    template <class Tp, class Size, class Up>
    typename std::enable_if<
        std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
        !std::is_same<Tp, bool>::value &&
        std::is_integral<Up>::value && sizeof(Up) == 1,
        Tp*>::type
    __fill_n(Tp* first, Size n, Up value)
    {
        if (n > 0)
        {
            std::memset(first, (unsigned char)value, (size_t)(n));
        }
        return first + n;
    }

    template <class OutputIter, class Size, class T>
    OutputIter fill_n(OutputIter first, Size n, const T& value)
    {
        return __fill_n(first, n, value);
    }

    /*
     * fill
     * 填充 [first, last) 区间
     */
    template <class ForwardIter, class T>
    void __fill(ForwardIter first, ForwardIter last, const T &value, mystl::forward_iterator_tag)
    {
        for (; first != last; ++first) {
            *first = value;
        }
    }

    template <class RandomIter, class T>
    void __fill(RandomIter first, RandomIter last, const T &value)
    {
        fill_n(first, last - first, value);
    }

    template <class ForwardIter, class T>
    void fill(ForwardIter first, ForwardIter last, const T &value)
    {
        __fill(first, last, value, iterator_category(first));
    }

    /*
     * move
     * 移动 [first, last) 区间内的元素到 [result, result + (last - first)) 内
     */
    // input iterator tag
    template <class InputIter, class OutputIter>
    OutputIter __move_aux(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag)
    {
        for (; first != last; ++first, ++result) {
            *result = mystl::move(*first);
        }
    }

    // random access iteractor tag
    template <class RandomIter, class OutputIter>
    OutputIter __move_aux(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n, ++first, ++result)
        {
            *result = mystl::move(*first);
        }

        return result;
    }

    template <class InputIter, class OutputIter>
    OutputIter __move(InputIter first, InputIter last, OutputIter result)
    {
        return __move_aux(first, last, result, iterator_category(first));
    }

    // trivially copy assignable 特化版本
    template <class Tp, class Up>
    typename std::enable_if<
      std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
      std::is_trivially_move_assignable<Up>::value,
      Up*>::type
    __move(Tp *first, Tp *last, Up *result)
    {
        const size_t n = static_cast<size_t>(last - first);
        if (n != 0) {
            std::memmove(result, first, n * sizeof(Up));
        }

        return result + n;
    }

    template <class InputIter, class OutputIter>
    OutputIter move(InputIter first, InputIter last, OutputIter result)
    {
        return __move(first, last, result);
    }
};