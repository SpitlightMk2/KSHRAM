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

#include "Shared/curves.h"
#include "tilt_style.h"

using namespace std;

namespace TiltStyler_Core
{

// NOTE: 参数中添加起始和结束的迭代器，以支持多段style混合

IndexList<double> TiltStyle_Null(IndexList<double>& guide, int step)
{
    return guide;
}

IndexList<double> TiltStyle_Uniform(IndexList<double>& guide, int step)
{

    IndexList<double> output;
    auto iter_curr = guide.begin();
    auto iter_next = iter_curr;
    ++iter_next;

    unique_ptr<ICurveFunctor> curve = make_unique<Sine>();
    for (; iter_next != guide.end(); ++iter_curr, ++iter_next)
    {
        // 突变
        if (!iter_curr->second.isSame())
        {
            // 计算参数
            double start_val = iter_curr->second.first();
            if (!output.empty() && output.last().first == iter_curr->first)
            {
                start_val = output.last().second.second();
            }
            double end_val = iter_curr->second.second();
            int start_time = iter_curr->first;
            int ideal_duration = 24;
            bool end_at_knob = false;

            if (!iter_next->second.isSame())
            {
                end_at_knob = (iter_next->first - start_time <= ideal_duration);
            }

            int duration = ideal_duration;
            if (!iter_next->second.isSame())
            {
                duration = min(duration, iter_next->first - start_time);
            }

            // 绘制曲线
            for (int i = 0; i < duration; i += step)
            {
                double x = 1.0 - static_cast<double>(i) / ideal_duration;
                double y = (*curve)(x);
                double pos_increment = y * (start_val - end_val);
                double pos_original = guide.LinearInterpolate(iter_curr->first + i);
                double pos = pos_original + pos_increment;
                output.insert(start_time + i, pos);
            }

            // 旋钮末尾的特别处理
            if (end_at_knob)
            {
                double x = 1.0 - static_cast<double>(duration) / ideal_duration;
                double y = (*curve)(x);
                double pos_increment = y * (start_val - end_val);
                double pos_original = guide.LinearInterpolate(iter_curr->first + duration - 1);
                double pos = pos_original + pos_increment;
                output.insert(start_time + duration, pos);
            }
            else
            {
                double x = 1.0 - static_cast<double>(duration) / ideal_duration;
                double y = (*curve)(x);
                double pos_increment = y * (start_val - end_val);
                double pos_original = guide.LinearInterpolate(iter_curr->first + duration);
                double pos = pos_original + pos_increment;
                output.insert(start_time + duration, pos);
            }
        }
        // 通常节点
        else
        {
            output.insert(*iter_curr);
        }
    }

    // 最后一个节点的处理
    // 突变
    if (!iter_curr->second.isSame())
    {
        double start_time = iter_curr->first;
        double start_val = iter_curr->second.first();
        double end_val = iter_curr->second.second();
        int duration = 24;
        unique_ptr<ICurveFunctor> curve = make_unique<SqrType2>();
        for (int i = 0; i < duration; i += step)
        {
            double x = 1.0 - static_cast<double>(i) / duration;
            double y = (*curve)(x);
            double pos_increment = y * (start_val - end_val);
            double pos_original = guide.LinearInterpolate(iter_curr->first + i);
            double pos = pos_original + pos_increment;
            output.insert(start_time + i, pos);
        }
        output.insert(start_time + duration, end_val);
    }
    // 通常节点
    else
    {
        output.insert(*iter_curr);
    }

    return output;
}

IndexList<double> TiltStyle_KSM(IndexList<double>& guide, int step)
{
    /*
    NOTE:
        在每个突变位置计算一次曲线
        检查突变后的值，如果足够长时间内都是0，那么用一排半的缓降
        否则用半拍的二次
        这之后，对原有的变化插值并叠加
    */

    IndexList<double> output;
    auto iter_curr = guide.begin();
    auto iter_next = iter_curr;
    ++iter_next;
    unique_ptr<ICurveFunctor> curve = make_unique<Sine>();

    for (; iter_next != guide.end(); ++iter_curr, ++iter_next)
    {
        // 突变
        if (!iter_curr->second.isSame())
        {
            // 计算参数
            double start_val = iter_curr->second.first();
            if (!output.empty() && output.last().first == iter_curr->first)
            {
                start_val = output.last().second.second();
            }
            double end_val = iter_curr->second.second();
            int start_time = iter_curr->first;
            int ideal_duration = 24;
            bool end_at_knob = false;

            if (iter_next->first - iter_curr->first >= 72 && iter_curr->second.second() == 0 &&
                iter_next->second.first() == 0)
            {
                ideal_duration = 72;
                curve = make_unique<Sine>();
            }
            else if (!iter_next->second.isSame())
            {
                end_at_knob = (iter_next->first - start_time <= ideal_duration);
            }

            int duration = ideal_duration;
            if (!iter_next->second.isSame())
            {
                duration = min(duration, iter_next->first - start_time);
            }

            // 绘制曲线
            for (int i = 0; i < duration; i += step)
            {
                double x = 1.0 - static_cast<double>(i) / ideal_duration;
                double y = (*curve)(x);
                double pos_increment = y * (start_val - end_val);
                double pos_original = guide.LinearInterpolate(iter_curr->first + i);
                double pos = pos_original + pos_increment;
                output.insert(start_time + i, pos);
            }
            // 旋钮末尾
            if (end_at_knob)
            {
                double x = 1.0 - static_cast<double>(duration) / ideal_duration;
                double y = (*curve)(x);
                double pos_increment = y * (start_val - end_val);
                double pos_original = guide.LinearInterpolate(iter_curr->first + duration - 1);
                double pos = pos_original + pos_increment;
                output.insert(start_time + duration, pos);
            }
            else
            {
                double x = 1.0 - static_cast<double>(duration) / ideal_duration;
                double y = (*curve)(x);
                double pos_increment = y * (start_val - end_val);
                double pos_original = guide.LinearInterpolate(iter_curr->first + duration);
                double pos = pos_original + pos_increment;
                output.insert(start_time + duration, pos);
            }
        }
        // 通常节点
        else
        {
            output.insert(*iter_curr);
        }
    }

    // 最后一个节点的处理
    // 突变
    if (!iter_curr->second.isSame())
    {
        double start_time = iter_curr->first;
        double start_val = iter_curr->second.first();
        double end_val = iter_curr->second.second();
        int duration = 24;
        unique_ptr<ICurveFunctor> curve = make_unique<SqrType2>();
        if (iter_next == guide.end() || iter_next->second.second() == 0)
        {
            duration = 72;
            curve = make_unique<Sine>();
        }
        for (int i = 0; i < duration; i += step)
        {
            double x = 1.0 - static_cast<double>(i) / duration;
            double y = (*curve)(x);
            double pos_increment = y * (start_val - end_val);
            double pos_original = guide.LinearInterpolate(iter_curr->first + i);
            double pos = pos_original + pos_increment;
            output.insert(start_time + i, pos);
        }
        output.insert(start_time + duration, end_val);
    }
    // 通常节点
    else
    {
        output.insert(*iter_curr);
    }

    return output;
}

}