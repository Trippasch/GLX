#include "Log.h"
#include "ImGuiSink.h"

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init()
{
    spdlog::set_pattern("%^[%T] %v%$");

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<ImGuiSink_mt>());

    // s_Logger = spdlog::stdout_color_mt("APP");
    s_Logger = std::make_shared<spdlog::logger>("APP", begin(sinks), end(sinks));
    s_Logger->set_level(spdlog::level::trace);
}
