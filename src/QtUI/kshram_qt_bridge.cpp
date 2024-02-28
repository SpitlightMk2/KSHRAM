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

#include "kshram_qt_bridge.h"

#include <sstream>
#include <string>

#include "src/Application/application_bus.h"
#include "src/Chart/chart.h"

/*
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


using namespace std;

namespace KSHRAM{

int ExecuteCommand(const string& input, const string& output, string& log)
{
    // 将cout重定向到这个字符串上
    ostringstream ss;
    cout.rdbuf(ss.rdbuf());
    Chart chart;
    if (!chart.ImportFromFile(input))
    {
        ss << "Failed to open input file!" << endl;
        // 获得所有的log
        log = ss.str();
        // 恢复cout的位置
        cout.rdbuf(cerr.rdbuf());
        return 1;
    }

    ApplicationBus& bus = ApplicationBus::GetInstance();
    try{
        
        bus.BindChart(std::move(chart));
        bus.RunCommands();

        Chart chart_out = bus.GetChart();

        chart_out.GetHeader() = chart.GetHeader();
        chart_out.GetCustomFX() = chart.GetCustomFX();

        chart_out.ExportToFile(output);
    }
    catch(std::exception& e)
    {
        ss << "Unhandled exception occured:\n"
             << e.what() << "\n";
        // 获得所有的log
        log = ss.str();
        // 恢复cout的位置
        cout.rdbuf(cerr.rdbuf());
        return 1;
    }
    

    ErrorCollector& err_collector = bus.GetErrorCollector();
    if (err_collector.ErrorCount() == 0 && err_collector.WarningCount() == 0)
    {
        ss << "Done." << endl;
    }
    else
    {
        ss << "Total Errors: " << err_collector.ErrorCount() << endl;
        ss << "Total Warnings: " << err_collector.WarningCount() << endl;
    }

    bus.Reset();
    // 获得所有的log
    log = ss.str();
    // 恢复cout的位置
    cout.rdbuf(cerr.rdbuf());

    return 0;
}

}
