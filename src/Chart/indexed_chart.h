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
#include "chart.h"

/// @brief
/// 使用有序表存储每种谱面要素的谱面。暂不包含头（谱面信息）和自定义fx的部分。
///
/// 易于进行查询和修改。
///
/// @note
/// 因为懒，大部分方法没有做常量的版本。
class IndexedChart {
public:
	using BTListType = IndexList<int>;
	using FXListType = IndexList<int>;
	using KnobListType = IndexList<int>;
	using MarkListType = IndexList<std::string>;
	using SpinEffectListType = IndexList<SpinEffect>;
	using CommentListType = IndexList<std::string>;
private:
	int total_time = 0;

	// BT
	IndexList<int> bt_lists[4];
	// FX
	IndexList<int> fx_lists[2];
	// 旋钮
	IndexList<int> knob_lists[2];
	// Marks
	IndexList<std::string> mark_lists[MarkTypesCount];
	// SpinEffects
	IndexList<SpinEffect> spin_effect_list;
	// Comments
	IndexList<std::string> comment_list;
	// Other Items
	IndexList<std::string> other_items_list;


public:
	IndexedChart() = default;
	IndexedChart(const IndexedChart&) = default;
	/// 构造并从chart导入数据
	inline IndexedChart(Chart& chart);

	/// 从chart导入数据
	void ImportFromChart(Chart& chart);
	/// 将自身数据导出为chart（可进一步转换为.ksh）
	Chart ExportToChart();

	/// [调试中] 从IndexedChart格式的文本读入
	bool ImportFromString(const std::string& content);
	/// [调试中] 导出为IndexedChart文本字符串
	std::string ExportToString();

	/// 将自身数据打印为 时间:值 的形式
	friend std::ostream& operator <<(std::ostream& os, IndexedChart ic);

private:
	/// 计算当前谱面总时长
	int CalculateTotalTime() const;
	/// 插入指定数据
	void InsertData(int type, int map_id, int time, const std::string& val);

public:
	/// 使用所给时间更新总时间(仅当所给时间大于总时间时起效)
	inline void UpdateTotalTime(int time);
	// 获取各索引表
	/// 获取BT索引表
	inline IndexList<int>& BTList(BT bt);
	/// 获取FX索引表
	inline IndexList<int>& FXList(FX fx);
	/// 获取旋钮索引表
	inline IndexList<int>& KnobList(Knob knob);
	/// 获取各类标记的索引表
	inline IndexList<std::string>& MarkList(MarkType mark, Side side);
	/// 获取回转特特效索引表
	inline IndexList<SpinEffect>& SpinEffectList();
	/// 获取注释的索引表
	inline IndexList<std::string>& CommentList();
	/// 获取其他内容的索引表
	inline IndexList<std::string>& OtherItemsList();

	// 衍生内容计算
	/// 获取旋钮位置表。计算时会考虑旋钮外扩，最终范围在-25 ~ 75之间。
	IndexList<double> KnobPosList(Knob knob) const;

	// 修改各类子表的部分
	/// 替换一段BT表的内容
	void ReplaceBT(BT bt, const IndexList<int>& lst, int offset = 0);
	/// 替换一段FX表的内容
	void ReplaceFX(FX fx, const IndexList<int>& lst, int offset = 0);
	/// 替换一段旋钮表的内容
	void ReplaceKnob(Knob knob, const IndexList<int>& lst, int offset = 0);
	/// 替换一段标记表的内容（标记没有左右区分）
	void ReplaceMark(MarkType mark, const IndexList<std::string>& lst, int offset = 0);
	/// 替换一段标记表的内容（标记有左右区分）
	void ReplaceMark(MarkType mark, Side side, const IndexList<std::string>& lst, int offset = 0);

	/// 全体偏移
	void Offset(int offset_val);

	/// @brief [调试中] 产生切片。切片从包含起始时刻内容，但不包含结束时刻内容。
	/// @param start_time 起始时间
	/// @param length 切片长度
	IndexedChart Slice(int start_time, int length) const;

	/// [调试中] 使用输入的chart的内容覆盖指定时间段的内容
	void ReplaceMerge(IndexedChart& chart, int offset = 0);

};

/// [调试中] 导出为IndexedChart文本
std::ostream& operator <<(std::ostream& os, IndexedChart ic);

/* INLINE FUNCTION */

#include "indexed_chart_inline.h"