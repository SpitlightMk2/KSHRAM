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

#include "src/IndexList/index_list.h"

namespace TiltStyler_Core
{

using TiltStylerFunc = IndexList<double>(IndexList<double>&, int);

/// 直接拿TiltGuide当输出，完全不做平滑
IndexList<double> TiltStyle_Null(IndexList<double>& guide, int step = 6);

/// 每个突变位置都放半拍曲线的平滑
IndexList<double> TiltStyle_Uniform(IndexList<double>& guide, int step = 6);

/// 仿照KSM样式的倾斜平滑
IndexList<double> TiltStyle_KSM(IndexList<double>& guide, int step = 6);

}