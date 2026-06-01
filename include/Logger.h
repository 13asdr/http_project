#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>
#include <vector>

class Logger
{
public:
    static void init()
    {
        // 两个输出目标：控制台 + 文件
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/app.log", false);

        consoleSink->set_level(spdlog::level::info);
        fileSink->set_level(spdlog::level::info);

        // 日志格式：[时间] [级别] 内容
        std::string pattern = "[%Y-%m-%d %H:%M:%S] [%^%l%$] %v";
        consoleSink->set_pattern(pattern);
        fileSink->set_pattern(pattern);

        std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};
        auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::info);

        spdlog::set_default_logger(logger);
    }

    static void info(const std::string &_msg) { spdlog::info(_msg); }
    static void warn(const std::string &_msg) { spdlog::warn(_msg); }
    static void error(const std::string &_msg) { spdlog::error(_msg); }
};