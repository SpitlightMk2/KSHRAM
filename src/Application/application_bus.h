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

#include <functional>
#include <list>
#include <vector>

#include "src/Chart/indexed_chart.h"
#include "src/Command/command.h"
#include "src/Command/command_map.h"

#include "src/Errors/error_collector.h"

/* #region Application Interface */

enum class ApplicationType : int
{
    /// 对谱面进行操作的通常类型
    Normal,
    /// 只会产生新的命令的脚本类型
    Scripting,
    /// 仅CommandBatch使用的批处理类型
    Batch
};

#define APP_TYPE(type_name) \
    ApplicationType type() const override { return ApplicationType::type_name; }

class IApplication
{
public:
    friend class ApplicationBus;

protected:
    // 接受的命令名称
    virtual std::vector<std::string> AcceptedCmdName() = 0;

    // 检查命令参数是否合法
    virtual bool CheckArgs(const Command& cmd) = 0;

    // 执行命令
    virtual bool ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart) = 0;

public:
    virtual ~IApplication() = default;

    // ApplicationBus使用的执行入口点
    bool ExecuteCommand(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart);

    virtual ApplicationType type() const { return ApplicationType::Normal; }
};

class ICompiler : public IApplication
{
public:
    friend class ApplicationBus;
    using ErrorStack = std::list<std::string>;

public:
    virtual ApplicationType type() const override { return ApplicationType::Scripting; }

protected:
    virtual bool Compile(const Command& cmd, CommandMap& cmd_map, ErrorStack& err_stack) = 0;

    virtual bool ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart) override;
};

/* #endregion Application Interface */

/* #region Application Bus */

#define GET_ERROR_COLLECTOR \
    ApplicationBus::GetInstance().GetErrorCollector()

class ApplicationBus
{
public:
    using DispatchMap = std::map<std::string, std::vector<std::shared_ptr<IApplication>>>;
    using CompileMap = std::map<std::string, std::vector<std::shared_ptr<ICompiler>>>;
    using ErrorStack = std::list<std::string>;

private:
    // 组件存储区
    std::vector<std::shared_ptr<IApplication>> app_list;
    // 存放各个功能组件的索引表
    DispatchMap app_dispatch_map;
    CompileMap script_compile_map;
    // 谱面本体
    Chart chart_;
    IndexedChart ic_;
    // Logger
    ErrorCollector err_collector;
    

private:
    // singleton
    ApplicationBus();
    ApplicationBus(const ApplicationBus&) = delete;
    ApplicationBus& operator=(const ApplicationBus&) = delete;

    void RegisterAllApplications();

    void InitializeDispatchMap();
    void InitializeCompilerMap();

    TimeInfo ToTimeInfo(int time_number);

public:
    inline ~ApplicationBus();
    /// 获取单例
    inline static ApplicationBus& GetInstance();

    /// 新增一个命令插件
    inline void AppendApplication(std::shared_ptr<IApplication>&& app);
    /// 获取Logger
    inline ErrorCollector& GetErrorCollector();
    /// 设置待处理的谱面
    inline void BindChart(const Chart& chart);
    /// 运行所有命令
    void RunCommands();
    /// 获取处理后的谱面
    inline Chart GetChart();
    /// 重置，清除谱面内容和错误记录
    void Reset();

    /// 检查给定的命令是否格式正确
    bool CheckCommand(const Command& command);
    /// 编译一组命令，使其中所有的命令都是可直接执行的
    CommandMap Compile(const CommandMap& map, ErrorStack& err_stack);
    /// 在特定时刻执行一组命令
    void ExecuteBatch(const CommandMap& batch, int time = 0);
};

/* #endregion Application Bus */

/* #region inline functions */

inline ApplicationBus::~ApplicationBus()
{
    // apps released automatically
}

inline ApplicationBus& ApplicationBus::GetInstance()
{
    static ApplicationBus bus;
    return bus;
}

inline void ApplicationBus::AppendApplication(std::shared_ptr<IApplication>&& app)
{
    this->app_list.push_back(std::forward<std::shared_ptr<IApplication>>(app));
}

inline void ApplicationBus::BindChart(const Chart& chart)
{
    this->chart_ = chart;
    this->ic_.ImportFromChart(this->chart_);
}

inline Chart ApplicationBus::GetChart()
{
    return this->chart_;
}

inline ErrorCollector& ApplicationBus::GetErrorCollector()
{
    return this->err_collector;
}

/* #endregion inline functions */
