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

#include "smooth_camera.h"

#include <vector>

#include "src/misc/enums.h"
#include "src/misc/utilities.h"
#include "../Shared/curve_generator.h"

using namespace std;
using namespace CurveGen;

namespace SmoothCamera_Core {

	enum class ProcessType {
		Normal,
		Additive
	};

	MarkType CmdMarkType(const std::string& str) {
		if (str == "zt" || str == "zoomtop" || str == "ztauto") {
			return MarkType::ZoomTop;
		}
		else if (str == "zb" || str == "zoombottom" || str == "zbauto") {
			return MarkType::ZoomBottom;
		}
		else if (str == "zs" || str == "zoomside" || str == "zsauto") {
			return MarkType::ZoomSide;
		}
		else if (str == "tilt" || str == "tiltauto") {
			return MarkType::Tilt;
		}
		else if (str == "split") {
			return MarkType::LaneSplit;
		}
		else {
			return MarkType::Error;
		}
	}

}


namespace core = SmoothCamera_Core;

vector<string> SmoothCamera::AcceptedCmdName(){
	return {
		"zt", "zb", "zs", "tilt", "split",
		"ztauto", "zbauto", "zsauto", "tiltauto",
		"zoomtop", "zoombottom", "zoomside"
	};
}

bool SmoothCamera::CheckArgs(const Command& cmd){
	// zt [curvetype] [reverse(bool)] [divisor(int)] (amp(double))
		if (cmd.argLength() >= 3 && cmd.argIsBool(1) && cmd.argIsInt(2)) {
			if (cmd.argLength() == 4) {
				return cmd.argIsDouble(3);
			}
			else {
				return true;
			}
		}
		else {
			return false;
		}
}

bool SmoothCamera::ProcessCmd(const Command& cmd, CommandMap& /*cmd_map*/, IndexedChart& chart) {

		// STEP 1 判断类型
		MarkType mark_type = core::CmdMarkType(cmd.cmd());

		// STEP 2 获取信息
		ErrorCollector& err_collector = GET_ERROR_COLLECTOR;
		int start_time = cmd.time();
		auto& mark_list = chart.MarkList(mark_type);

		auto iter = mark_list.nextItem(start_time);
		if (iter == mark_list.end()) {
			err_collector.ErrorLog(ErrorMessage<ErrorType::ObjectNotFound>());
			return false;
		}

		int end_time = iter->first;

		auto iter2 = mark_list.prevItem(start_time);
		if (iter2 == mark_list.end()) {
			err_collector.ErrorLog(ErrorMessage<ErrorType::ObjectNotFound>());
			return false;
		}

		if (!IsFloat(iter2->second.second()) || !IsFloat(iter->second.first())) {
			err_collector.ErrorLog(ErrorMessage<ErrorType::ParamIsNotNumber>());
			return false;
		}

		double start_pos = stod(iter2->second.second());
		double end_pos = stod(iter->second.first());

		if (start_pos == end_pos) {
			err_collector.ErrorLog("Camera value does not change here.");
			return false;
		}

		int length = end_time - start_time;
		const string& curve_type = cmd.arg(0);
		bool reverse = cmd.argAsBool(1);
		int step = 192 / cmd.argAsInt(2);
		bool roundInt = (mark_type != MarkType::Tilt);
		
		auto curve_functor = NewCurveFunctor(curve_type);
		bool snake = false;
		if (curve_type.size() > 0 && curve_type[0] == 'd') {
			curve_functor = NewCurveFunctor(curve_type.substr(1));
			snake = true;
		}
		if (curve_functor == nullptr) {
			err_collector.ErrorLog(ErrorMessage<ErrorType::InvalidArgs>());
			return false;
		}

		if (cmd.argLength() == 4) {
			curve_functor->amp = cmd.argAsDouble(3);
		}

		// STEP 3 生成
		DoubleCurveData data = CameraCurve_Normal(*curve_functor, start_pos, end_pos, length, reverse, step, snake, roundInt);

		// STEP 4 组装
		auto curve_map = AssembleCameraData(data, start_time);

		chart.ReplaceMark(mark_type, curve_map);

		return true;
	}