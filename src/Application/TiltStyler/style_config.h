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

#include <functional>
#include "src/Chart/indexed_chart.h"
#include "tilt_position.h"
#include "tilt_style.h"

namespace TiltStyler_Core
{

class StyleConfig
{
public:
    std::unique_ptr<ITiltPos> pleft = std::make_unique<MaxTiltL>();
    std::unique_ptr<ITiltPos> pright = std::make_unique<MaxTiltR>();
    std::function<TiltStylerFunc> styler = nullptr;
    bool laser2x_extend = false;
    bool zero_left = false, zero_right = false;

public:
    void ConfigFromStr(const std::vector<std::string>& conf, bool with_style = false);
};

IndexList<double> MakeTiltGuide(IndexedChart& chart, int start, int end,
        std::map<int, StyleConfig>& func_map);

}  // namespace TiltStyler_Core