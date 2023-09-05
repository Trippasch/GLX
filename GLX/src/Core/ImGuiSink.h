#pragma once

#include "Console.h"
#include <spdlog/sinks/base_sink.h>

template <typename Mutex> class ImGuiSink : public spdlog::sinks::base_sink<Mutex>
{
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
        Console::AddLog(fmt::to_string(formatted).c_str());
    }

    void flush_() override
    {
    }
};

using ImGuiSink_mt = ImGuiSink<std::mutex>;
using ImGuiSink_st = ImGuiSink<spdlog::details::null_mutex>;