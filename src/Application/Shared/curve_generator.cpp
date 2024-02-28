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

#include "curve_generator.h"

using namespace std;
using namespace CurveGen;


/* #region Assist Function */

/// Knob Curve: Dynamic Mode 用
vector<int> PossibleStep(int x) {
	if (x <= 6 || (x >= 10 && x <= 13) || x == 19 || x == 20) {
		return {};
	}
	else if (x == 7 || x == 14 || x == 21 || x == 28) {
		return { 7 };
	}
	else if (x == 8) {
		return { 8 };
	}
	else if (x == 9 or x == 18 or x == 27) {
		return { 9 };
	}
	else if (x == 15 or x == 22 or x == 29) {
		return { 7, 8 };
	}
	else if (x == 17 or x == 26) {
		return { 8, 9 };
	}
	else {
		return { 7, 8, 9 };
	}
}

/// Knob Curve: Global Mode 用
void KeyPointSelector(vector<tuple<int, int, double>>& posList, int length) {
	// 筛选结果，使相邻两个节点间隔不小于7
	int headMinimumTime = 7;
	int headMarkCount = 0;
	int tailMaximumTime = length - 7;
	int tailMarkCount = 0;

	while (true) {
		// 尾部退出条件
		if (headMinimumTime - 7 > tailMaximumTime) {
			while (tailMarkCount > 0 && headMinimumTime - 7 < tailMaximumTime + 7 &&
				headMinimumTime - 7 > tailMaximumTime) {
				size_t offset = posList.size() - tailMarkCount;
				posList.erase(posList.begin() + offset);
				tailMaximumTime = get<0>(posList[offset - 1]) - 7;
			}
			break;
		}
		// 头部游标前进
		while (get<0>(posList[headMarkCount]) < headMinimumTime) {
			posList.erase(posList.begin() + headMarkCount);
		}
		headMinimumTime = get<0>(posList[headMarkCount]) + 7;
		headMarkCount += 1;

		// 头部退出条件
		if (headMinimumTime - 7 > tailMaximumTime) {
			while (headMarkCount > 0 && headMinimumTime - 7 < tailMaximumTime + 7 &&
				headMinimumTime - 7 > tailMaximumTime) {
				posList.erase(posList.begin() + headMarkCount - 1);
				headMinimumTime = get<0>(posList[headMarkCount]) + 7;
			}
			break;
		}
		// 尾部游标前进
		while (get<0>(posList[posList.size() - 1 - tailMarkCount]) > tailMaximumTime) {
			size_t offset = posList.size() - 1 - tailMarkCount;
			posList.erase(posList.begin() + offset);
		}
		tailMaximumTime = get<0>(posList[posList.size() - 1 - tailMarkCount]) - 7;
		tailMarkCount += 1;
	}
}

inline double CalculateSlope(const CurveEntry<int>& start, const CurveEntry<int>& end, bool laser2x = false) {
	return abs(
		(ToKnobPos(end.pos, laser2x) - ToKnobPos(start.pos, laser2x)) / static_cast<double>(end.time - start.time)
	);
}

inline double CalculateSlope(const CurveEntry<double>& start, const CurveEntry<double>& end, bool laser2x = false) {
	return abs(
		(end.pos - start.pos) / static_cast<double>(end.time - start.time)
	);
}

/* #endregion Assist Function */


/* #region Smooth Filters */

void SmoothFilter(IntCurveData& curve_data, bool laser2x) {
	for (int i = 1; i < curve_data.size() - 1;) {
		
		CurveEntry<int>& last_data = curve_data[i - 1];
		CurveEntry<int>& curr_data = curve_data[i];
		CurveEntry<int>& next_data = curve_data[i + 1];

		// 由线性标记规则剔除
		if(last_data.linear && curr_data.linear && next_data.linear)
		{
			curve_data.erase(curve_data.begin() + i);
			continue;
		}

		// 由前后斜率规则剔除
		double slope_before = CalculateSlope(last_data, curr_data, laser2x);
		double slope_after = CalculateSlope(curr_data, next_data, laser2x);

		if (slope_before >= slope_after) {
			curve_data.erase(curve_data.begin() + i);
			continue;
		}

		++i;
	}
}

void SmoothFilter(DoubleCurveData& curve_data) {
	for (int i = 1; i < curve_data.size() - 1;) {

		CurveEntry<double>& last_data = curve_data[i - 1];
		CurveEntry<double>& curr_data = curve_data[i];
		CurveEntry<double>& next_data = curve_data[i + 1];

		// 由线性标记规则剔除
		if (last_data.linear && curr_data.linear && next_data.linear) {
			curve_data.erase(curve_data.begin() + i);
			continue;
		}
		
		// 由前后斜率规则剔除
		double slope_before = CalculateSlope(last_data, curr_data);
		double slope_after = CalculateSlope(curr_data, next_data);

		if (slope_before >= slope_after) {
			curve_data.erase(curve_data.begin() + i);
			continue;
		}

		++i;
	}
}

void LinearFilter(DoubleCurveData& curve_data) {
	for (int i = 1; i < curve_data.size() - 1;) {

		CurveEntry<double>& last_data = curve_data[i - 1];
		CurveEntry<double>& curr_data = curve_data[i];
		CurveEntry<double>& next_data = curve_data[i + 1];

		// 由线性标记规则剔除
		if (last_data.linear && curr_data.linear && next_data.linear) {
			curve_data.erase(curve_data.begin() + i);
		}
		else{
			++i;
		}
	}
}

/* #endregion Smooth Filters */


/* #region Knob Curves */

IntCurveData CurveGen::KnobCurve_Static(ICurveFunctor& func, const int start, const int end, const int length, bool reverse, const int step, bool laser2x) {
	IntCurveData output;
	output.push_back({ 0, start, true });
	// 线性
	if (func.name() == "linear") {
		// 起点+终点，结束
		output.push_back({ length, end, true });
		return output;
	}
	else {
		double width = end - start;
		int time = step;
		int length_minus_step = length - step;
		while (time <= length_minus_step) {
			double x = static_cast<double>(time) / length;

			double curvPos = func(x);
			curvPos = start + curvPos * width;
			bool linear = func.linearSection(x);

			output.push_back({ time, ToKnobIndex(curvPos, laser2x), linear });
			time += step;
		}

		output.push_back({ length, end, true });
	}

	SmoothFilter(output, laser2x);
	return output;
}

IntCurveData CurveGen::KnobCurve_AfterSlam(ICurveFunctor& func, const int start, const int end, const int length, bool reverse, const int step, bool laser2x) {
	IntCurveData output;
	output.push_back({ 0, start, true });
	// 线性
	if (func.name() == "linear") {
		// 起点+终点，结束
		output.push_back({ length, end, true });
		return output;
	}
	else {
		double width = end - start;
		int time = step;
		int length_minus_step = length - step;
		int afterslam_count = 6;
		while (time <= length_minus_step) {
			double x = static_cast<double>(time) / length;

			double curvPos = func(x);
			curvPos = start + curvPos * width;
			bool linear = func.linearSection(x);

			output.push_back({ time, ToKnobIndex(curvPos, laser2x), linear });

			if (reverse && time == length - 42) {
				time += 7;
			}
			else if (!reverse && afterslam_count > 0) {
				time += 7;
			}
			else {
				time += step;
			}
		}

		output.push_back({ length, end, true });
	}

	SmoothFilter(output, laser2x);
	return output;
}

IntCurveData CurveGen::KnobCurve_Dynamic(ICurveFunctor& func, const int start, const int end, const int length, bool reverse, bool laser2x) {
	IntCurveData output;
	output.push_back({ 0, start, true });
	// 线性
	if (func.name() == "linear") {
		// 起点+终点，结束
		output.push_back({ length, end, true });
		return output;
	}
	else {
		double width = end - start;
		int time = 0;
		int remain_time = length;
		while (remain_time > 0) {
			vector<int> steps = PossibleStep(remain_time);
			if (steps.size() == 0) {
				output.push_back({ length, end });
				break;
			}
			int step = 0;
			int curvPos = 0;
			double error = 1.0;
			bool linear = false;
			// 尝试可能的不同步长，选择舍入误差最小者
			for (int step_try : steps) {
				double x = static_cast<double>(time + step_try) / length;

				double pos = func(x);
				pos = start + pos * width;

				int pos_index = ToKnobIndex(pos, laser2x);
				double remain = abs(pos - pos_index);

				if (remain < error) {
					step = step_try;
					curvPos = pos_index;
					error = remain;
					linear = func.linearSection(x);
				}
			}

			time += step;
			output.push_back({ time, curvPos, linear });
			remain_time -= step;
		}

		output.back() = { length, end, true };
	}

	SmoothFilter(output, laser2x);
	return output;
}

IntCurveData CurveGen::KnobCurve_Global(ICurveFunctor& func, const int start, const int end, const int length, bool reverse, bool laser2x) {
	IntCurveData output;
	output.push_back({ 0, start, true });
	// 线性
	if (func.name() == "linear") {
		// 起点+终点，结束
		output.push_back({ length, end, true });
		return output;
	}
	else {
		double width = end - start;

		// 全时间刻采样（除了临近开头结尾的部分）
		using entry = tuple<int, int, double>;
		vector<entry> allPosList;
		for (int i = 7; i <= length - 7; ++i) {
			double x = static_cast<double>(i) / length;

			double pos = func(x);
			pos = start + pos * width;
			int pos_index = ToKnobIndex(pos, laser2x);
			double err = abs(pos - pos_index);

			allPosList.push_back(entry(i, pos_index, err));
		}

		// 按误差从小到大排序
		sort(allPosList.begin(), allPosList.end(),
			[](const entry& a, const entry& b) { return get<2>(a) < get<2>(b); });
		// 截取前1/4
		size_t cutoff = min(allPosList.size(), (allPosList.size() + 12) / 4 + 1);
		allPosList.erase(allPosList.begin() + cutoff, allPosList.end());
		// 按时间排序
		sort(allPosList.begin(), allPosList.end(),
			[](const entry& a, const entry& b) { return get<0>(a) < get<0>(b); });

		KeyPointSelector(allPosList, length);

		for (auto& item : allPosList) {
			int time = get<0>(item);
			int pos = get<1>(item);
			double x = static_cast<double>(time) / length;
			output.push_back({ time, pos, func.linearSection(x) });
		}
	}

	SmoothFilter(output, laser2x);
	return output;
}

/* #endregion Knob Curves */


/* #region Camera Curves */

DoubleCurveData CameraCurve_Static(ICurveFunctor& func, double start, double end, const int length, bool reverse, const int step, bool roundInt) {
	DoubleCurveData output;
	if (reverse) {
		std::swap(start, end);
	}

	output.push_back({ 0, start, true });
	// 线性
	if (func.name() == "linear") {
		// 起点+终点，结束
		output.push_back({ length, end, true });
		return output;
	}
	else {
		double width = end - start;
		int time = step;
		int length_minus_step = length - step;
		while (time <= length_minus_step) {
			double x = static_cast<double>(time) / length;

			double curvPos = func(x);
			curvPos = start + curvPos * width;
			bool linear = func.linearSection(x);
			if (roundInt) {
				curvPos = round(curvPos);
			}

			output.push_back({ time, curvPos, linear });
			time += step;
		}

		output.push_back({ length, end, true });
	}

	// 浮点就不用过filter了
	if (roundInt) {
		SmoothFilter(output);
	}
	else{
		LinearFilter(output);
	}

	if (reverse) {
		for (auto& item : output) {
			item.time = length - item.time;
		}
		// 有些地方需要时间升序，所以反一下
		std::reverse(output.begin(), output.end());
	}
	return output;
}

// 简单曲线和复合曲线的统一入口
DoubleCurveData CurveGen::CameraCurve_Normal(ICurveFunctor& func, const double start, const double end, const int length, bool reverse, const int step, bool snake, bool roundInt) {
	// 简单曲线
	if (!snake) {
		return CameraCurve_Static(func, start, end, length, reverse, step, roundInt);
	}
	// S型复合曲线
	else {
		DoubleCurveData output;

		// 前半段
		int mid_time = length / 2;
		double mid_pos = (start + end) / 2;
		auto first_half = CameraCurve_Static(func, start, mid_pos, mid_time, false, step, roundInt);
		output.insert(
			output.end(), first_half.begin(), first_half.end()
		);
		// 后半段
		if (length % 2 == 0) {
			output.pop_back();
		}
		auto second_half = CameraCurve_Static(func, mid_pos, end, mid_time, true, step, roundInt);
		Offset(second_half, length - mid_time);
		output.insert(output.end(), second_half.begin(), second_half.end());

		return output;
	}

}

bool CurveGen::CameraCurve_Additive_TypeCheck(const std::string& type)
{
	static std::vector<std::string> ValidTypes
	{
		"i", "impact", "ri", "rimpact",
		"c", "charge", "rc", "rcharge",
		"a", "arch",
		"sq", "square"
	};
	
	return find(ValidTypes.begin(), ValidTypes.end(), type) != ValidTypes.end();
}

// 增量曲线入口
// 要保证这里的输出是时间升序排列的
DoubleCurveData CurveGen::CameraCurve_Additive(const std::string& type, ICurveFunctor& func, const double offset, const int length, const int step, bool roundInt) {
	DoubleCurveData output;
	if (type == "i" || type == "impact") {
		output.push_back({ 0, 0 });
		DoubleCurveData ret = CameraCurve_Static(func, offset, 0, length - step, true, step, roundInt);
		Offset(ret, step);
		output.insert(output.end(), ret.begin(), ret.end());
	}
	else if (type == "ri" || type == "rimpact") {
		DoubleCurveData ret = CameraCurve_Static(func, 0, offset, length - step, false, step, roundInt);
		output.push_back({ length, 0 });
		output.insert(output.end(), ret.begin(), ret.end());
	}
	else if (type == "c" || type == "charge") {
		DoubleCurveData ret = CameraCurve_Static(func, 0, offset, length - step, true, step, roundInt);
		output.push_back({ length, 0 });
		output.insert(output.end(), ret.begin(), ret.end());
	}
	else if (type == "rc" || type == "rcharge") {
		output.push_back({ 0, 0 });
		DoubleCurveData ret = CameraCurve_Static(func, offset, 0, length - step, false, step, roundInt);
		Offset(ret, step);
		output.insert(output.end(), ret.begin(), ret.end());
	}
	else if (type == "a" || type == "arch") {
		// 前半段
		int mid_time = length / 2;
		DoubleCurveData first_half = CameraCurve_Static(func, 0, offset, mid_time, true, step, roundInt);
		output.insert(
			output.end(), first_half.begin(), first_half.end()
		);
		// 后半段
		if (length % 2 == 0) {
			output.pop_back();
		}
		DoubleCurveData second_half = CameraCurve_Static(func, offset, 0, mid_time, false, step, roundInt);
		Offset(second_half, length - mid_time);
		output.insert(output.end(), second_half.begin(), second_half.end());
	}

	else if (type == "sq" || type == "square")
	{
		output = {
			{0, 0}, {step, offset},
			{length, offset}, {length + step, 0}
		};
	}

	return output;
}

#pragma endregion


#pragma region Assemble

void CurveGen::Addition(DoubleCurveData& data, int start_time, typename IndexedChart::MarkListType& original, double fallback) {
	vector<int> intp_point;
	for (auto& item : data) {
		intp_point.push_back(item.time + start_time);
	}

	auto intp_result = original.LinearInterpolate(intp_point, fallback);
	for (int i = 0; i < data.size(); ++i) {
		data[i].pos += intp_result[i];
	}
}

/* #endregion Camera Curves */