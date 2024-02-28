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

#include "src/misc/utilities.h"

struct ICurveFunctor {
	double amp = 0;
	virtual const std::string_view name() = 0;
	virtual double operator()(double x) = 0;
	virtual bool linearSection(double x) = 0;
};

/* #region Curves */

/// 直线
struct Linear : public ICurveFunctor {
	inline virtual const std::string_view name() override { return "linear"; }
	inline virtual double operator()(double x) override {
		return x;
	}
	inline virtual bool linearSection(double) override {
		return false;
	}
};

/// 二次曲线 y = x^2
struct SqrType1 : public ICurveFunctor {
	inline virtual const std::string_view name() override { return "p"; }
	inline virtual double operator()(double x) override {
		return x * x;
	}
	inline virtual bool linearSection(double) override {
		return false;
	}
};

/// 二次曲线 y = 1.01 * (0.9x + 0.1)^2 - 0.01 
struct SqrType2 : public ICurveFunctor {
	inline virtual const std::string_view name() override { return "p2"; }
	inline virtual double operator()(double x) override {
		double x2 = x * 0.9 + 0.1;
		return 1.01 * x2 * x2 - 0.01;
	}
	inline virtual bool linearSection(double) override {
		return false;
	}
};

/// 三次曲线 y = x^3
struct CubicType1 : public ICurveFunctor {
	inline virtual const std::string_view name() override { return "c"; }
	inline virtual double operator()(double x) override {
		return x * x * x;
	}
	inline virtual bool linearSection(double) override {
		return false;
	}
};

/// 三次曲线 y = 1.008 * （0.8x + 0.2)^3 - 0.008
struct CubicType2 : public ICurveFunctor {
	inline virtual const std::string_view name() override { return "c2"; }
	inline virtual double operator()(double x) override {
		double x2 = x * 0.8 + 0.2;
		return 1.008 * x2 * x2 * x2 - 0.008;
	}
	inline virtual bool linearSection(double) override {
		return false;
	}
};

/// 带平滑起始的直线：y = 35.7142*x^3 (x < 0.1) | y = 1.07142 - 0.07142x (x >= 0.1)
struct SmoothenedLinearType1 : public ICurveFunctor {
	inline virtual const std::string_view name() override { return "sl1"; }
	inline virtual double operator()(double x) override {
		return x < 0.1 ?
			35.7142 * x * x * x :
			1.07142 - 0.07142 * x;
	}
	inline virtual bool linearSection(double x) override {
		return (x >= 0.1);
	}
};

/// 带平滑起始的直线：y = 2.2857*x^2 (x < 0.25) | y = 1.1428 - 0.1428x (x >= 0.25)
struct SmoothenedLinearType2 : public ICurveFunctor {
	inline virtual const std::string_view name() override { return "sl2"; }
	inline virtual double operator()(double x) override {
		return x < 0.25 ?
			2.2857 * x * x :
			1.1428 * x - 0.1428;
	}
	inline virtual bool linearSection(double x) override {
		return (x >= 0.25);
	}
};

/// 正、余弦曲线 y = 1 - cos(pi/2 - x)
struct Sine : public ICurveFunctor {
	inline virtual const std::string_view name() override { return "sin"; }
	inline virtual double operator()(double x) override {
		return 1 - cos(HALF_PI * x);
	}
	inline virtual bool linearSection(double) override {
		return false;
	}
};

/// 指数曲线：y = (e^(amp * x) - amp * x - 1) / (e^(amp) - amp - 1)
///
/// amp: 0.001 ~ 5.0
struct Exp : public ICurveFunctor {
	inline Exp() { this->amp = 1.0; }
	inline virtual const std::string_view name() override { return "exp"; }
	inline virtual double operator()(double x) override {
		amp = amp < 0.0001 ? 0.0001 :
			amp > 5.0 ? 5.0 : amp;
		return (exp(amp * x) - amp * x - 1) / (exp(amp) - amp - 1);
	}
	inline virtual bool linearSection(double) override {
		return false;
	}
};

/// 幂函数曲线： y = x ^ amp
///
/// amp: 1.0 ~ 5.0
struct Polynomial : public ICurveFunctor {
	inline Polynomial() { this->amp = 2.0; }
	inline virtual const std::string_view name() override { return "xn"; }
	inline virtual double operator()(double x) override {
		amp = amp < 1.0 ? 1.0 :
			amp > 5.0 ? 5.0 : amp;
		return pow(x, amp);
	}
	inline virtual bool linearSection(double) override {
		return (this->amp == 1.0);
	}
};

/* #endregion Curves */

/** 通过名字创建仿函数
 *
 * 可用名：
 * - p, p1, parabola, p2, parabola2
 * - c, c1, cubic, c2, cubic2
 * - sl, sl1, smoothlinear, sl2, smoothlinear2
 * - s, sin, sine
 * - e, exp, exponential
 * - xn, polynomial
**/
std::unique_ptr<ICurveFunctor> NewCurveFunctor(const std::string& type);

