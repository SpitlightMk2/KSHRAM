#pragma once

/*
    This file is part of KSHRAM.

    KSHRAM: A command-style K-Shoot Mania chart editing toolpack.
    Copyright (C) 2024 Singular_Photon

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <array>

/* #region PairEntry声明 */

/// 存储一对数据或是单个数据的类型。如果只存储一个数据，类型会表现为存了[a,a]这样的一对。
template <typename T>
class PairEntry
{
private:
    std::array<T, 2> arr;
    bool same;

public:
    inline PairEntry() = default;
    inline PairEntry(const PairEntry& other) = default;
    inline PairEntry& operator=(const PairEntry& other) = default;
    inline PairEntry(PairEntry&& other) = default;
    inline PairEntry& operator=(PairEntry&& other) = default;

    /// 自动将T2类型转换为T类型的拷贝构造
    template <typename T2>
    inline PairEntry(const PairEntry<T2>& other);

    /// 单个值构造（first = second)
    inline explicit PairEntry(const T& a);
    /// 单个值构造（first = second)
    inline explicit PairEntry(T&& a);
    /// 两个值构造
    inline explicit PairEntry(const T& a, const T& b);
    /// 两个值构造
    inline explicit PairEntry(T&& a, T&& b);
    /// std::pair构造
    inline explicit PairEntry(const std::pair<T, T>& pair);
    /// std::pair构造
    inline explicit PairEntry(std::pair<T, T>&& pair);
    /// 获取第一个值
    inline const T& first() const;
    /// 获取第二个值
    inline const T& second() const;
    /// 第一个值与第二个值是否相同？
    inline bool isSame() const;

    /// 设置为单个值（first = second)
    inline void assign(const T& a);
    /// 设置为单个值（first = second)
    inline void assign(T&& a);
    /// 设置为两个值
    inline void assign(const T& a, const T& b);
    /// 设置为两个值
    inline void assign(T&& a, T&& b);
    /// 设置为std::pair的两个值
    inline void assign(const std::pair<T, T>& pair);
    /// 设置为std::pair的两个值
    inline void assign(std::pair<T, T>&& pair);
    /// 与另一个PairEntry交换数据
    inline void swap(PairEntry& other);
    /// 设置唯一值
    inline void setUniform(const T& a);
    /// 设置唯一值
    inline void setUniform(T&& a);
    /// 设置first值
    inline void setFirst(const T& a);
    /// 设置first值
    inline void setFirst(T&& a);
    /// 设置second值
    inline void setSecond(const T& b);
    /// 设置second值
    inline void setSecond(T&& b);
    /// 设置second值为first值
    inline void setSecondAsFirst();
};

/* #endregion PairEntry声明 */

/* #region PairEntry实现 */

template <typename T>
template <typename T2>
inline PairEntry<T>::PairEntry(const PairEntry<T2>& other)
{
    if constexpr (std::is_same_v<T, std::string>)
    {
        this->same = other.isSame();
        this->arr[0] = std::to_string(other.first());
        if (!this->same)
        {
            this->arr[1] = std::to_string(other.second());
        }
    }
    else
    {
        this->same = other.isSame();
        this->arr[0] = static_cast<T>(other.first());
        if (!this->same)
        {
            this->arr[1] = static_cast<T>(other.second());
        }
    }
}

template <typename T>
inline PairEntry<T>::PairEntry(const T& a)
{
    this->arr[0] = a;
    this->same = true;
}

template <typename T>
inline PairEntry<T>::PairEntry(T&& a)
{
    this->arr[0] = std::move(a);
    this->same = true;
}

template <typename T>
inline PairEntry<T>::PairEntry(const T& a, const T& b)
{
    this->arr[0] = a;
    this->arr[1] = b;
    this->same = false;
}

template <typename T>
inline PairEntry<T>::PairEntry(T&& a, T&& b)
{
    this->arr[0] = std::move(a);
    this->arr[1] = std::move(b);
    this->same = false;
}

template <typename T>
inline PairEntry<T>::PairEntry(const std::pair<T, T>& pair)
{
    this->arr[0] = pair.first;
    this->arr[1] = pair.second;
    this->same = false;
}

template <typename T>
inline PairEntry<T>::PairEntry(std::pair<T, T>&& pair)
{
    this->arr[0] = std::move(pair.first);
    this->arr[1] = std::move(pair.second);
    this->same = false;
}

template <typename T>
inline const T& PairEntry<T>::first() const
{
    return this->arr[0];
}

template <typename T>
inline const T& PairEntry<T>::second() const
{
    return this->same ? this->arr[0] : this->arr[1];
}

template <typename T>
inline bool PairEntry<T>::isSame() const
{
    return this->same;
}

template <typename T>
inline void PairEntry<T>::assign(const T& a)
{
    this->arr[0] = a;
    this->same = true;
}

template <typename T>
inline void PairEntry<T>::assign(T&& a)
{
    this->arr[0] = std::move(a);
    this->same = true;
}

template <typename T>
inline void PairEntry<T>::assign(const T& a, const T& b)
{
    this->arr[0] = a;
    this->arr[1] = b;
    this->same = false;
}

template <typename T>
inline void PairEntry<T>::assign(T&& a, T&& b)
{
    this->arr[0] = std::move(a);
    this->arr[1] = std::move(b);
    this->same = false;
}

template <typename T>
inline void PairEntry<T>::assign(const std::pair<T, T>& pair)
{
    this->arr[0] = pair.first;
    this->arr[1] = pair.second;
    this->same = false;
}

template <typename T>
inline void PairEntry<T>::assign(std::pair<T, T>&& pair)
{
    this->arr[0] = std::move(pair.first);
    this->arr[1] = std::move(pair.second);
    this->same = false;
}

template <typename T>
inline void PairEntry<T>::swap(PairEntry& other)
{
    this->arr.swap(other.arr);
}

template <typename T>
inline void PairEntry<T>::setUniform(const T& a)
{
    this->arr[0] = a;
    this->same = true;
}

template <typename T>
inline void PairEntry<T>::setUniform(T&& a)
{
    this->arr[0] = a;
    this->same = true;
}

template <typename T>
inline void PairEntry<T>::setFirst(const T& a)
{
    this->arr[0] = a;
}

template <typename T>
inline void PairEntry<T>::setFirst(T&& a)
{
    this->arr[0] = a;
}

template <typename T>
inline void PairEntry<T>::setSecond(const T& b)
{
    this->arr[1] = b;
    this->same = false;
}

template <typename T>
inline void PairEntry<T>::setSecond(T&& b)
{
    this->arr[1] = b;
    this->same = false;
}

template <typename T>
inline void PairEntry<T>::setSecondAsFirst()
{
    this->same = true;
}

/* #endregion PairEntry实现 */

/* #region PairEntry的数学运算函数 */

#define REGISTER_UNARY_OPERATOR(op)                           \
    template <typename T>                                     \
    PairEntry<T> operator op(const PairEntry<T>& p)           \
    {                                                         \
        if (p.isSame())                                       \
        {                                                     \
            return PairEntry<T>(op p.first());                \
        }                                                     \
        else                                                  \
        {                                                     \
            return PairEntry<T>(op p.first(), op p.second()); \
        }                                                     \
    }

#define REGISTER_UNARY_FUNCTION(name, func)                         \
    template <typename T>                                           \
    PairEntry<T> name(const PairEntry<T>& p)                        \
    {                                                               \
        if (p.isSame())                                             \
        {                                                           \
            return PairEntry<T>(func(p.first()));                   \
        }                                                           \
        else                                                        \
        {                                                           \
            return PairEntry<T>(func(p.first()), func(p.second())); \
        }                                                           \
    }

#define REGISTER_BINARY_OPERATOR(op)                                                   \
    template <typename T>                                                              \
    PairEntry<T> operator op(const PairEntry<T>& p1, const PairEntry<T>& p2)           \
    {                                                                                  \
        if (p1.isSame() && p2.isSame())                                                \
        {                                                                              \
            return PairEntry<T>(p1.first() op p2.first());                             \
        }                                                                              \
        else                                                                           \
        {                                                                              \
            return PairEntry<T>(p1.first() op p2.first(), p1.second() op p2.second()); \
        }                                                                              \
    }                                                                                  \
    template <typename T>                                                              \
    PairEntry<T> operator op(const PairEntry<T>& p1, const T& p2)                      \
    {                                                                                  \
        if (p1.isSame())                                                               \
        {                                                                              \
            return PairEntry<T>(p1.first() op p2);                                     \
        }                                                                              \
        else                                                                           \
        {                                                                              \
            return PairEntry<T>(p1.first() op p2, p1.second() op p2);                  \
        }                                                                              \
    }                                                                                  \
    template <typename T>                                                              \
    PairEntry<T> operator op(const T& p2, const PairEntry<T>& p1)                      \
    {                                                                                  \
        if (p1.isSame())                                                               \
        {                                                                              \
            return PairEntry<T>(p1.first() op p2);                                     \
        }                                                                              \
        else                                                                           \
        {                                                                              \
            return PairEntry<T>(p1.first() op p2, p1.second() op p2);                  \
        }                                                                              \
    }

#define REGISTER_BINARY_FUNCTION(name, func)                                                   \
    template <typename T>                                                                      \
    PairEntry<T> name(const PairEntry<T>& p1, const PairEntry<T>& p2)                          \
    {                                                                                          \
        if (p1.isSame() && p2.isSame())                                                        \
        {                                                                                      \
            return PairEntry<T>(func(p1.first(), p2.first()));                                 \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            return PairEntry<T>(func(p1.first(), p2.first()), func(p1.second(), p2.second())); \
        }                                                                                      \
    }

REGISTER_UNARY_OPERATOR(-);
REGISTER_UNARY_FUNCTION(abs, abs);

REGISTER_BINARY_OPERATOR(+);
REGISTER_BINARY_OPERATOR(-);
REGISTER_BINARY_OPERATOR(*);
REGISTER_BINARY_OPERATOR(/);

REGISTER_BINARY_FUNCTION(max, std::max);
REGISTER_BINARY_FUNCTION(min, std::min);

/* #endregion */