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

#include "camera_amp.h"

#include <string>

using namespace std;

namespace CameraAmp_Core {

	inline double Amplify(double origin, double amp, double center) {
		return center + amp * (origin - center);
	}

	inline CommandMap::Iterator FindEndCommand(const Command& cmd, CommandMap& cmd_map) {
		int start_time = cmd.time();
		string end_cmd = "end " + cmd.cmd();
		auto iter = cmd_map.FindNearestCommand(end_cmd, start_time);

		return iter;
	}

	inline void RemoveEndCommand(CommandMap& cmd_map, CommandMap::Iterator iter) {
		cmd_map.erase(iter);
	}

	void AmplifySection(IndexedChart::MarkListType& mark_list, int start_time, int end_time, double amp, double center, bool roundInt) {
		auto iter = mark_list.nextItem(start_time - 1);
		auto iter_end = mark_list.nextItem(end_time - 1);

		while (iter != iter_end) {
			bool sudden_change = !iter->second.isSame();
			if (IsFloat(iter->second.first())) {
				double amp_first = Amplify(stod(iter->second.first()), amp, center);
				if (roundInt) {
					iter->second.setFirst(
						to_string(static_cast<int>(round(amp_first)))
					);
				}
				else {
					iter->second.setFirst(to_string(amp_first));
				}
			}
			
			if (sudden_change && IsFloat(iter->second.second())) {
				double amp_second = Amplify(stod(iter->second.second()), amp, center);
				if (roundInt) {
					iter->second.setSecond(
						to_string(static_cast<int>(round(amp_second)))
					);
				}
				else {
					iter->second.setSecond(to_string(amp_second));
				}
			}

			++iter;
		}
	}
}

namespace core = CameraAmp_Core;

vector<string> CameraAmp::AcceptedCmdName()
{
	return { "ztamp", "zbamp", "zsamp", "tiltamp", "splitamp" };
}

bool CameraAmp::CheckArgs(const Command& cmd) {
	// ztamp [amp] ([center-val])
	if (cmd.argLength() >= 1 && cmd.argIsDouble(0)) {
		if (cmd.argLength() >= 2) {
			return cmd.argIsDouble(1);
		}
		else {
			return true;
		}
	}
	else {
		return false;
	}
}

bool CameraAmp::ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart) {
	// STEP 1 判断命令类型
	MarkType type;
	if (cmd.cmd() == "ztamp") {
		type = MarkType::ZoomTop;
	}
	else if (cmd.cmd() == "zbamp") {
		type = MarkType::ZoomBottom;
	}
	else if (cmd.cmd() == "zsamp") {
		type = MarkType::ZoomSide;
	}
	else if (cmd.cmd() == "tiltamp") {
		type = MarkType::Tilt;
	}
	else if (cmd.cmd() == "splitamp") {
		type = MarkType::LaneSplit;
	}
	else {
		return false;
	}

	// STEP 2 获取信息
	ErrorCollector& err_collector = GET_ERROR_COLLECTOR;
	double amp = cmd.argAsDouble(0);
	double center = 0;

	auto& mark_list = chart.MarkList(type);

	if (cmd.argLength() >= 2) {
		center = cmd.argAsDouble(1);
	}
	else {
		
		auto item_iter = mark_list.prevItem(cmd.time());
		if (item_iter == mark_list.end()) {
			item_iter = mark_list.nextItem(cmd.time());
			if (item_iter == mark_list.end()) {
				err_collector.ErrorLog(ErrorMessage<ErrorType::ObjectNotFound>());
				return false;
			}
		}

		auto& center_str = item_iter->second.second();
		if (!IsFloat(center_str)) {
			err_collector.ErrorLog(ErrorMessage<ErrorType::ParamIsNotNumber>());
			return false;
		}
		center = stod(center_str);
	}

	int start_time = cmd.time();
	auto end_cmd_iter = core::FindEndCommand(cmd, cmd_map);
	
	if (end_cmd_iter == cmd_map.end()) {
		string end_cmd = "end " + cmd.cmd();
		err_collector.ErrorLog(ErrorMessage<ErrorType::EndCommandNotFound>() + "\n End command syntax: " + end_cmd);
		return false;
	}

	int end_time = end_cmd_iter->first;

	bool roundInt = (type != MarkType::Tilt);

	// STEP 3 执行
	core::AmplifySection(mark_list, start_time, end_time, amp, center, roundInt);

	// STEP 4 删除结束命令
	core::RemoveEndCommand(cmd_map, end_cmd_iter);

	return true;
}