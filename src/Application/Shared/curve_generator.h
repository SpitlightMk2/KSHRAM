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

#include <vector>

#include "curves.h"

#include "src/Chart/indexed_chart.h"


namespace CurveGen
{

	template <typename T>
	struct CurveEntry
	{
		int time;
		T pos;
		bool linear;
	};

	template <typename T>
	using CurveData = std::vector<CurveEntry<T>>;

	using IntCurveData = CurveData<int>;
	using DoubleCurveData = CurveData<double>;

	/// 调整CurveData的时间偏移
	template <typename T>
	inline void Offset(CurveData<T>& data, int offset) {
		for (auto& item : data) {
			item.time += offset;
		}
	}

	/// 数据拼合
	template <typename T> 
	inline void MergeData(CurveData<T>& tgt, const CurveData<T>& src)
	{
		tgt.insert(tgt.end(), src.begin(), src.end());
	}


	/* #region 旋钮曲线 */

	/// 生成旋钮曲线：固定步长模式
	IntCurveData KnobCurve_Static(ICurveFunctor& func, const int start, const int end, const int length, bool reverse, const int step, bool laser2x);

	/// 生成旋钮曲线：固定步长模式，在直角之后（6*7 + n*8)
	IntCurveData KnobCurve_AfterSlam(ICurveFunctor& func, const int start, const int end, const int length, bool reverse, const int step, bool laser2x);
	
	/// 生成旋钮曲线：非固定步长模式
	IntCurveData KnobCurve_Dynamic(ICurveFunctor& func, const int start, const int end, const int length, bool reverse, bool laser2x);

	/// 生成旋钮曲线：全局选点模式
	IntCurveData KnobCurve_Global(ICurveFunctor& func, const int start, const int end, const int length, bool reverse, bool laser2x);

	/// 将旋钮数据转换为IndexList（之后就可以写进IndexedChart了）
	inline typename IndexedChart::KnobListType AssembleCurveData(const IntCurveData& data, int start_time) {
		IndexedChart::KnobListType output;
		for (auto& item : data) {
			output.insert(item.time + start_time, item.pos);
		}

		return output;
	}

	/* #endregion 旋钮曲线 */

	/* #region 镜头曲线 */

	/// 通常镜头曲线
	DoubleCurveData CameraCurve_Normal(ICurveFunctor& func, const double start, const double end, const int length, bool reverse, const int step, bool snake, bool roundInt);

	/// 增量镜头曲线：增量模式检查
	bool CameraCurve_Additive_TypeCheck(const std::string& type);

	/// 增量镜头曲线
	DoubleCurveData CameraCurve_Additive(const std::string& type, ICurveFunctor& func, const double offset, const int length, const int step, bool roundInt);

	/// 将镜头曲线数据转换为IndexList（之后就可以写进IndexedChart了）
	inline typename IndexedChart::MarkListType AssembleCameraData(const DoubleCurveData& data, int start_time) {
		IndexedChart::MarkListType output;
		for (auto& item : data) {
			output.insert(item.time + start_time, std::to_string(item.pos));
		}

		return output;
	}

	/// 从original上面采样，并将采样结果加到data上面。data应当是按时间升序排序的。
	void Addition(DoubleCurveData& data, int start_time, typename IndexedChart::MarkListType& original, double fallback = NAN);

	/// 将增量镜头曲线数据增加到现有镜头上，随后转换为IndexList。data应当是按时间升序排序的。
	inline typename IndexedChart::MarkListType AssembleIncrementCameraData(const DoubleCurveData& data, int start_time, typename IndexedChart::MarkListType& original) {
		IndexedChart::MarkListType output;
		DoubleCurveData data_copy = data;
		Addition(data_copy, start_time, original, 0.0);
		for (auto& item : data_copy) {
			output.insert(item.time + start_time, std::to_string(item.pos));
		}
		
		return output;
	}

}