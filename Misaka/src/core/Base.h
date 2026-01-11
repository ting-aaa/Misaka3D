#pragma once

// ============================================
// Misaka3D Engine - 基础定义
// ============================================

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

// 平台检测
#ifdef _WIN32
    #define MISAKA_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define MISAKA_PLATFORM_MACOS
#elif defined(__linux__)
    #define MISAKA_PLATFORM_LINUX
#endif

// 调试宏
#ifdef MISAKA_DEBUG
    #define MISAKA_ASSERT(x, msg) if(!(x)) { std::cerr << "Assertion Failed: " << msg << std::endl; __debugbreak(); }
#else
    #define MISAKA_ASSERT(x, msg)
#endif

// 智能指针别名
namespace Misaka {
    template<typename T>
    using Scope = std::unique_ptr<T>;
    
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    template<typename T>
    using Ref = std::shared_ptr<T>;
    
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}
