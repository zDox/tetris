#include "Log.hpp"


std::shared_ptr<spdlog::logger> Log::core_logger;
std::shared_ptr<spdlog::logger> Log::network_logger;

void Log::init(){
    spdlog::set_pattern("%^[%T] %n: %v%$");
    
    core_logger = spdlog::stdout_color_mt("CORE");
    core_logger->set_level(spdlog::level::trace);

    network_logger = spdlog::stdout_color_mt("NETWORK");
    network_logger->set_level(spdlog::level::trace);
}
