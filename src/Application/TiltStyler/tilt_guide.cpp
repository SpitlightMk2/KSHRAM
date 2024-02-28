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

#include "tilt_guide.h"

#include "Shared/knob_query.h"
#include "src/IndexList/index_list_window.h"

using namespace std;

namespace TiltStyler_Core
{

void RegulateTiltGuide(IndexList<double>& guide)
{
    if (guide.size() < 3)
    {
        return;
    }
    if (guide.first().second.first() == guide.first().second.second())
    {
        guide.first().second.setUniform(guide.first().second.first());
    }
    if (guide.last().second.first() == guide.last().second.second())
    {
        guide.last().second.setUniform(guide.last().second.first());
    }

    IndexListWindow<double, 3> guide_window(guide);

    while (!guide_window.IsRightOutOfBound())
    {
        auto& center = guide_window[1];
        if (center.second.first() == center.second.second())
        {
            center.second.setUniform(center.second.first());
        }

        int center_key = 0;
        double val = center.second.first();
        if (center.second.isSame() && val == guide_window[0].second.second() && val == guide_window[2].second.first())
        {
            RemoveAndIncrement(guide, guide_window, 1);
        }
        else
        {
            guide_window.MoveForward();
        }
    }
}

IndexList<double> MergeTiltGuide(IndexList<double>& a, IndexList<double>& b)
{
    IndexList<double> output;
    if (a.empty())
    {
        return b;
    }
    if (b.empty())
    {
        return a;
    }
    auto iter_a = a.begin();
    auto end_a = a.end();
    auto iter_b = b.begin();
    auto end_b = b.end();

    while (iter_a != end_a || iter_b != end_b)
    {
        int key = min(iter_a->first, iter_b->first);

        if (iter_b == end_b)
        {
            double pos_b = b.last().second.second();
            output.insert(iter_a->first, iter_a->second + pos_b);
            ++iter_a;
        }
        else if (iter_a == end_a)
        {
            double pos_a = a.last().second.second();
            output.insert(iter_b->first, iter_b->second + pos_a);
            ++iter_b;
        }
        else if (iter_a->first == iter_b->first)
        {
            output.insert(iter_a->first, iter_a->second + iter_b->second);
            ++iter_a;
            ++iter_b;
        }
        else if (iter_a->first < iter_b->first)
        {
            double pos_b = b.LinearInterpolate(iter_a->first);
            output.insert(iter_a->first, iter_a->second + pos_b);
            ++iter_a;
        }
        else /* iter_b->first < iter_a->first */
        {
            double pos_a = a.LinearInterpolate(iter_b->first);
            output.insert(iter_b->first, iter_b->second + pos_a);
            ++iter_b;
        }
    }

    return output;
}

IndexList<double> MakeTiltGuide(IndexList<double>& knob, int start, int end, ITiltPos& tilt_func)
{
    IndexList<double> output;

    auto iter = knob.prevItem(start);
    auto iter_end = knob.nextItem(end);

    bool empty_head = false, empty_tail = false;
    if (iter->second.isSame() && iter->second.first() == -1 && iter->first == 0)
    {
        empty_head = true;
    }
    if (iter_end == knob.end())
    {
        // --iter_end;
        empty_tail = true;
    }

    bool has_knob = (iter->second.first() != -1);

    for (; iter != iter_end; ++iter)
    {
        // 空记录，pass
        if (iter->second.first() == -1 && iter->second.second() == -1)
        {
            continue;
        }
        // 裸直角
        if (!has_knob && IsSlamToVoid(iter))
        {
            // 准备时的变化
            int last_time = output.empty() ? -128 : output.last().first;
            double slam_from = tilt_func(iter->second.first());
            if (iter->first - last_time <= 96)
            {
                output.last().second.setSecond(slam_from);
            }
            else if (slam_from != 0)
            {
                output.insert(iter->first - 96, 0, slam_from);
            }

            // 直角时的变化
            output.insert(iter->first, slam_from, 0);
            ++iter;
            if (iter == iter_end)
            {
                break;
            }
            has_knob = false;
            tilt_func.setKnobState(false);
        }
        // 带有直角的末尾
        else if (IsSlamToVoid(iter))
        {
            double slam_from = tilt_func(iter->second.first());
            output.insert(iter->first, slam_from, 0);
            ++iter;
            if (iter == iter_end)
            {
                break;
            }
            has_knob = false;
            tilt_func.setKnobState(false);
        }
        // 通常结尾
        else if (has_knob && iter->second.second() == -1)
        {
            double from_pos = tilt_func(iter->second.first());
            output.insert(iter->first, from_pos, 0);
            has_knob = false;
            tilt_func.setKnobState(false);
        }
        // 开头
        else if (!has_knob && iter->second.first() != -1)
        {
            tilt_func.setKnobState(true);
            int last_time = output.empty() ? -128 : output.last().first;
            double prep_val = tilt_func(iter->second.first());
            double start_val = tilt_func(iter->second.second());
            if (iter->first - last_time <= 96)
            {
                output.last().second.setSecond(prep_val);
            }
            else if (prep_val != 0)
            {
                output.insert(iter->first - 96, 0, prep_val);
            }

            if (iter->second.first() == iter->second.second())
            {
                output.insert(iter->first, prep_val);
            }
            else
            {
                output.insert(iter->first, prep_val, start_val);
                ++iter;
                if (iter == iter_end)
                {
                    break;
                }
            }
            has_knob = true;
        }
        // 直角节点
        else if (has_knob && IsSlam(iter))
        {
            double slam_from = tilt_func(iter->second.first());
            double slam_to = tilt_func(iter->second.second());
            output.insert(iter->first, slam_from, slam_to);
            ++iter;
            if (iter == iter_end)
            {
                break;
            }
        }
        // 通常节点
        else if (has_knob)
        {
            double tilt_pos = tilt_func(iter->second.first());
            output.insert(iter->first, tilt_pos);
        }
    }

    if (empty_head)
    {
        int current_begin = output.first().first;
        if (start < current_begin)
        {
            output.insert(start, 0);
        }
    }

    if (empty_tail && !output.hasKey(end))
    {
        output.insert(end, 0);
    }

    RegulateTiltGuide(output);

    return output;
}

}  // namespace TiltStyler_Core