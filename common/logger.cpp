#include "logger.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#else
    #include <unistd.h>
    #include <sys/resource.h>
#endif

namespace vrsa {
namespace common {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    // Включаем все категории по умолчанию
    enabledCategories[LogCategory::CORE] = true;
    enabledCategories[LogCategory::GDAL] = true;
    enabledCategories[LogCategory::VECTOR] = true;
    enabledCategories[LogCategory::RASTER] = true;
    enabledCategories[LogCategory::UI] = true;
    enabledCategories[LogCategory::DATABASE] = true;
    enabledCategories[LogCategory::RENDERING] = true;
    enabledCategories[LogCategory::MEMORY] = true;

}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);
    currentLevel = level;
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex);

    if (logFile.is_open()) {
        logFile.close();
    }

    logFilename = filename;
    logFile.open(filename, std::ios::app);

    if (!logFile.is_open()) {
        std::cerr << "Cannot open log file: " << filename << std::endl;
    }
}

void Logger::enableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(logMutex);
    consoleEnabled = enable;
}

void Logger::enableCategory(const std::string& category, bool enable) {
    std::lock_guard<std::mutex> lock(logMutex);
    enabledCategories[category] = enable;
}

// Основные методы логирования
void Logger::log(LogLevel level, const std::string& category, const std::string& message) {
    log(level, category, message, "", 0, "");
}

void Logger::log(LogLevel level, const std::string& category, const std::string& message,
                 const char* file, int line, const char* function) {
    if (!shouldLog(level, category)) {
        return;
    }

    std::string formatted_message = formatMessage(level, category, message, file, line, function);

    std::lock_guard<std::mutex> lock(logMutex);

    if (consoleEnabled) {
        writeToConsole(formatted_message);
    }

    if (logFile.is_open()) {
        writeToFile(formatted_message);
    }
}

// Удобные методы
void Logger::trace(const std::string& category, const std::string& message,
                   const char* file, int line, const char* function) {
    log(LogLevel::TRACE, category, message, file, line, function);
}

void Logger::debug(const std::string& category, const std::string& message,
                   const char* file, int line, const char* function) {
    log(LogLevel::DEBUG, category, message, file, line, function);
}

void Logger::info(const std::string& category, const std::string& message,
                  const char* file, int line, const char* function) {
    log(LogLevel::INFO, category, message, file, line, function);
}

void Logger::warning(const std::string& category, const std::string& message,
                     const char* file, int line, const char* function) {
    log(LogLevel::WARNING, category, message, file, line, function);
}

void Logger::error(const std::string& category, const std::string& message,
                   const char* file, int line, const char* function) {
    log(LogLevel::ERROR, category, message, file, line, function);
}

void Logger::critical(const std::string& category, const std::string& message,
                      const char* file, int line, const char* function) {
    log(LogLevel::CRITICAL, category, message, file, line, function);
}

// Мониторинг производительности
void Logger::startTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(logMutex);
    timers[operation] = std::chrono::steady_clock::now();
}

void Logger::stopTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(logMutex);
    auto it = timers.find(operation);
    if (it != timers.end()) {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - it->second);
        info("PERF", operation + " took " + std::to_string(duration.count()) + " ms");
        timers.erase(it);
    }
}

void Logger::logMemoryUsage(const std::string& context) {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        double memory_mb = pmc.WorkingSetSize / (1024.0 * 1024.0);
        debug(LogCategory::MEMORY, context + " - Memory usage: " +
              std::to_string(memory_mb) + " MB");
    }
#else
    // Linux/MacOS
    std::ifstream status_file("/proc/self/status");
    std::string line;
    while (std::getline(status_file, line)) {
        if (line.find("VmRSS:") != std::string::npos) {
            std::istringstream iss(line);
            std::string key;
            long value;
            std::string unit;
            iss >> key >> value >> unit;
            double memory_mb = value / 1024.0;
            debug(LogCategory::MEMORY, context + " - Memory usage: " +
                  std::to_string(memory_mb) + " MB");
            break;
        }
    }
#endif
}

// Приватные методы
std::string Logger::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::TRACE:    return "TRACE";
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARN";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

std::string Logger::formatMessage(LogLevel level, const std::string& category,
                                 const std::string& message, const char* file,
                                 int line, const char* function) const {
    std::stringstream ss;
    ss << "[" << getCurrentTime() << "] "
       << "[" << levelToString(level) << "] "
       << "[" << category << "] "
       << "[" << std::this_thread::get_id() << "] ";

    if (file && strlen(file) > 0) {
        // Берем только имя файла, не полный путь
        const char* filename = strrchr(file, '/');
        if (!filename) filename = strrchr(file, '\\');
        if (filename) filename++;
        else filename = file;

        ss << "[" << filename;
        if (line > 0) ss << ":" << line;
        if (function && strlen(function) > 0) ss << " " << function;
        ss << "] ";
    }

    ss << message;
    return ss.str();
}

bool Logger::shouldLog(LogLevel level, const std::string& category) const {
    // Проверяем уровень
    if (level < currentLevel) {
        return false;
    }

    // Проверяем категорию
    auto it = enabledCategories.find(category);
    if (it != enabledCategories.end() && !it->second) {
        return false;
    }

    return true;
}

void Logger::writeToFile(const std::string& message) {
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.flush(); // Гарантируем запись
    }
}

void Logger::writeToConsole(const std::string& message) {
    // Цвета для разных уровней (только для поддерживающих терминалов)
    static const bool use_colors =
#ifdef _WIN32
        false; // Упрощаем для Windows
#else
        isatty(fileno(stdout));
#endif

    if (use_colors) {
        // ANSI цвета
        std::string color_code;
        if (message.find("[ERROR]") != std::string::npos ||
            message.find("[CRITICAL]") != std::string::npos) {
            color_code = "\033[31m"; // Красный
        } else if (message.find("[WARN]") != std::string::npos) {
            color_code = "\033[33m"; // Желтый
        } else if (message.find("[INFO]") != std::string::npos) {
            color_code = "\033[32m"; // Зеленый
        } else if (message.find("[DEBUG]") != std::string::npos) {
            color_code = "\033[36m"; // Голубой
        }

        if (!color_code.empty()) {
            std::cout << color_code << message << "\033[0m" << std::endl;
            return;
        }
    }

    std::cout << message << std::endl;
}

} // namespace core
} // namespace vrsa
