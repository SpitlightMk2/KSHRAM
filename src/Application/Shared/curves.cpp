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

#include "curves.h"

using namespace std;

std::unique_ptr<ICurveFunctor> NewCurveFunctor(const std::string& type) {
	if (type == "p" || type == "p1" || type == "parabola") {
		return std::unique_ptr<ICurveFunctor>(new SqrType1());
	}
	else if (type == "p2" || type == "parabola2") {
		return std::unique_ptr<ICurveFunctor>(new SqrType2());
	}
	else if (type == "c" || type == "c1" || type == "cubic") {
		return std::unique_ptr<ICurveFunctor>(new CubicType1());
	}
	else if (type == "c2" || type == "cubic2") {
		return std::unique_ptr<ICurveFunctor>(new CubicType2());
	}
	else if (type == "sl" || type == "sl1" || type == "smoothlinear") {
		return std::unique_ptr<ICurveFunctor>(new SmoothenedLinearType1());
	}
	else if (type == "sl2" || type == "smoothlinear2") {
		return std::unique_ptr<ICurveFunctor>(new SmoothenedLinearType2());
	}
	else if (type == "s" || type == "sin" || type == "sine") {
		return std::unique_ptr<ICurveFunctor>(new Sine());
	}
	else if (type == "e" || type == "exp" || type == "exponential") {
		return std::unique_ptr<ICurveFunctor>(new Exp());
	}
	else if (type == "xn" || type == "polynomial") {
		return std::unique_ptr<ICurveFunctor>(new Polynomial());
	}
	else {
		return nullptr;
	}
}