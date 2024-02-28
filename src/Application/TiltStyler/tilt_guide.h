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

#include "tilt_position.h"
#include "src/IndexList/index_list.h"

namespace TiltStyler_Core {

/// 移除TiltGuide当中没用的记录
void RegulateTiltGuide(IndexList<double> &guide);

/// 加法合并两个旋钮的TiltGuide
IndexList<double> MergeTiltGuide(IndexList<double> &a, IndexList<double> &b);


IndexList<double> MakeTiltGuide(IndexList<double> &knob, int start, int end,
                                ITiltPos &tilt_func);

} // namespace TiltStyler_Core