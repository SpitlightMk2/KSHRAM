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

#include "knob_query.h"

bool IsSlam(IndexedChart& chart, Knob knob, int time)
{
    IndexList<int>& lst = chart.KnobList(knob);
    if (!lst.hasKey(time))
    {
        return false;
    }
    else
    {
        int start_val = lst.startVal(time);
        int end_val = lst.endVal(time);
        return start_val != end_val && start_val != -1 && end_val != -1;
    }
}

bool IsSlam(IndexList<int>& knob_list, int time)
{
    if (!knob_list.hasKey(time))
    {
        return false;
    }
    else
    {
        int start_val = knob_list.startVal(time);
        int end_val = knob_list.endVal(time);
        return start_val != end_val && start_val != -1 && end_val != -1;
    }
}

bool IsSlam(const IndexList<int>::Iterator& iter)
{
    return (iter->second.first() != iter->second.second() && iter->second.second() != -1);
}

bool IsSlam(const IndexList<double>::Iterator& iter)
{
    return (iter->second.first() != iter->second.second() && iter->second.second() != -1);
}

bool IsSlamToVoid(const IndexList<int>::Iterator& iter)
{
    auto next = iter;
    ++next;

    return (iter->second.first() != iter->second.second() && next->second.first() != next->second.second() &&
            next->second.first() == iter->second.second() &&
            next->second.second() == -1 && next->first - iter->first <= 6  // 时间差小于1/8拍
    );
}

bool IsSlamToVoid(const IndexList<double>::Iterator& iter)
{
    auto next = iter;
    ++next;

    return (iter->second.first() != iter->second.second() && next->second.first() != next->second.second() &&
            next->second.first() == iter->second.second() &&
            next->second.second() == -1 && next->first - iter->first <= 6  // 时间差小于1/8拍
    );
}

bool IsStart(IndexedChart& chart, Knob knob, int time)
{
    IndexList<int>& lst = chart.KnobList(knob);
    if (!lst.hasKey(time))
    {
        return false;
    }
    else
    {
        auto iter = lst.prevItem(time - 1);
        return (iter == lst.end() || iter->second.second() == -1);
    }
}

bool IsNormalEnd(IndexedChart& chart, Knob knob, int time)
{
    IndexList<int>& lst = chart.KnobList(knob);
    if (!lst.hasKey(time))
    {
        return false;
    }
    else
    {
        auto& val = lst.getVal(time);
        return val.second() == -1;
    }
}

/// @brief 查询特定时刻是否是旋钮的结束点（包括直角结束）
bool IsEnd(IndexedChart& chart, Knob knob, int time)
{
    IndexList<int>& lst = chart.KnobList(knob);
    auto iter = lst.find(time);
    if (!lst.hasKey(time))
    {
        return false;
    }
    // 通常结束
    else if (iter->second.second() == -1)
    {
        return true;
    }
    // 直角结束
    else
    {
        return IsSlamToVoid(lst.find(time));
    }
}

bool ExistsSlam(IndexedChart& chart, Knob knob, int start, int end)
{
    IndexList<int>& lst = chart.KnobList(knob);
    auto iter = lst.nextItem(start - 1);
    while (iter != lst.end() && iter->first <= end)
    {
        if (iter->second.first() != iter->second.second() && iter->second.first() != -1 && iter->second.second() != -1)
        {
            return true;
        }
        ++iter;
    }

    return false;
}

int LastSlam(IndexedChart& chart, Knob knob, int time)
{
    int output = -1;
    IndexList<int>& lst = chart.KnobList(knob);
    auto iter = lst.prevItem(time);
    while (iter != lst.end())
    {
        if (iter->second.first() != iter->second.second() && iter->second.first() != -1 && iter->second.second() != -1)
        {
            output = iter->first;
            break;
        }

        --iter;
    }

    return output;
}

bool IsLaser2x(IndexedChart& chart, Knob knob, int time)
{
    IndexList<int>& knob_list = chart.KnobList(knob);
    auto iter = knob_list.prevItem(time + 1);
    if (iter == knob_list.end())
    {
        return false;
    }

    // 找到旋钮起始点
    while (iter->second.second() != -1 && iter != knob_list.begin())
    {
        --iter;
    }
    // 如果旋钮表开头是个旋钮起点就不用加，否则加
    if (!(iter == knob_list.begin() && iter->second.second() != -1))
    {
        ++iter;
    }

    // 查是否有2x标记
    int query_time = iter->first;
    auto& laser2x_list = chart.MarkList(MarkType::Laser2x, ToSide(knob));
    if (laser2x_list.hasKey(query_time))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static double InterpolateKnobPos(IndexList<int>& lst, int time)
{
    auto prev_iter = lst.prevItem(time);
    if (prev_iter != lst.end())
    {
        while (prev_iter != lst.begin() && prev_iter->second.first() == -1)
        {
            --prev_iter;
        }
    }

    auto next_iter = lst.nextItem(time);
    while (next_iter != lst.end() && next_iter->second.first() == -1)
    {
        ++next_iter;
    }

    int prev_pos = prev_iter == lst.end() ? -1 : prev_iter->second.second();
    int next_pos = next_iter == lst.end() ? -1 : next_iter->second.first();

    // 在直角结束的位置插值视作-1
    if (next_iter->second.second() == -1 &&
        next_iter->first - prev_iter->first <= 6 &&
        IsSlam(lst, prev_iter->first))
    {
        next_pos = -1;
    }

    if (prev_pos == -1 || next_pos == -1)
    {
        return -1;
    }
    else
    {
        return LinearInterpolation(prev_iter->first, prev_pos, next_iter->first, next_pos, time);
    }
}

double KnobStartPos(IndexedChart& chart, Knob knob, int time)
{
    IndexList<int>& lst = chart.KnobList(knob);
    // 恰好有一个记录
    if (lst.hasKey(time))
    {
        // 直角且结束的时候视作-1
        if (lst.endVal(time) == -1)
        {
            auto prev = lst.prevItem(time - 1);
            if (prev->second.first() != prev->second.second() && time - prev->first <= 6)
            {
                return -1;
            }
            else
            {
                return lst.startVal(time);
            }
        }
        else
        {
            return lst.startVal(time);
        }
    }
    // 需要插值的情况
    else
    {
        return InterpolateKnobPos(lst, time);
    }
}

double KnobEndPos(IndexedChart& chart, Knob knob, int time)
{
    IndexList<int>& lst = chart.KnobList(knob);
    // 恰好有一个记录
    if (lst.hasKey(time))
    {
        // 直角且结束的时候视作-1
        if (lst.startVal(time) != lst.endVal(time))
        {
            auto next = lst.nextItem(time);
            if (next->second.second() == -1 && next->first - time <= 6)
            {
                return -1;
            }
            else
            {
                return lst.startVal(time);
            }
        }
        else
        {
            return lst.startVal(time);
        }
    }

    // 需要插值的情况
    else
    {
        return InterpolateKnobPos(lst, time);
    }
}

std::pair<int, double> NextStartPos(IndexedChart& chart, Knob knob, int time)
{
    IndexList<int>& lst = chart.KnobList(knob);
    if (lst.size() == 0)
    {
        return {-1, -1};
    }
    auto iter = lst.nextItem(time - 1);

    if (iter == lst.end())
    {
        return {-1, -1};
    }
    if (iter == lst.begin())
    {
        return {iter->first, iter->second.first()};
    }
    --iter;
    while (iter != lst.end() && iter->second.first() != -1)
    {
        ++iter;
    }
    if (iter != lst.end())
    {
        ++iter;
    }

    if (iter == lst.end())
    {
        return {-1, -1};
    }
    return {iter->first, iter->second.first()};
}

std::pair<int, double> LastEndPos(IndexedChart& chart, Knob knob, int time)
{
    IndexList<int>& lst = chart.KnobList(knob);
    if (lst.size() == 0)
    {
        return {-1, -1};
    }
    auto iter = lst.prevItem(time);

    if (iter == lst.end())
    {
        return {-1, -1};
    }
    while (iter != lst.begin() && (iter->second.second() != -1 || iter->second.first() == -1))
    {
        --iter;
    }

    if (iter == lst.end())
    {
        return {-1, -1};
    }
    return {iter->first, iter->second.first()};
}

std::pair<int, double> LastEndPos_BeforeSlam(IndexedChart& chart, Knob knob, int time)
{
    IndexList<int>& lst = chart.KnobList(knob);
    if (lst.size() == 0)
    {
        return {-1, -1};
    }
    auto iter = lst.prevItem(time);

    if (iter == lst.end())
    {
        return {-1, -1};
    }
    while (iter != lst.begin() && (iter->second.second() != -1 || iter->second.first() == -1))
    {
        --iter;
    }

    if (iter == lst.end())
    {
        return {-1, -1};
    }

    if (IsSlam(chart, knob, iter->first - 6))
    {
        --iter;
    }

    return {iter->first, iter->second.first()};
}
