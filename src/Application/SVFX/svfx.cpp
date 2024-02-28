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

#include "svfx.h"

#include <cmath>

using namespace std;

namespace SVFX_Core {

	using BPMData = vector<pair<int, double>>;

/* #region Curves */

	struct ISVFXFunctor {
		double amp = 1;
		int length = 0;

		virtual const std::string_view name() = 0;
		virtual double operator()(double x1, double x2) = 0;
	};

	struct SVFXLinear : public ISVFXFunctor {
		inline virtual const std::string_view name() override { return "linear"; }
		inline virtual double operator()(double x1, double x2) override {
			double normal_amp = atan(amp) / PI * 2; // 0 - inf ==> 0 - 1
			double k = log((1 + normal_amp) / (1 - normal_amp));
			double stdavr = log(((1 - normal_amp) * length + 2 * normal_amp * x2) /
				((1 - normal_amp) * length + 2 * normal_amp * x1));

			return length / k * stdavr;
		}
	};

	struct SVFXQuad : public ISVFXFunctor {
		inline virtual const std::string_view name() override { return "p"; }
		inline virtual double operator()(double x1, double x2) override {
			double k = atan(1 / amp);
			double ampN = ampN = amp * length;
			double stdavr = atan(x2 / ampN) - atan(x1 / ampN);

			return length / k * stdavr;
		}
	};

	struct SVFXSqrt : public ISVFXFunctor {
		inline virtual const std::string_view name() override { return "sq"; }
		inline virtual double operator()(double x1, double x2) override {
			double k = (1 + amp * log(amp / (amp + 1)));
			double sqrtx1 = sqrt(x1 / length);
			double sqrtx2 = sqrt(x2 / length);
			double stdavr = sqrtx2 - sqrtx1 + amp * log((amp + sqrtx1) / (amp + sqrtx2));

			return length / k * stdavr;
		}
	};

	struct SVFXSine : public ISVFXFunctor {
		inline virtual const std::string_view name() override { return "s"; }
		inline virtual double operator()(double x1, double x2) override {
			double normal_amp = amp + 1; // 0 - inf ==> 1 - inf
			double b = sqrt(normal_amp * normal_amp - 1);
			double k = (2 * atan((normal_amp + 1) / b) / b) - ((2 * atan(1 / b)) / b);
			double norm = PI / 4 / length;
			double normx1 = x1 * norm;
			double normx2 = x2 * norm;
			double stdavr = ((2 * atan((normal_amp * tan(normx2) + 1) / b) / b) -
				(2 * atan((normal_amp * tan(normx1) + 1) / b) / b));

			return length / k * stdavr;
		}
	};

	struct SVFXExp : public ISVFXFunctor {
		inline virtual const std::string_view name() override { return "e"; }
		inline virtual double operator()(double x1, double x2) override {
			double k = (1 - exp(-amp)) / amp;
			double norm = -amp / length;
			double normx1 = x1 * norm;
			double normx2 = x2 * norm;
			double stdavr = (exp(normx1) - exp(normx2)) / amp;

			return length / k * stdavr;
		}
	};

/* #endregion */

	unique_ptr<ISVFXFunctor> NewSVFXFunctor(const string& type) {
		if (type == "l" || type == "linear") {
			return unique_ptr<ISVFXFunctor>(new SVFXLinear());
		}
		else if (type == "p" || type == "parabola") {
			return unique_ptr<ISVFXFunctor>(new SVFXQuad());
		}
		else if (type == "sq" || type == "sqrt") {
			return unique_ptr<ISVFXFunctor>(new SVFXSqrt());
		}
		else if (type == "s" || type == "sine") {
			return unique_ptr<ISVFXFunctor>(new SVFXSine());
		}
		else if (type == "e" || type == "exp") {
			return unique_ptr<ISVFXFunctor>(new SVFXExp());
		}
		else {
			return nullptr;
		}
	}

/* #region assist func */

	

/* #endregion */

/* #region curve gen */

	BPMData GenerateData(ISVFXFunctor& func, double bpm, int step) {
		BPMData output;
		int length = func.length;
		int count = length / step;
		for (int i = 0; i < count; i += 1) {
			double x1 = static_cast<double>(length * i) / count;
			double x2 = static_cast<double>(length * (i + 1)) / count;

			double avr_bpm = step * bpm / func(x1, x2);
			output.push_back({ step * i, avr_bpm });
		}

		return output;
	}

/* #endregion */

	inline typename IndexedChart::MarkListType AssembleData(const BPMData& data, int start_time) {
		IndexedChart::MarkListType output;
		for (auto& item : data) {
			output.insert(item.first + start_time, to_string(item.second));
		}

		return output;
	}

}

namespace core = SVFX_Core;

vector<string> SVFX::AcceptedCmdName()
{
	return { "svfx" };
}

bool SVFX::CheckArgs(const Command& cmd) {
		// svfx [type] [reverse(bool)] [length(float, ratio)] [div(int)] (BPM(float)) (amp(float))
		if (cmd.argLength() >= 4 && cmd.argIsBool(1)  &&
			(cmd.argIsDouble(2) || cmd.argIsRatio(2)) && cmd.argIsInt(3)) {
			if (cmd.argLength() >= 6) {
				return cmd.argIsDouble(4) && cmd.argIsDouble(5);
			}
			else if (cmd.argLength() == 5) {
				return cmd.argIsDouble(4);
			}
			else {
				return true;
			}
		}
		else {
			return false;
		}
	}

bool SVFX::ProcessCmd(const Command& cmd, CommandMap& /*cmd_map*/, IndexedChart& chart) {
	
	// STEP 1 获取信息
	ErrorCollector& err_collector = GET_ERROR_COLLECTOR;
	unique_ptr<core::ISVFXFunctor> pfunc = core::NewSVFXFunctor(cmd.arg(0));
	bool reverse = cmd.argAsBool(1);
	int length = cmd.argAsDoubleOrRatio(2, 48.0);
	pfunc->length = length;
	int step = 192 / cmd.argAsInt(3);

	if (pfunc->length % step != 0) {
		err_collector.ErrorLog("Invalid divisor.");
		return false;
	}

	if (cmd.argLength() >= 5){
		pfunc->amp = cmd.argAsDouble(4);
	}

	double bpm;
	if (cmd.argLength() >= 6) {
		bpm = cmd.argAsDouble(5);
	}
	else {
		// 寻找当前bpm
		auto bpm_list = chart.MarkList(MarkType::BPM);
		bpm = stod(bpm_list.prevItem(cmd.time())->second.first());
	}

	// STEP 2 生成数据
	core::BPMData bpm_data = core::GenerateData(*pfunc, bpm, step);

	if (reverse) {
		for (auto& item : bpm_data) {
			item.first = length - step - item.first;
		}
	}

	bpm_data.push_back({ length, bpm });

	// STEP 3 写入

	auto bpm_map = core::AssembleData(bpm_data, cmd.time());

	chart.ReplaceMark(MarkType::BPM, bpm_map);

	return true;
}