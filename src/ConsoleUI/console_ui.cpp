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

#include <filesystem>
#include <iostream>
#include <string>

#include "src/Application/application_bus.h"
#include "src/Chart/chart.h"
#include "src/FileSystem/path_manager.h"
#include "versions.h"

using namespace std;

int ExecuteCommand(const string& input, const string& output)
{
    Chart chart;
    if (!chart.ImportFromFile(input))
    {
        cerr << "Failed to open input file!" << endl;
        return 1;
    }

    ApplicationBus& bus = ApplicationBus::GetInstance();
    try
    {
        bus.BindChart(std::move(chart));
        bus.RunCommands();

        Chart chart_out = bus.GetChart();

        chart_out.GetHeader() = chart.GetHeader();
        chart_out.GetCustomFX() = chart.GetCustomFX();

        chart_out.ExportToFile(output);
    }
    catch (std::exception& e)
    {
        cerr << "Unhandled exception occured:\n"
             << e.what() << "\n";
        return 1;
    }

    ErrorCollector& err_collector = bus.GetErrorCollector();
    if (err_collector.ErrorCount() == 0 && err_collector.WarningCount() == 0)
    {
        cout << "Done." << endl;
    }
    else
    {
        cout << "Total Errors: " << err_collector.ErrorCount() << endl;
        cout << "Total Warnings: " << err_collector.WarningCount() << endl;
    }

    ApplicationBus::GetInstance().Reset();

    return 0;
}

inline string AddPostfix(const string& ksh)
{
    string output = ksh;
    output.insert(ksh.find_last_of('.'), "_out");

    return output;
}

void GreetingMessage()
{
    cout << "KSHRAM " PROJECT_VERSION ", Compiled with MinGW.\n"
         << "\tCopyright (C) 2024 Singular_Photon\n"
         << "===================================================================\n"
         << "This program comes with ABSOLUTELY NO WARRANTY.\n"
         << "This is free software, and you are welcome to redistribute it\n"
         << "under certain conditions; type \"KSHRAM.exe --version\" for details.\n"
         << "===================================================================\n"
         << "Type \"KSHRAM.exe --help\" to see the usage of this program.\n";
}

void HelpMessage()
{
    cout << "Usage:\n"
         << "1. Interactive Start:\n"
         << "\trun KSHRAM.exe directly (with no args).\n"
         << "2. Open in a terminal:\n"
         << "\tKSHRAM.exe [input file] ([output file])\n"
         << "\tOutput file name is optional.\n"
         << "\tIf not given, the output file will be named \"xxx_out.ksh\".\n";
}

void VersionMessage()
{
    cout << "KSHRAM " PROJECT_VERSION ", Compiled with MinGW.\n"
         << "\tCopyright (C) 2024 Singular_Photon\n"
         << "===================================================================\n"
         << "This program is free software: you can redistribute it and/or modify\n"
         << "it under the terms of the GNU General Public License as published by\n"
         << "the Free Software Foundation, either version 3 of the License, or\n"
         << "(at your option) any later version.\n\n"

         << "This program is distributed in the hope that it will be useful,\n"
         << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
         << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
         << "GNU General Public License for more details.\n\n"

         << "You should have received a copy of the GNU General Public License\n"
         << "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n"
         << "===================================================================\n";
}

void InteractiveStart()
{
    cout << "KSHRAM " PROJECT_VERSION ". Compiled with MinGW.\n";
    cout << "Now in Interactive Mode.\n";
    cout << "Enter input ksh file name(or q to exit):" << endl;
    string input, output;
    getline(cin, input);
    while (input != "q")
    {
        cout << "Enter output ksh file name(empty for auto naming):" << endl;
        getline(cin, output);

        if (input.find('.') == string::npos)
        {
            input.append(".ksh");
        }

        if (output.empty())
        {
            output = AddPostfix(input);
        }

        if (output.find('.') == string::npos)
        {
            output.append(".ksh");
        }

        // 输入谱面的路径
        filesystem::path chart_path(input), output_path(output);
        if (output_path.is_relative())
        {
            output_path = chart_path.parent_path().string() + output_path.filename().string();
        }
        PathManager::GetInstance().AddPath(chart_path.parent_path().string());
        ExecuteCommand(input, output);
        PathManager::GetInstance().RemoveLastPath();

        // 接受下一个输入
        cout << "===================================================================\n";
        cout << "Enter input ksh file name(or q to exit):" << endl;
        getline(cin, input);
    }
}

int ConsoleEntrance(int argc, char** argv)
{
    // 登录搜索路径
    // 程序位置
    filesystem::path program_path(argv[0]);
    PathManager::GetInstance().AddPath(program_path.parent_path().string());
    // Current Working Dir
    PathManager::GetInstance().AddPath(filesystem::current_path().string());

    if (argc > 1)
    {
        try
        {
            PathManager::GetInstance().AddPath(argv[1]);
        }
        catch (exception& e)
        {
            cout << "input directory is not pure ASCII, "
                    "and KSHRAM will not be able to load batch files from it.\n";
        }
    }

    // 进入KSHRAM处理
    if (argc == 1)
    {
        InteractiveStart();
        return 0;
    }
    else if (argc == 2)
    {
        string input = argv[1];
        // help message
        if (input == "--help" || input == "-h")
        {
            HelpMessage();
        }
        // version message
        else if (input == "--version" || input == "-v")
        {
            VersionMessage();
        }
        else
        {
            string output = AddPostfix(input);
            return ExecuteCommand(input, output);
        }
    }
    else if (argc == 3)
    {
        string input = argv[1];
        string output = argv[2];
        return ExecuteCommand(input, output);
    }
    else
    {
        GreetingMessage();
    }

    return 0;
}