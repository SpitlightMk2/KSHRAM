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

#include "src/Measure/measure.h"
#include "src/Header/header.h"
#include "src/IndexList/index_list.h"

// 暂时先这么弄，如果要做自定义fx的编辑再细化。
using CustomFX = std::string;

/// 谱面，包括头（谱面信息），谱面本体，自定义fx三个部分。
class Chart {
public:
	friend class IndexedChart;

private:
	// 谱面头
	Header header;
	// 谱面主体
	std::vector<Measure> measures;
	// 自定义FX
	CustomFX custom_fx;

	// 小节起始时间反查表
	IndexList<int> measure_at_time;

public:
	Chart() = default;

	/// 从文件导入
	inline bool ImportFromFile(const std::string& path);

	/// 导出到文件
	inline void ExportToFile(const std::string& path);

	/// 从字符串导入
	bool ImportFromString(const std::string& path);

	/// 导出到字符串
	std::string ExportToString();

	/// 获取头部分
	inline Header& GetHeader();

	/// 获取自定义fx
	inline CustomFX& GetCustomFX();

	/// 获取第n小节
	inline Measure& operator[](int n);

	/// 根据小节和记录两个id获取记录
	inline Entry& operator()(int measure_id, int entry_id);

	/// 获取指定时间所对应的小节ID（小节包括起始时间，不包括结束时间）
	inline int MeasureIDAtTime(int time);

	/// 获取指定时间所在的小节（小节包括起始时间，不包括结束时间）
	inline Measure& MeasureAtTime(int time);

	/// 获取指定时间所对应的小节内记录id
	inline int EntryIDAtTime(int time);

	/// 获取指定时间所对应的小节记录
	inline Entry& EntryAtTime(int time);

	/// 获取指定时间所对应的小节id和小节内记录id
	inline std::tuple<int, int> MeasureAndEntryIDAtTime(int time);

	/// 获取指定id处记录所对应的时间
	inline int TimeAtID(int measure_id, int entry_id);

	/// 谱面总长
	inline int TotalTime();


public:

	/// 获取BT的索引表
	IndexList<int> BTIndexList(BT bt);

	/// 获取FX的索引表
	IndexList<int> FXIndexList(FX fx);

	/// 获取旋钮的索引表
	IndexList<int> KnobIndexList(Knob side);

	/// 获取指定Mark的索引表
	IndexList<std::string> MarkIndexList(MarkType mark, Side side = Side::L);

	/// 获取回转特效的索引表
	IndexList<SpinEffect> SpinEffectList();

	/// 获取注释的索引表
	IndexList<std::string> CommentIndexList();

	/// 获取未知内容的索引表
	IndexList<std::string> OtherItemIndexList();

	/// 在末尾加入小节
	inline void AppendMeasure(Measure& measure);

public:
	/// 逐个Entry访问的迭代器
	class EntryIterator {
	private:
		Chart* p_chart;
		int measure_id, entry_id;

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = Entry;
		using difference_type = int;
		using pointer = Entry*;
		using reference = Entry&;

		friend class Chart;

	private:
		EntryIterator(Chart& chart, int measure_id = 0, int entry_id = 0);

	public:
		EntryIterator(const EntryIterator&) = default;

	public:
		EntryIterator& operator++();
		inline EntryIterator operator++(int);
		EntryIterator& operator--();
		inline EntryIterator operator--(int);

		inline bool operator==(const EntryIterator& other) const;
		inline bool operator!=(const EntryIterator& other) const;

		value_type& operator*();
		
		inline int Time() const;

	};

	inline EntryIterator begin();

	inline EntryIterator end();
};

#include "chart_inline.h"