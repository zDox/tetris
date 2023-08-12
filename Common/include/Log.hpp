#ifndef LOG_HPP
#define LOG_HPP

#include <format>
#include <iostream>

// Core Logger macros
#define CORE_TRACE(...) std::cout << std::format(__VA_ARGS__) << "\n";
#define CORE_DEBUG(...) std::cout << std::format(__VA_ARGS__) << "\n";
#define CORE_INFO(...) std::cout << std::format(__VA_ARGS__) << "\n";
#define CORE_WARN(...) std::cout << std::format(__VA_ARGS__) << "\n";
#define CORE_ERROR(...) std::cout << std::format(__VA_ARGS__) << "\n";
#define CORE_CRITICAL(...) std::cout << std::format(__VA_ARGS__) << "\n";

// Network Logger macros
#define NETWORK_TRACE(...) std::cout << std::format(__VA_ARGS__) << "\n";
#define NETWORK_DEBUG(...) std::cout << std::format(__VA_ARGS__) << "\n";
#define NETWORK_INFO(...) std::cout << std::format(__VA_ARGS__) << "\n";
#define NETWORK_WARN(...) std::cout << std::format(__VA_ARGS__) << "\n";
#define NETWORK_ERROR(...) std::cout << std::format(__VA_ARGS__) << "\n";
#define NETWORK_CRITICAL(...) std::cout << std::format(__VA_ARGS__) << "\n";

#endif
