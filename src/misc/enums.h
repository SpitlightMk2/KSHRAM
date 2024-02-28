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

#include <string>


/// ksh旋钮位置记录字符
extern char g_KshKnobList[51];


/// notes类型枚举：BT
enum class BT 
{
	A, B, C, D
};


/// notes类型枚举：FX
enum class FX
{
	L, R
};

/// notes类型枚举：旋钮
enum class Knob 
{
	L, R
};

/// 旋钮/FX枚举的二合一（使用时通过别的方式得知用的是哪一类）
enum class Side {
	L, R
};

enum class Comment
{
	Command,
	Others
};

enum class Others
{
	Others
};

/// FX的左右转为Side的左右
inline constexpr Side ToSide(FX fx) {
	return (fx == FX::L) ? Side::L : Side::R;
}

/// 旋钮的左右转为Side的左右
inline constexpr Side ToSide(Knob k) {
	return (k == Knob::L) ? Side::L : Side::R;
}

/// 键盘note状态
enum class KeyState {
	None, Chip, Long
};

/// 旋钮状态
enum class KnobState {
	None, Line, Slam, End
};

/// 谱面标记类型
enum class MarkType {
	BPM, TimeSignature, FXLong, FXChip, 
	Laser2x, Filter, KnobVolume, SlamSound,
	SlamVolume, Tilt, ZoomTop, ZoomBottom,
	ZoomSide, Stop, LaneSplit, Error
};

// 注意：这里的Count比上面总量多，因为FXLong, FXChip, Laser2x还需要区分左右侧。另外Error不计入。
constexpr int MarkTypesCount = 18;

/// 轨道回转效果类型
enum class Spin {
	Circle, Side, Shake, None
};

/* #region 枚举变量的转换 */

/// Mark类型在ksh中的关键字
const std::string MarkStr(MarkType t, Side side = Side::L);

/// 轨道回转效果在ksh中的关键字
const std::string SpinStr(Spin spin, Side side);

/// BT的数字序号
int BTIndex(BT bt);
/// 从序号反推BT枚举类型
BT BTByIndex(int i);

/// FX的数字序号
int FXIndex(FX fx);
/// 从序号反推FX枚举类型
FX FXByIndex(int i);

/// 旋钮的数字序号
int KnobIndex(Knob knob);
/// 从序号反推旋钮枚举类型
Knob KnobByIndex(int i);

/// Mark类型的数字序号
int MarkIndex(MarkType t, Side side = Side::L);
/// 从序号反推Mark类型
MarkType MarkByIndex(int i);
/// 从序号反推Mark的Side
Side MarkSideByIndex(int i);

/* #endregion */
