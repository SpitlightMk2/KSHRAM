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

#include <iostream>
#include <vector>

#include "src/Entry/entry.h"

/// 单个小节
class Measure
{
protected:
    /// 拍号（分子）
    int time_sig_numer;
    /// 拍号（分母）
    int time_sig_denom;
    /// 小节起始时间（单位：1/48拍）
    int start_time;
    /// 单个Entry的时长
    int entry_timespan;
    /// Entry的容器
    std::vector<Entry> entries;

protected:
    void InitializeAfterImport();
    inline void UpdateEntryTimespan();

public:
    /// 生成一个空小节
    Measure();
    /// 拷贝构造
    Measure(const Measure&) = default;
    /// 获得一个给定拍号的空小节
    static Measure BlankMeasure(int numer = 4, int denom = 4);

    /* #region 读取与写回 */
    /// 从ksh中导入
    void ImportFromKsh(const std::string& ksh);
    /// 导出为ksh
    std::string ExportToKsh();

    /* #endregion */

    /* #region 元数据 */
    /// 是否为空
    inline bool Empty();
    /// 获取或者设置起始时间。单位：1/48beat
    inline int& StartTime();
    /// 获取起始时间。单位：1/48beat
    inline int StartTime() const;
    /// 获取结束时间。单位：1/48beat
    inline int EndTime() const;
    /// 获取长度
    inline int Length() const;
    /// 获取单个记录的持续时长
    inline int EntryTimespan() const;
    /// 获取总时长
    inline int TotalTimespan() const;
    /// 局部时间转换为偏移下标
    inline int IndexByLocalTime(int local_time) const;
    /// 偏移下标转换为局部时间
    inline int LocalTimeByIndex(int index) const;

    /* #endregion */

    /* #region 操作 */
    /// 清空
    inline void Clear();

    /// @brief 扩张记录数量。注意：扩充后measure需仍然满足单条记录的时长是1/48拍的整数倍的条件。
    /// @param amp 倍数：单条记录扩充为多少条记录。
    void ExpandBy(int amp);

    /// @brief 扩张记录数量到每个记录的时长为指定值。
    /// @param timespan 单个记录的时长，单位为1/48拍。
    inline void ExpandToTimespan(int timespan);

    /// @brief 扩张记录数量到能满足指定的divisor值。
    /// @param divisor 目标divisor值。如16表示1/4拍为单位
    inline void ExpandToDivisor(int divisor);

    /// 下标访问记录
    inline Entry& EntryByIndex(int index);

    /// 下标访问记录
    inline const Entry& EntryByIndex(int index) const;

    /// 局部时间访问记录
    inline Entry& EntryByLocalTime(int local_time);

    /// 局部时间访问记录
    inline const Entry& EntryByLocalTime(int local_time) const;

    /// 下标访问记录
    inline Entry& operator[](int index);

    /// 下标访问记录
    inline const Entry& operator[](int index) const;

    /// 局部时间访问记录
    inline Entry& operator()(int local_time);

    /// 局部时间访问记录
    inline const Entry& operator()(int local_time) const;

    /// 设置拍号
    inline void SetTimeSignature(int numer, int denom);

    /// 获取拍号
    inline void GetTimeSignature(int& numer, int& denom) const;

    /* #endregion */

    /* #region Iterator */

    /// 固定步长迭代器（按创建时的步长递增）
    class Iterator
    {
    private:
        Measure* p_measure;
        int time_index;
        int step;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Entry;
        using difference_type = int;
        using pointer = Entry*;
        using reference = Entry&;

        friend class Measure;

    private:
        Iterator(Measure& m, int index);

    public:
        Iterator(const Iterator&) = default;

        inline Iterator& operator++();
        inline Iterator operator++(int);
        inline Iterator& operator--();
        inline Iterator operator--(int);

        inline Iterator& operator+=(int n);
        inline Iterator& operator-=(int n);

        inline Iterator operator+(int n) const;
        inline Iterator operator-(int n) const;
        inline int operator-(const Iterator& other) const;

        inline bool operator==(const Iterator& other) const;
        inline bool operator!=(const Iterator& other) const;
        inline bool operator<(const Iterator& other) const;
        inline bool operator<=(const Iterator& other) const;
        inline bool operator>(const Iterator& other) const;
        inline bool operator>=(const Iterator& other) const;

        inline Entry& operator[](int n) const;
        inline Entry& operator*() const;

        inline int Time() const;
        inline int LocalTime() const;
    };

    /// 固定步长迭代器（按创建时的步长递增）
    class ConstIterator
    {
    private:
        const Measure* p_measure;
        int time_index;
        int step;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = const Entry;
        using difference_type = int;
        using pointer = const Entry*;
        using reference = const Entry&;

        friend class Measure;

    private:
        ConstIterator(const Measure& m, int index);

    public:
        ConstIterator(const ConstIterator&) = default;

        inline ConstIterator& operator++();
        inline ConstIterator operator++(int);
        inline ConstIterator& operator--();
        inline ConstIterator operator--(int);

        inline ConstIterator& operator+=(int n);
        inline ConstIterator& operator-=(int n);

        inline ConstIterator operator+(int n) const;
        inline ConstIterator operator-(int n) const;
        inline int operator-(const ConstIterator& other) const;

        inline bool operator==(const ConstIterator& other) const;
        inline bool operator!=(const ConstIterator& other) const;
        inline bool operator<(const ConstIterator& other) const;
        inline bool operator<=(const ConstIterator& other) const;
        inline bool operator>(const ConstIterator& other) const;
        inline bool operator>=(const ConstIterator& other) const;

        inline const Entry& operator[](int n) const;
        inline const Entry& operator*() const;

        inline int Time() const;
        inline int LocalTime() const;
    };

    /* #endregion */

    /* #region begin/end */

    /// 获得起始迭代器
    inline Iterator begin();
    /// 获得起始常量迭代器
    inline ConstIterator begin() const;
    /// 获得终止迭代器
    inline Iterator end();
    /// 获得终止常量迭代器
    inline ConstIterator end() const;

    /* #endregion */

public:
    friend std::istream& operator>>(std::istream&, Measure&);
    friend std::ostream& operator<<(std::ostream&, const Measure&);
};

/// 将小节输出到文本流
std::istream& operator>>(std::istream&, Measure&);
/// 从文本流读取
std::ostream& operator<<(std::ostream&, const Measure&);

/* INLINE FUNCTIONS */

#include "measure_inline.h"