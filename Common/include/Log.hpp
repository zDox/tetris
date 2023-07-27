#ifndef LOG_HPP
#define LOG_HPP

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class Log{
private:
    static std::shared_ptr<spdlog::logger> core_logger;
    static std::shared_ptr<spdlog::logger> network_logger;
public:
    static void init();

    inline static std::shared_ptr<spdlog::logger>& getCoreLogger() {return core_logger;}
    inline static std::shared_ptr<spdlog::logger>& getNetworkLogger() {return network_logger;}
};

// Core Logger macros
#define CORE_TRACE(...) Log::getCoreLogger()->trace(__VA_ARGS__)
#define CORE_DEBUG(...) Log::getCoreLogger()->debug(__VA_ARGS__)
#define CORE_INFO(...) Log::getCoreLogger()->info(__VA_ARGS__)
#define CORE_WARN(...) Log::getCoreLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...) Log::getCoreLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...) Log::getCoreLogger()->critical(__VA_ARGS__)

// Network Logger macros
#define NETWORK_TRACE(...) Log::getNetworkLogger()->trace(__VA_ARGS__)
#define NETWORK_DEBUG(...) Log::getNetworkLogger()->debug(__VA_ARGS__)
#define NETWORK_INFO(...) Log::getNetworkLogger()->info(__VA_ARGS__)
#define NETWORK_WARN(...) Log::getNetworkLogger()->warn(__VA_ARGS__)
#define NETWORK_ERROR(...) Log::getNetworkLogger()->error(__VA_ARGS__)
#define NETWORK_CRITICAL(...) Log::getNetworkLogger()->critical(__VA_ARGS__)


#endif
