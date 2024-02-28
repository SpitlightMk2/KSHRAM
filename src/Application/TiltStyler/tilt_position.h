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

#include <cmath>

namespace TiltStyler_Core
{

struct ITiltPos
{
    double amp;
    virtual double operator()(double curve_pos) = 0;
    virtual void setKnobState(bool has_knob) = 0;
};

struct MaxTiltL : public ITiltPos
{
    MaxTiltL() { amp = 0.02; }
    inline virtual double operator()(double curve_pos) override { return curve_pos * amp; }
    inline virtual void setKnobState(bool) override { /* no-op */ };
};

struct MidTilt : public ITiltPos
{
    MidTilt() { amp = 0.02; }
    inline virtual double operator()(double curve_pos) override { return (curve_pos - 25) * amp; }
    inline virtual void setKnobState(bool) override { /* no-op */ };
};

struct MaxTiltR : public ITiltPos
{
    MaxTiltR() { amp = 0.02; }
    inline virtual double operator()(double curve_pos) override { return (curve_pos - 50) * amp; }
    inline virtual void setKnobState(bool) override { /* no-op */ };
};

struct NoTilt : public ITiltPos
{
    NoTilt() { amp = 0; }
    inline virtual double operator()(double curve_pos) override { return 0; }
    inline virtual void setKnobState(bool) override { /* no-op */ };
};

struct MaxTiltL_Keep : public ITiltPos
{
private:
    double max = -1;
    bool keep = false;
public:
    MaxTiltL_Keep() { amp = 0.02; }
    inline virtual double operator()(double curve_pos) override
    {
        double tilt_pos = curve_pos * amp;
        if(keep && tilt_pos > max)
        {
            max = tilt_pos;
        }
        return keep ? max : tilt_pos;
    }
    inline virtual void setKnobState(bool has_knob) override
    {
        keep = has_knob;
        if(!has_knob)
        {
            max = -1;
        }
    };
};

struct MaxTiltR_Keep : public ITiltPos
{
private:
    double max = 1;
    bool keep = false;
public:
    MaxTiltR_Keep() { amp = 0.02; }
    inline virtual double operator()(double curve_pos) override
    {
        double tilt_pos = (curve_pos - 50) * amp * amp;
        if(keep && tilt_pos < max)
        {
            max = tilt_pos;
        }
        return keep ? max : tilt_pos;
    }
    inline virtual void setKnobState(bool has_knob) override
    {
        keep = has_knob;
        if(!has_knob)
        {
            max = 1;
        }
    };
};

}