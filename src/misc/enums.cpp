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

#include "enums.h"

char g_KshKnobList[51] =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o' };

const std::string MarkStr(MarkType t, Side side) {
	switch (t)
	{
	case MarkType::BPM:
		return "t";
	case MarkType::TimeSignature:
		return "beat";
	case MarkType::FXLong:
		if (side == Side::L) {
			return "fx-l";
		}
		else /* if (side == Side::R) */ {
			return "fx-r";
		}
	case MarkType::FXChip:
		if (side == Side::L) {
			return "fx-l_se";
		}
		else /* if (side == Side::R) */ {
			return "fx-r_se";
		}
	case MarkType::Laser2x:
		if (side == Side::L) {
			return "laserrange_l";
		}
		else /* if (side == Side::R) */ {
			return "laserrange_r";
		}
	case MarkType::Filter:
		return "filtertype";
	case MarkType::KnobVolume:
		return "pfiltergain";
	case MarkType::SlamSound:
		return "chokkakuse";
	case MarkType::SlamVolume:
		return "chokkakuvol";
	case MarkType::Tilt:
		return "tilt";
	case MarkType::ZoomTop:
		return "zoom_top";
	case MarkType::ZoomBottom:
		return "zoom_bottom";
	case MarkType::ZoomSide:
		return "zoom_side";
	case MarkType::Stop:
		return "stop";
	case MarkType::LaneSplit:
		return "center_split";
	default:
		return "//";
	}
}

const std::string SpinStr(Spin spin, Side side) {
	switch (spin)
	{
	case Spin::Circle:
		if (side == Side::L) {
			return "@(";
		}
		else {
			return "@)";
		}
	case Spin::Side:
		if (side == Side::L) {
			return "@<";
		}
		else {
			return "@>";
		}
	case Spin::Shake:
		if (side == Side::L) {
			return "S<";
		}
		else {
			return "S>";
		}
	default:
		return "";
	}
}

int BTIndex(BT bt) {
	switch (bt)
	{
	case BT::A:
		return 0;
	case BT::B:
		return 1;
	case BT::C:
		return 2;
	case BT::D:
		return 3;
	default:
		return -1;
	}
}

BT BTByIndex(int i) {
	switch (i)
	{
	case 0:
		return BT::A;
	case 1:
		return BT::B;
	case 2:
		return BT::C;
	case 3:
		return BT::D;
	default:
		return BT::A;
	}
}

int FXIndex(FX fx) {
	switch (fx)
	{
	case FX::L:
		return 0;
	case FX::R:
		return 1;
	default:
		return -1;
	}
}

FX FXByIndex(int i) {
	switch (i)
	{
	case 0:
		return FX::L;
	case 1:
		return FX::R;
	default:
		return FX::L;
	}
}

int KnobIndex(Knob knob) {
	switch (knob)
	{
	case Knob::L:
		return 0;
	case Knob::R:
		return 1;
	default:
		return -1;
	}
}

Knob KnobByIndex(int i) {
	switch (i)
	{
	case 0:
		return Knob::L;
	case 1:
		return Knob::R;
	default:
		return Knob::L;
	}
}

int MarkIndex(MarkType t, Side side) {
	switch (t)
	{
	case MarkType::BPM:
		return 0;
	case MarkType::TimeSignature:
		return 1;
	case MarkType::FXLong:
		if (side == Side::L) {
			return 2;
		}
		else if (side == Side::R) {
			return 3;
		}
		break;
	case MarkType::FXChip:
		if (side == Side::L) {
			return 4;
		}
		else if (side == Side::R) {
			return 5;
		}
		break;
	case MarkType::Laser2x:
		if (side == Side::L) {
			return 6;
		}
		else if (side == Side::R) {
			return 7;
		}
		break;
	case MarkType::Filter:
		return 8;
	case MarkType::KnobVolume:
		return 9;
	case MarkType::SlamSound:
		return 10;
	case MarkType::SlamVolume:
		return 11;
	case MarkType::Tilt:
		return 12;
	case MarkType::ZoomTop:
		return 13;
	case MarkType::ZoomBottom:
		return 14;
	case MarkType::ZoomSide:
		return 15;
	case MarkType::Stop:
		return 16;
	case MarkType::LaneSplit:
		return 17;
	default:
		return -1;
	}

	return -1;
}

MarkType MarkByIndex(int i) {
	switch (i)
	{
	case 0:
		return MarkType::BPM;
	case 1:
		return MarkType::TimeSignature;
	case 2:
	case 3:
		return MarkType::FXLong;
	case 4:
	case 5:
		return MarkType::FXChip;
	case 6:
	case 7:
		return MarkType::Laser2x;
	case 8:
		return MarkType::Filter;
	case 9:
		return MarkType::KnobVolume;
	case 10:
		return MarkType::SlamSound;
	case 11:
		return MarkType::SlamVolume;
	case 12:
		return MarkType::Tilt;
	case 13:
		return MarkType::ZoomTop;
	case 14:
		return MarkType::ZoomBottom;
	case 15:
		return MarkType::ZoomSide;
	case 16:
		return MarkType::Stop;
	case 17:
		return MarkType::LaneSplit;
	default:
		return MarkType::Error;
	}
}

Side MarkSideByIndex(int i) {
	switch (i)
	{
	case 3:
	case 5:
	case 7:
		return Side::R;
	// 其他都是L
	default:
		return Side::L;
	}
}