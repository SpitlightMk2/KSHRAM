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

#include "src/misc/enums.h"

#include <string>
#include <vector>

/* #region Mark */

/// 对应于ksh中"a=b"这样的额外记录
struct Mark
{
public:
	MarkType type;
	Side side;
	std::string value;
	// 额外数据记录段
	std::string param;

public:
	/// 空构造
	inline Mark();
	/// 拷贝构造
	inline Mark(const Mark&) = default;
	/// 从Mark字符串构造
	Mark(const std::string& str);
	/// 从参数构造
	inline Mark(MarkType, Side, std::string value, std::string param = "");
	/// 转换为Mark字符串
	std::string ToString() const;

	/// 比较是否是同类型的mark
	bool IsSameType(MarkType type, Side side = Side::L) const;
};


/* #endregion */

/* #region SpinEffect */

/// 谱面回转特效。ksh中对应的字符紧跟在谱面内容之后。
struct SpinEffect {
public:
	Spin spin;
	Side side;
	int length;
	int params[3];

public:
	inline SpinEffect();
	inline SpinEffect(const SpinEffect&) = default;
	SpinEffect(const std::string& str);

	/// 转换为Spin字符串
	std::string ToString() const;

	friend std::ostream& operator <<(std::ostream& os, SpinEffect se);
};

/* #endregion */


/* #region Entry */

/// 单条ksh记录，包括当前时刻的note记录，所有mark，回转，注释等信息。
class Entry
{
protected:
	std::vector<Mark> marks;
	std::string comments;
	std::string other_parts;
	std::string notes;
	SpinEffect spin_effect;

	bool err_flag;

public:
	/// 空构造：将marks和comments留空，将notes设置为空白谱面段
	Entry();

	/// @brief 从ksh片段构造
	/// @param ksh ksh片段。可包含多条marks，一条comments和一行谱面行。
	Entry(const std::string& ksh);

	/// 拷贝构造
	Entry(const Entry&) = default;
	Entry& operator=(const Entry&) = default;

	/// 移动构造
	Entry(Entry&&) = default;
	Entry& operator=(Entry&&) = default;

public:
	/// @brief 从ksh片段导入数据
	/// @param ksh ksh片段。可包含多条marks，一条comments和一行谱面行。
	void ImportFromKsh(const std::string& ksh);

	/// 导出为ksh片段。不带有末尾换行符。
	std::string ExportToKsh() const;

	/// 从流输入
	friend std::istream& operator>> (std::istream& is, Entry& entry);

	/// 输出到流
	friend std::ostream& operator<< (std::ostream& os, const Entry& entry);

	/// 当前Entry有错误。在导入数据之后用于检查。
	inline bool Error();

	/// 获得指定BT的状态
	KeyState GetBTState(BT bt) const;

	/// 写入指定BT的状态
	void SetBTState(BT bt, KeyState state);
	
	/// 获得指定FX的状态
	KeyState GetFXState(FX fx) const;

	/// 写入指定FX的状态
	void SetFXState(FX fx, KeyState state);

	/// 获取指定旋钮的位置下标
	int GetKnobIndex(Knob knob) const;

	/// 获取指定旋钮的位置
	double GetKnobPos(Knob knob) const;

	/// 设置指定旋钮的位置
	void SetKnobPos(Knob knob, int index);

	/// 查找指定Mark
	bool HasMark(MarkType mark, Side side = Side::L) const;

	/// 查找指定Mark是否有多个
	bool HasMultipleMarks(MarkType mark, Side side = Side::L) const;

	/// 查找和获取指定Mark, 总是获取第一个匹配。
	std::vector<Mark>::iterator FindFirstMark(MarkType mark, Side side = Side::L);

	/// 查找和获取指定Mark, 总是获取第一个匹配。
	std::vector<Mark>::const_iterator FindFirstMark(MarkType mark, Side side = Side::L) const;

	/// 查找和获取指定Mark, 总是获取最后一个匹配。
	std::vector<Mark>::reverse_iterator FindLastMark(MarkType mark, Side side = Side::L);

	/// 查找和获取指定Mark, 总是获取最后一个匹配。
	std::vector<Mark>::const_reverse_iterator FindLastMark(MarkType mark, Side side = Side::L) const;

	/// 获取指定的Mark的值，总是获取第一个匹配。
	std::string GetFirstMarkVal(MarkType mark, Side side = Side::L) const;

	/// 获取指定的Mark的值，总是获取最后一个匹配。
	std::string GetLastMarkVal(MarkType mark, Side side = Side::L) const;

	/// 设置指定Mark的值，总是获取第一个匹配。
	void SetFirstMarkVal(const std::string& val, MarkType mark, Side side = Side::L);

	/// 设置指定Mark的值，总是获取最后一个匹配。
	void SetLastMarkVal(const std::string& val, MarkType mark, Side side = Side::L);

	/// 添加一个Mark
	inline void AddMark(const Mark& mark);

	/// 删除所有指定类型的Mark
	inline void DeleteMarks(MarkType mark, Side side = Side::L);

	/// 获得回转特效对象
	inline SpinEffect& GetSpinEffect();

	/// 获得回转特效对象
	inline const SpinEffect& GetSpinEffect() const;

	/// 访问Comment字符串
	inline std::string& Comments();

	/// 访问Comment字符串
	inline const std::string& Comments() const;

	/// 访问other_items字符串
	inline std::string& OtherItems();

	/// 访问other_items字符串
	inline const std::string& OtherItems() const;

	/// 产生一个后继Entry。用于扩充一个小节中的Entry量。
	Entry InsertionEntry() const;

};

/* #endregion */

/* INLINE FUNCTION */

#include "entry_inline.h"