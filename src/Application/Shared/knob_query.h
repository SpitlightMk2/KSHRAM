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

#include "src/Chart/indexed_chart.h"

/// @brief 查询特定时刻是否存在直角旋钮（突变）
bool IsSlam(IndexedChart& chart, Knob knob, int time);

/// @brief 查询特定时刻是否存在直角旋钮（突变）
bool IsSlam(IndexList<int>& knob_list, int time);

/// @brief 查询特定记录点是否是直角旋钮（突变）
bool IsSlam(const IndexList<int>::Iterator& iter);

/// @brief 查询特定记录点是否是直角旋钮（突变）
bool IsSlam(const IndexList<double>::Iterator& iter);

/// @brief 查询特定记录点是否是直角旋钮，且直角结束后立刻断开
bool IsSlamToVoid(const IndexList<int>::Iterator& iter);

/// @brief 查询特定记录点是否是直角旋钮，且直角结束后立刻断开
bool IsSlamToVoid(const IndexList<double>::Iterator& iter);

/// @brief 查询特定时刻是否是旋钮的起始点
bool IsStart(IndexedChart& chart, Knob knob, int time);

/// @brief 查询特定时刻是否是旋钮的结束点（仅通常结束，不包括直角结束）
bool IsNormalEnd(IndexedChart& chart, Knob knob, int time);

/// @brief 查询特定时刻是否是旋钮的结束点（包括直角结束）
bool IsEnd(IndexedChart& chart, Knob knob, int time);

/// @brief 查询特定时间段内是否有直角旋钮（包括首尾位置）
bool ExistsSlam(IndexedChart& chart, Knob knob, int start, int end);

/// @brief 查询指定时刻之前最近的一处直角旋钮。如果不存在任何直角，返回-1
int LastSlam(IndexedChart& chart, Knob knob, int time);

/// @brief 查询特定时刻对应的旋钮是否是外扩的
bool IsLaser2x(IndexedChart& chart, Knob knob, int time);

/// @brief 查询特定时刻旋钮的起始位置。如果没有旋钮，返回-1。
double KnobStartPos(IndexedChart& chart, Knob knob, int time);

/// @brief 查询特定时刻旋钮的结束位置。如果没有旋钮，返回-1。
double KnobEndPos(IndexedChart& chart, Knob knob, int time);

/// @brief 查询特定时刻后首次出现旋钮的时间和起始位置
std::pair<int, double> NextStartPos(IndexedChart& chart, Knob knob, int time);

/// @brief 查询特定时刻前旋钮结束的时间和结束位置
std::pair<int, double> LastEndPos(IndexedChart& chart, Knob knob, int time);

/// @brief 查询特定时刻前旋钮结束的时间和结束位置。如果结束位置有直角，返回直角开始的位置。
std::pair<int, double> LastEndPos_BeforeSlam(IndexedChart& chart, Knob knob, int time);
