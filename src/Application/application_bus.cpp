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

#include "application_bus.h"

using namespace std;

/* #region IApplication & ICompiler */

inline bool IApplication::ExecuteCommand(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart)
{
#ifdef DEBUG
    auto accepted_cmd_name = this->AcceptedCmdName();
    if (std::find(accepted_cmd_name.begin(),
                  accepted_cmd_name.end(),
                  cmd.cmd()) == accepted_cmd_name.end())
    {
        return false;
    }
#endif

    return ProcessCmd(cmd, cmd_map, chart);
}

bool ICompiler::ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart)
{
    CommandMap lambda_batch;
    ErrorStack err_stack;

    // COMPILE
    bool success = Compile(cmd, lambda_batch, err_stack);
    if (!err_stack.empty())
    {
        std::string all_invalid_cmds = "";
        for (auto& cmd : err_stack)
        {
            all_invalid_cmds += "\n\t" + cmd;
        }

        GET_ERROR_COLLECTOR.ErrorLog("Failed to load this batch. Detected following command with invalid syntax:" + all_invalid_cmds);
        return false;
    }

    // EXECUTE
    auto& err_collector = GET_ERROR_COLLECTOR;
    err_collector.AddToStack(cmd.toOriginalString());
    ApplicationBus::GetInstance().ExecuteBatch(lambda_batch);
    err_collector.PopStack();

    return success;
}

/* #endregion IApplication & ICompiler */

/* #region Application Bus */

// 各类application的声明头文件
#include "CameraAmp/camera_amp.h"
#include "CommandBatch/command_batch.h"
#include "CurveKnob/curve_knob.h"
#include "BasicInsertion/knob_approx.h"
#include "BasicInsertion/note_approx.h"
#include "BasicInsertion/write_mark.h"
#include "SVFX/svfx.h"
#include "Scripting/delay.h"
#include "Scripting/looper.h"
#include "SmoothCamera/smooth_camera.h"
#include "SmoothCamera/smooth_camera_increment.h"
#include "Swing/swing.h"
#include "TiltStyler/tilt_styler.h"

#define REGISTER_UTILS(name) this->app_list.push_back(std::make_shared<name>())

void ApplicationBus::RegisterAllApplications()
{
    // REGISTER_UTILS(...)
    REGISTER_UTILS(CurveKnob);
    REGISTER_UTILS(SmoothCamera);
    REGISTER_UTILS(SmoothCameraIncrement);
    REGISTER_UTILS(SVFX);
    REGISTER_UTILS(CameraAmp);
    REGISTER_UTILS(Swing);
    REGISTER_UTILS(TiltStyler);
    REGISTER_UTILS(NoteApprox);
    REGISTER_UTILS(KnobApprox);
    REGISTER_UTILS(KnobAddApprox);
    REGISTER_UTILS(KnobSlamApprox);

    REGISTER_UTILS(Looper);
    REGISTER_UTILS(Delay);
    REGISTER_UTILS(WriteMark);
    
    REGISTER_UTILS(CommandBatch);
    return;
}

ApplicationBus::ApplicationBus()
{
    RegisterAllApplications();
    InitializeDispatchMap();
    InitializeCompilerMap();
}

TimeInfo ApplicationBus::ToTimeInfo(int time_number)
{
    int measure_id = this->chart_.MeasureIDAtTime(time_number);
    const auto& measure = this->chart_[measure_id];
    // 如果Chart中的小节数量不足，那么根据最后一小节的拍号顺延。
    int measure_size = measure.TotalTimespan();
    int local_time = time_number - measure.StartTime();
    measure_id += local_time / measure_size;
    local_time = local_time % measure_size;
    int gcd = std::gcd(local_time, 48);
    int denom = local_time == 0 ? 4 : 192 / gcd;

    TimeInfo output;
    output.measure = measure_id;
    output.entry_numer = local_time / gcd;
    output.entry_denom = denom;

    return output;
}

void ApplicationBus::InitializeDispatchMap()
{
    this->app_dispatch_map.insert({"__DEFAULT__", {/*empty vector*/}});
    for (auto& app : this->app_list)
    {
        vector<string> cmd_names = app->AcceptedCmdName();
        if (!cmd_names.empty())
        {
            for (string name : cmd_names)
            {
                // new command
                if (this->app_dispatch_map.find(name) == this->app_dispatch_map.end())
                {
                    this->app_dispatch_map.insert({name, {/*empty vector*/}});
                }

                this->app_dispatch_map[name].push_back(app);
            }
        }
        else
        {
            this->app_dispatch_map["__DEFAULT__"].push_back(app);
        }
    }
}

void ApplicationBus::InitializeCompilerMap()
{
    for (auto& app : this->app_list)
    {
        if (app->type() == ApplicationType::Scripting)
        {
            vector<string> cmd_names = app->AcceptedCmdName();
            if (!cmd_names.empty())
            {
                for (string name : cmd_names)
                {
                    // new command
                    if (this->script_compile_map.find(name) == this->script_compile_map.end())
                    {
                        this->script_compile_map.insert({name, {/*empty vector*/}});
                    }

                    this->script_compile_map[name].push_back(std::reinterpret_pointer_cast<ICompiler>(app));
                }
            }
        }
    }
}

void ApplicationBus::RunCommands()
{
    // 获取注释表
    auto& comment_list = this->ic_.CommentList();

    CommandMap cmd_map(comment_list);
    CommandMap failed_map;  // 执行不了的东西

    unsigned int current_delay = 0, next_delay = UINT32_MAX;
    while (!cmd_map.Empty())
    {
        for (auto iter = cmd_map.begin(); iter != cmd_map.end();)
        {
            Command command = iter->second;
            // 延迟判断
            if (command.delay() > current_delay)
            {
                next_delay = min(next_delay, command.delay());
                ++iter;
                continue;
            }
            else
            {
                // 设置Logger
                auto time_info = this->ToTimeInfo(command.time());
                err_collector.SetRootCmdTime(time_info);
                err_collector.SetExecTime(time_info);
                err_collector.SetCommand(command.toOriginalString());
                bool success = false;
                // 执行
                auto entrance = this->app_dispatch_map.find(command.cmd());
                if (entrance != this->app_dispatch_map.end())
                {
                    bool cmd_checked = false;
                    for (std::shared_ptr<IApplication>& app : entrance->second)
                    {
                        cmd_checked = app->CheckArgs(command);
                        if (cmd_checked)
                        {
                            success = app->ExecuteCommand(command, cmd_map, ic_);
                            if (success)
                            {
                                break;
                            }
                        }
                    }
                    if (!cmd_checked)
                    {
                        // 如果没有任何一个插件接受此命令，那么报检查错误
                        this->err_collector.ErrorLog(ErrorMessage<ErrorType::InvalidArgs>());
                    }
                }

                else  // start at __DEFAULT__
                {
                    auto default_apps = this->app_dispatch_map.at("__DEFAULT__");
                    for (std::shared_ptr<IApplication>& app : default_apps)
                    {
                        if (app->CheckArgs(command))
                        {
                            success = app->ExecuteCommand(command, cmd_map, ic_);
                            if (success)
                            {
                                break;
                            }
                        }
                    }
                }

                auto iter_before_increase = iter;
                ++iter;
                // 把没有成功执行的命令移动到failed_map里面
                if (!success)
                {
                    failed_map.insert(iter_before_increase->first, iter_before_increase->second);
                }
                cmd_map.erase(iter_before_increase);
            }
        }
        // 下一个延迟点
        current_delay = next_delay;
        next_delay = UINT32_MAX;
    }

    // 写回
    ic_.CommentList() = failed_map.ExportToComment();

    this->chart_ = ic_.ExportToChart();
}

bool ApplicationBus::CheckCommand(const Command& command)
{
    bool cmd_is_valid = false;
    // 看看命令是否合法
    auto entrance = this->app_dispatch_map.find(command.cmd());
    if (entrance != this->app_dispatch_map.end())
    {
        for (std::shared_ptr<IApplication>& app : entrance->second)
        {
            // 只要其中某一个插件认为合法就行（因为执行的时候也是这样）
            cmd_is_valid = app->CheckArgs(command);
            if (cmd_is_valid)
            {
                break;
            }
        }
    }

    return cmd_is_valid;
}

CommandMap ApplicationBus::Compile(const CommandMap& map, ErrorStack& err_stack)
{
    CommandMap input_map = map;
    CommandMap compiled_map;
    while (!input_map.Empty())
    {
        for (auto iter = input_map.begin(); iter != input_map.end();)
        {
            Command command = iter->second;
            bool cmd_is_valid = false, cmd_is_not_script = false;
            // 过滤掉空命令
            if (!command.empty())
            {
                // 执行script命令
                auto entrance = this->script_compile_map.find(command.cmd());
                if (entrance != this->script_compile_map.end())
                {
                    for (std::shared_ptr<ICompiler>& app : entrance->second)
                    {
                        cmd_is_valid = app->Compile(command, input_map, err_stack);
                        if (cmd_is_valid)
                        {
                            break;
                        }
                    }
                }
                else
                {
                    cmd_is_not_script = true;
                }

                // 登记非script命令
                if (cmd_is_not_script)
                {
                    cmd_is_valid = CheckCommand(iter->second);
                    if (cmd_is_valid)
                    {
                        compiled_map.insert(iter->first, iter->second);
                    }
                }
                // 错误命令转为字符串之后扔进err_stack。
                if (!cmd_is_valid)
                {
                    err_stack.push_back(iter->second.toString());
                }
            }

            auto iter_before_increase = iter;
            ++iter;
            input_map.erase(iter_before_increase);
        }
    }

    return compiled_map;
}

void ApplicationBus::ExecuteBatch(const CommandMap& batch, int time)
{
    CommandMap input_map = batch.Offset(time);
    CommandMap failed_map;
    while (!input_map.Empty())
    {
        for (auto iter = input_map.begin(); iter != input_map.end();)
        {
            Command command = iter->second;
            bool success = false;
            // 执行
            auto entrance = this->app_dispatch_map.find(command.cmd());
            if (entrance != this->app_dispatch_map.end())
            {
                // 设置Logger
                err_collector.SetExecTime(this->ToTimeInfo(command.time()));
                err_collector.SetCommand(command.toOriginalString());
                bool cmd_checked = false;
                for (std::shared_ptr<IApplication>& app : entrance->second)
                {
                    cmd_checked = app->CheckArgs(command);
                    if (cmd_checked)
                    {
                        success = app->ExecuteCommand(command, input_map, this->ic_);
                        if (success)
                        {
                            break;
                        }
                    }
                }

                if (!cmd_checked)
                {
                    // 如果没有任何一个插件认为命令参数合法，那么报错
                    this->err_collector.ErrorLog(ErrorMessage<ErrorType::InvalidArgs>());
                }
            }

            auto iter_before_increase = iter;
            ++iter;
            if (!success)
            {
                failed_map.insert(iter_before_increase->first, iter_before_increase->second);
            }
            input_map.erase(iter_before_increase);
        }
    }
}

void ApplicationBus::Reset()
{
    this->err_collector.Reset();
    this->chart_ = Chart();
    this->ic_ = IndexedChart();
}

/* #endregion Application Bus */