#pragma once

#include <memory>
#include <spdlog/spdlog.h>

class Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};

#if defined(GL_DEBUG) || defined(GL_RELEASE)
#   define GL_TRACE(...)   ::Log::GetLogger()->trace(__VA_ARGS__);
#   define GL_INFO(...)    ::Log::GetLogger()->info(__VA_ARGS__);
#   define GL_WARN(...)    ::Log::GetLogger()->warn(__VA_ARGS__);
#   define GL_ERROR(...)   ::Log::GetLogger()->error(__VA_ARGS__);
#   define GL_FATAL(...)   ::Log::GetLogger()->critical(__VA_ARGS__);
#elif defined(GL_DIST)
#   define GL_TRACE(...)
#   define GL_INFO(...)
#   define GL_WARN(...)
#   define GL_ERROR(...)
#   define GL_FATAL(...)
#endif // GL_DEBUG || GL_RELEASE
