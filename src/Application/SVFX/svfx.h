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

#include "../application_bus.h"

class SVFX : public IApplication {
public:
	SVFX() = default;
	~SVFX() = default;

	APP_TYPE(Normal);

	std::vector<std::string> AcceptedCmdName() override;

    bool CheckArgs(const Command& cmd) override;

    bool ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart) override;
};