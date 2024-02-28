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

#include <cmath>
#include <iostream>
#include <map>
#include <vector>

#include "src/IndexList/pair.h"
#include "src/misc/utilities.h"


/* #region IndexList */

/// 形如 [时间: 值1, 值2] 的存储表。里面使用的是PairEntry，所以可以放单个值或者两个值。
template <typename T>
class IndexList
{
protected:
    std::map<int, PairEntry<T>> map_;

protected:
    // 将记录值转为double。对于不可转换的值，处理为fallback。
    static double toDouble(const T& val, double fallback);

public:
    using Component = typename std::pair<const int, PairEntry<T>>;
    using Iterator = typename std::map<int, PairEntry<T>>::iterator;
    using ConstIterator = typename std::map<const int, PairEntry<T>>::const_iterator;
    // 模板Tag
    using IsIndexList = void;

public:
    inline IndexList();
    /// 自动将T2类型转换为T类型的拷贝构造
    template <typename T2>
    inline IndexList(const IndexList<T2>& other);

    /// 插入单个元素
    inline void insert(int key, const T& item);
    /// 插入单个元素
    inline void insert(int key, T&& item);
    /// 插入具有突变的单个元素
    inline void insert(int key, const T& before, const T& after);
    /// 插入具有突变的单个元素
    inline void insert(int key, T&& before, T&& after);
    /// 插入PairEntry
    inline void insert(int key, const PairEntry<T>& item);
    /// 插入PairEntry
    inline void insert(int key, PairEntry<T>&& item);
    /// 插入pair包装的单个元素
    inline void insert(const std::pair<const int, T>& item);
    /// 插入pair包装的单个元素
    inline void insert(std::pair<const int, T>&& item);
    /// 插入pair包装的具有突变的单个元素
    inline void insert(const std::pair<const int, std::pair<T, T>>& item);
    /// 插入pair包装的具有突变的单个元素
    inline void insert(std::pair<const int, std::pair<T, T>>&& item);
    /// 插入pair包装的PairEntry元素
    inline void insert(const std::pair<const int, PairEntry<T>>& item);
    /// 插入pair包装的PairEntry元素
    inline void insert(std::pair<const int, PairEntry<T>>&& item);
    /// 插入一整个map
    inline void insert(const std::map<int, T>& map);
    /// 插入一整个map
    inline void insert(const std::map<int, std::pair<T, T>>& map);
    /// 插入一整个map
    inline void insert(const std::map<int, PairEntry<T>>& map);
    /// 插入一整个IndexList
    inline void insert(const IndexList& index_list);

    /// 删除单个元素
    inline void erase(int key);
    /// 删除单个元素
    inline void erase(const Iterator& iter);
    /// 删除范围内的所有元素
    inline void erase(int start, int end);
    /// 清除所有内容
    inline void clear();

    /// 获取长度
    inline size_t size() const;
    /// 查询是否为空
    inline bool empty() const;
    /// 查询是否有某个key
    inline bool hasKey(int key) const;
    /// 查询值
    inline PairEntry<T>& getVal(int key);
	/// 查询值
	inline const PairEntry<T>& getVal(int key) const;
	/// 查询值
    inline PairEntry<T>& operator[](int key);
	/// 查询值
	inline const PairEntry<T>& operator[](int key) const;
    /// 获取起始值
    inline const T& startVal(int key) const;
    /// 获取结束值
    inline const T& endVal(int key) const;
    /// 修改为单个值
    inline void setUniformVal(int key, const T& val);
    /// 修改起始值
    inline void setStartVal(int key, const T& val);
    /// 修改结束值
    inline void setEndVal(int key, const T& val);
    /// 查询某个记录的起始值是否等于结束值
    inline bool valIsUniform(int key);

    /// 改变某个元素的key, 同时让iter指向改变key的同一个元素
    inline void changeKey(Iterator& iter, int new_key);
    /// 改变某个元素的key
    inline void changeKey(const Iterator& iter, int new_key);

    /// 获取指定key的元素迭代器
    inline Iterator find(int key);
    /// 获取指定时间之前最近的元素迭代器（包括time）
    inline Iterator prevItem(int time);
    /// 获取指定时间之前最近的元素迭代器（包括time）
    inline ConstIterator prevItem(int time) const;
    /// 获取指定时间之后最近的元素迭代器（不包括time）
    inline Iterator nextItem(int time);
    /// 获取指定时间之后最近的元素迭代器（不包括time）
    inline ConstIterator nextItem(int time) const;

    /// 获取第一个元素
    inline Component& first();
    /// 获取第一个元素
    inline const Component& first() const;
    /// 获取最后一个元素
    inline Component& last();
    /// 获取最后一个元素
    inline const Component& last() const;

    inline Iterator begin();
    inline Iterator end();
    inline ConstIterator begin() const;
    inline ConstIterator end() const;

    /// 获取指定时间区间的子列表。如果边界上没有元素，会将指定时间之外最近的元素包括在表中。
    IndexList<T> surroundingSublist(int start, int end = -1) const;
    /// 获取指定时间区间的子列表。子列表不包含给定区间之外的任何内容（但是包括给定区间边界上的内容）。
    IndexList<T> innerSublist(int start, int end = -1) const;
    /// 获取指定时间区间的子列表，并且通过线性插值得到指定时间首尾的两个值
    IndexList<T> clamp(int start, int end = -1) const;
    /// 整体偏移
    IndexList<T> offset(int offset) const;

    /// 线性插值获得当前位置的值。对于不可转为数字的关键点，视为fallback值。
    /// 默认的fallback值为NAN，也就意味着有关的插值结果均为NAN。
    /// 如果插值位置恰好是一个具有突变记录的点，返回的是突变记录突变后的值。
    double LinearInterpolate(int time, double fallback = NAN) const;

    /// 线性插值获得一组时间的值。这里的time_vec应为升序，并且时间间隔不要过长。
    /// 返回值遵循单点插值的逻辑。
    std::vector<double> LinearInterpolate(const std::vector<int>& time_vec, double fallback = NAN) const;

    /// 将列表内容输出到std::ostream
    friend std::ostream& operator<<(std::ostream& os, const IndexList<T>& lst)
    {
        for (typename IndexList<T>::ConstIterator iter = lst.map_.begin(); iter != lst.map_.end(); ++iter)
        {
            if (iter != lst.map_.begin())
            {
                os << std::endl;
            }
            IndexList<T>::Component item = *iter;
            if (item.second.isSame())
            {
                os << item.first << " : " << item.second.first();
            }
            else
            {
                os << item.first << " : " << item.second.first() << ", " << item.second.second();
            }
        }

        return os;
    }
};

/* #endregion IndexList */

/* #region IndexList实现 */

template <typename T>
inline IndexList<T>::IndexList()
{
    // 啥也不做
}

template <typename T>
template <typename T2>
inline IndexList<T>::IndexList(const IndexList<T2>& other)
{
    for (auto iter = other.begin(); iter != other.end(); ++iter)
    {
        this->map_.insert_or_assign(iter->first, static_cast<PairEntry<T>>(iter->second));
    }
}

template <typename T>
inline void IndexList<T>::insert(int key, const T& item)
{
    this->map_.insert_or_assign(key, PairEntry<T>(item));
}

template <typename T>
inline void IndexList<T>::insert(int key, T&& item)
{
    this->map_.insert_or_assign(key, PairEntry<T>(std::move(item)));
}

template <typename T>
inline void IndexList<T>::insert(int key, const T& before, const T& after)
{
    this->map_.insert_or_assign(key, PairEntry<T>(before, after));
}

template <typename T>
inline void IndexList<T>::insert(int key, T&& before, T&& after)
{
    this->map_.insert_or_assign(key, PairEntry<T>(std::move(before), std::move(after)));
}

template <typename T>
inline void IndexList<T>::insert(int key, const PairEntry<T>& item)
{
    this->map_.insert_or_assign(key, item);
}

template <typename T>
inline void IndexList<T>::insert(int key, PairEntry<T>&& item)
{
    this->map_.insert_or_assign(key, std::move(item));
}

template <typename T>
inline void IndexList<T>::insert(const std::pair<const int, T>& item)
{
    this->insert(item.first, PairEntry<T>(item.second));
}

template <typename T>
inline void IndexList<T>::insert(std::pair<const int, T>&& item)
{
    this->insert(item.first, PairEntry<T>(item.second));
}

template <typename T>
inline void IndexList<T>::insert(const std::pair<const int, std::pair<T, T>>& item)
{
    this->insert(item.first, PairEntry<T>(item.second.first, item.second.second));
}

template <typename T>
inline void IndexList<T>::insert(std::pair<const int, std::pair<T, T>>&& item)
{
    this->insert(item.first, PairEntry<T>(item.second.first, item.second.second));
}

template <typename T>
inline void IndexList<T>::insert(const std::pair<const int, PairEntry<T>>& item)
{
    this->insert(item.first, item.second);
}

template <typename T>
inline void IndexList<T>::insert(std::pair<const int, PairEntry<T>>&& item)
{
    this->insert(item.first, item.second);
}

template <typename T>
inline void IndexList<T>::insert(const std::map<int, T>& map)
{
    for (auto& item : map)
    {
        this->map_.insert(item.first, PairEntry<T>(item.second));
    }
}

template <typename T>
inline void IndexList<T>::insert(const std::map<int, std::pair<T, T>>& map)
{
    for (auto& item : map)
    {
        this->map_.insert(item.first, PairEntry<T>(item.second.first, item.second.second));
    }
}

template <typename T>
inline void IndexList<T>::insert(const std::map<int, PairEntry<T>>& map)
{
    this->map_.insert(map.begin(), map.end());
}

template <typename T>
inline void IndexList<T>::insert(const IndexList& index_list)
{
    this->map_.insert(index_list.map_.begin(), index_list.map_.end());
}

template <typename T>
inline void IndexList<T>::erase(int key)
{
    this->map_.erase(key);
}

template <typename T>
inline void IndexList<T>::erase(const Iterator& iter)
{
    this->map_.erase(iter);
}

template <typename T>
inline void IndexList<T>::erase(int start, int end)
{
    this->map_.erase(this->map_.lower_bound(start), this->map_.lower_bound(end));
}

template <typename T>
inline void IndexList<T>::clear()
{
    this->map_.clear();
}

template <typename T>
inline size_t IndexList<T>::size() const
{
    return this->map_.size();
}

template <typename T>
inline bool IndexList<T>::empty() const
{
    return this->map_.empty();
}

template <typename T>
inline bool IndexList<T>::hasKey(int key) const
{
    return this->map_.find(key) != this->map_.end();
}

template <typename T>
inline PairEntry<T>& IndexList<T>::getVal(int key)
{
    return this->map_[key];
}

template <typename T>
inline const PairEntry<T>& IndexList<T>::getVal(int key) const
{
    return this->map_[key];
}

template <typename T>
inline PairEntry<T>& IndexList<T>::operator[](int key)
{
    return this->map_[key];
}

template <typename T>
inline const PairEntry<T>& IndexList<T>::operator[](int key) const
{
    return this->map_[key];
}

template <typename T>
inline const T& IndexList<T>::startVal(int key) const
{
    return this->map_.at(key).first();
}

template <typename T>
inline const T& IndexList<T>::endVal(int key) const
{
    return this->map_.at(key).second();
}

template <typename T>
inline bool IndexList<T>::valIsUniform(int key)
{
    return this->map_[key].isSame() || this->startVal(key) == this->endVal(key);
}

template <typename T>
inline void IndexList<T>::setUniformVal(int key, const T& val)
{
	this->map_[key].setUniform(val);
}

template <typename T>
inline void IndexList<T>::setStartVal(int key, const T& val)
{
	this->map_[key].setFirst(val);
}

template <typename T>
inline void IndexList<T>::setEndVal(int key, const T& val)
{
	this->map_[key].setSecond(val);
}

template <typename T>
inline void IndexList<T>::changeKey(IndexList::Iterator& iter, int new_key)
{
    this->insert(new_key, iter->second);
    this->map_.erase(iter);
    iter = this->map_.find(new_key);
}

template <typename T>
inline void IndexList<T>::changeKey(const IndexList::Iterator& iter, int new_key)
{
    this->insert(new_key, iter->second);
    this->map_.erase(iter);
}

template <typename T>
inline typename IndexList<T>::Iterator IndexList<T>::find(int key)
{
    return this->map_.find(key);
}

template <typename T>
inline typename IndexList<T>::Iterator IndexList<T>::prevItem(int time)
{
    auto before_iter = this->map_.upper_bound(time);
    if (before_iter == this->map_.begin())
    {
        return this->map_.end();
    }
    else
    {
        --before_iter;
        return before_iter;
    }
}

template <typename T>
inline typename IndexList<T>::ConstIterator IndexList<T>::prevItem(int time) const
{
    auto before_iter = this->map_.upper_bound(time);
    if (before_iter == this->map_.begin())
    {
        return this->map_.end();
    }
    else
    {
        --before_iter;
        return before_iter;
    }
}

template <typename T>
inline typename IndexList<T>::Iterator IndexList<T>::nextItem(int time)
{
    return this->map_.upper_bound(time);
}

template <typename T>
inline typename IndexList<T>::ConstIterator IndexList<T>::nextItem(int time) const
{
    return this->map_.upper_bound(time);
}

template <typename T>
inline typename IndexList<T>::Component& IndexList<T>::first()
{
    return *(this->map_.begin());
}

template <typename T>
inline const typename IndexList<T>::Component& IndexList<T>::first() const
{
    return *(this->map_.begin());
}

template <typename T>
inline typename IndexList<T>::Component& IndexList<T>::last()
{
    return *(--this->map_.end());
}

template <typename T>
inline const typename IndexList<T>::Component& IndexList<T>::last() const
{
    return *(--this->map_.end());
}

template <typename T>
inline typename IndexList<T>::Iterator IndexList<T>::begin()
{
    return this->map_.begin();
}

template <typename T>
inline typename IndexList<T>::Iterator IndexList<T>::end()
{
    return this->map_.end();
}

template <typename T>
inline typename IndexList<T>::ConstIterator IndexList<T>::begin() const
{
    return this->map_.begin();
}

template <typename T>
inline typename IndexList<T>::ConstIterator IndexList<T>::end() const
{
    return this->map_.end();
}

template <typename T>
IndexList<T> IndexList<T>::surroundingSublist(int start, int end) const
{
    if (end == -1)
    {
        end = this->last().first;
    }

    IndexList<T> output;

    IndexList<T>::ConstIterator start_iter = this->prevItem(start);
    if (start_iter == this->map_.end())
    {
        start_iter = this->nextItem(start - 1);
    }

    IndexList<T>::ConstIterator end_iter = this->nextItem(end - 1);
    for (IndexList<T>::ConstIterator iter = start_iter; iter != end_iter; ++iter)
    {
        output.insert(*iter);
    }
    if (end_iter != this->map_.end())
    {
        output.insert(*end_iter);
    }

    return output;
}

template <typename T>
IndexList<T> IndexList<T>::innerSublist(int start, int end) const
{
    if (end == -1)
    {
        end = this->last().first;
    }

    IndexList<T> output;

    IndexList<T>::ConstIterator start_iter = this->nextItem(start - 1);
    IndexList<T>::ConstIterator end_iter = this->prevItem(end);
    for (IndexList<T>::ConstIterator iter = start_iter; iter != end_iter; ++iter)
    {
        output.insert(*iter);
    }
    if (end_iter != this->map_.end())
    {
        output.insert(*end_iter);
    }

    return output;
}

template <typename T>
IndexList<T> IndexList<T>::clamp(int start, int end) const
{
    IndexList<T> output = this->innerSublist(start, end);

    if(this->first().first < start && !this->hasKey(start))
    {
        T val = this->LinearInterpolate(start);
        output.insert(start, val);
    }

    if(end >= 0 && this->last().first > end && !this->hasKey(end))
    {
        T val = this->LinearInterpolate(end);
        output.insert(end, val);
    }

    return output;
}

template <typename T>
IndexList<T> IndexList<T>::offset(int offset) const
{
    IndexList<T> output;
    for (auto item : this->map_)
    {
        output.insert(item.first + offset, item.second);
    }

    return output;
}

template <typename T>
double IndexList<T>::toDouble(const T& val, double fallback)
{
    if constexpr (std::is_same_v<T, std::string>)
    {
        if (IsFloat(val))
        {
            return stod(val);
        }
        else
        {
            return fallback;
        }
    }
    else if constexpr (std::is_arithmetic_v<T>)
    {
        return static_cast<double>(val);
    }
    else
    {
        return fallback;
    }
}

template <typename T>
double IndexList<T>::LinearInterpolate(int time, double fallback) const
{
    auto before_iter = this->map_.upper_bound(time);
    // 时间早于第一条记录的情况
    if (before_iter == this->map_.begin())
    {
        auto before = *(before_iter);
        return toDouble(before.second.first(), fallback);
    }
    // else
    auto before = *(--before_iter);

    auto after_iter = this->map_.upper_bound(time);
    // 时间晚于最后一条记录的情况
    if (after_iter == this->map_.end())
    {
        auto after = *(--after_iter);
        return toDouble(after.second.second(), fallback);
    }
    // else
    auto after = *(after_iter);

    // before.second 是 PairEntry
    double start = toDouble(before.second.second(), fallback);
    double end = toDouble(after.second.first(), fallback);

    if (std::isnan(start) && std::isnan(end))
    {
        return NAN;
    }
    else if(std::isnan(start)){
        return end;
    }
    else if(std::isnan(end)){
        return start;
    }
    else
    {
        return LinearInterpolation(before.first, start, after.first, end, time);
    }
}

template <typename T>
std::vector<double> IndexList<T>::LinearInterpolate(const std::vector<int>& time_vec, double fallback) const
{
    // 空输入特殊处理：空输出
    if (time_vec.size() == 0)
    {
        return std::vector<double>();
    }
    // 自身为空，插值表返回全fallback
    if (this->size() == 0)
    {
        std::vector<double> zero(time_vec.size());
        std::fill(zero.begin(), zero.end(), fallback);
        return zero;
    }
    // else

    int first_time_point = time_vec[0];
    // 是否在第一个记录之前？
    bool time_is_before_first_index = false;
    // 是否在最后一个记录之后？
    bool time_is_after_last_index = false;

    // 设置两个迭代器的初始位置，并更新两个flag
    auto before_iter = this->map_.upper_bound(first_time_point);
    if (before_iter == this->map_.begin())
    {
        time_is_before_first_index = true;
        // 注意：由于map的迭代器不能先移到合法位置以外的部分，所以首次移动迭代器时before不要动。
    }
    else
    {
        --before_iter;
    }
    auto after_iter = this->map_.upper_bound(first_time_point);
    if (after_iter == this->map_.end())
    {
        time_is_after_last_index = true;
    }

    // 创建返回值列表
    const int kSize = static_cast<int>(time_vec.size());
    std::vector<double> output(kSize);

    // 逐个插值
    for (int i = 0; i < kSize; ++i)
    {
        int time = time_vec[i];
        // 将两迭代器向前递进
        while (!time_is_after_last_index && after_iter->first <= time)
        {
            if (!time_is_before_first_index)
            {
                ++before_iter;
            }
            else
            {
                time_is_before_first_index = false;
            }
            ++after_iter;

            if (after_iter == this->map_.end())
            {
                time_is_after_last_index = true;
            }
        }

        // 插值
        if (time_is_before_first_index)
        {
            output[i] = toDouble(after_iter->second.first(), fallback);
        }
        else if (time_is_after_last_index)
        {
            output[i] = toDouble(before_iter->second.second(), fallback);
        }
        else
        {
            double start = toDouble(before_iter->second.second(), fallback);
            double end = toDouble(after_iter->second.first(), fallback);

            if (std::isnan(start) && std::isnan(end))
            {
                output[i] = NAN;
            }
            else if(std::isnan(start)){
                output[i] = end;
            }
            else if(std::isnan(end)){
                output[i] = start;
            }
            else
            {
                output[i] = LinearInterpolation(before_iter->first, start, after_iter->first, end, time);
            }
        }
    }

    return output;
}

/* #endregion IndexList实现 */
