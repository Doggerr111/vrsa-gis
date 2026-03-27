#ifndef LOGGER_H
#define LOGGER_H

#define ENABLE_DEBUG_LOGS

#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <QTextEdit>

namespace vrsa {
namespace common {

/**
 * @english
 * @brief Log levels for GIS application
 * @endenglish
 * @russian
 * @brief Уровни логирования для ГИС приложения
 * @endrussian
 */
enum class LogLevel
{
    TRACE,      ///< Детальная отладочная информация
    DEBUG,      ///< Отладочная информация
    INFO,       ///< Общая информация о работе
    WARNING,    ///< Предупреждения
    ERROR,      ///< Ошибки
    CRITICAL    ///< Критические ошибки
};

/**
 * @english
 * @brief Log categories for different GIS components
 * @endenglish
 * @russian
 * @brief Категории логирования для различных компонентов ГИС
 * @endrussian
 */
namespace LogCategory
{
    constexpr const char* CORE      = "CORE";
    constexpr const char* MEMORY    = "MEMORY";

    constexpr const char* GDAL      = "GDAL";
    constexpr const char* PROJ      = "PROJ";
    constexpr const char* NETWORK   = "NET";

    constexpr const char* VECTOR    = "VECTOR";
    constexpr const char* RASTER    = "RASTER";
    constexpr const char* DATABASE  = "DB";

    constexpr const char* RENDERING = "RENDER";
    constexpr const char* UI        = "UI";


}

/**
 * @english
 * @brief Main logger class for GIS development
 * @endenglish
 * @russian
 * @brief Основной класс логгера для разработки ГИС
 * @endrussian
 */
class Logger {
public:

    static Logger& getInstance();


    void setLogLevel(LogLevel level);
    void setLogFile(const std::string& filename);
    void enableConsoleOutput(bool enable);
    void enableCategory(const std::string& category, bool enable);
    void enableThreadLogging(bool enable) { logThreadEnabled = enable; };
    void enableFileLogging(bool enable) { logFileEnabled = enable; };

    void setLogWidget(QTextEdit* widget);                          // главный виджет
    void setCategoryWidget(const std::string& category, QTextEdit* widget);  // по категориям
    void clearWidgets();


    void log(LogLevel level, const std::string& category, const std::string& message);
    void log(LogLevel level, const std::string& category, const std::string& message,
             const char* file, int line, const char* function);


    void trace(const std::string& category, const std::string& message,
               const char* file = "", int line = 0, const char* function = "");
    void debug(const std::string& category, const std::string& message,
               const char* file = "", int line = 0, const char* function = "");
    void info(const std::string& category, const std::string& message,
              const char* file = "", int line = 0, const char* function = "");
    void warning(const std::string& category, const std::string& message,
                 const char* file = "", int line = 0, const char* function = "");
    void error(const std::string& category, const std::string& message,
               const char* file = "", int line = 0, const char* function = "");
    void critical(const std::string& category, const std::string& message,
                  const char* file = "", int line = 0, const char* function = "");


    void startTimer(const std::string& operation);
    void stopTimer(const std::string& operation);
    void logMemoryUsage(const std::string& context);

private:
    Logger();
    ~Logger();

    // Non-copyable
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string getCurrentTime() const;
    std::string levelToString(LogLevel level) const;
    std::string formatMessage(LogLevel level, const std::string& category,
                             const std::string& message, const char* file,
                             int line, const char* function) const;
    std::string formatMessageForWidgets(LogLevel level, const std::string& category,
                             const std::string& message) const;
    QString formatHtml(const std::string& message, LogLevel level, const std::string& category);
    bool shouldLog(LogLevel level, const std::string& category) const;
    void writeToFile(const std::string& message);
    void writeToConsole(const std::string& message);
    void writeToWidgets(const std::string& formatted, LogLevel level, const std::string& category);
    void writeToWidget(QTextEdit* widget, const QString& html);

private:
    std::ofstream logFile;
    std::string logFilename = "log";
    LogLevel currentLevel = LogLevel::INFO;
    bool consoleEnabled = true;

    bool logThreadEnabled = true;
    bool logFileEnabled = true;
    std::mutex logMutex;

    std::unordered_map<std::string, bool> enabledCategories;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> timers;

    //новые поля для GUI
    QTextEdit* mMainWidget = nullptr;
    std::unordered_map<std::string, QTextEdit*> mCategoryWidgets;
    QStringList mPendingMessages;  // для сообщений до установки виджета
    std::mutex mWidgetMutex;
};

}
}

// ============================================================================
// Макросы для удобного использования
// ============================================================================

// Базовые макросы логирования
#define VRSA_TRACE(category, message) \
    vrsa::common::Logger::getInstance().trace(category, message, __FILE__, __LINE__, __FUNCTION__)

#define VRSA_DEBUG(category, message) \
    vrsa::common::Logger::getInstance().debug(category, message, __FILE__, __LINE__, __FUNCTION__)

#define VRSA_INFO(category, message) \
    vrsa::common::Logger::getInstance().info(category, message, __FILE__, __LINE__, __FUNCTION__)

#define VRSA_WARNING(category, message) \
    vrsa::common::Logger::getInstance().warning(category, message, __FILE__, __LINE__, __FUNCTION__)

#define VRSA_ERROR(category, message) \
    vrsa::common::Logger::getInstance().error(category, message, __FILE__, __LINE__, __FUNCTION__)

#define VRSA_CRITICAL(category, message) \
    vrsa::common::Logger::getInstance().critical(category, message, __FILE__, __LINE__, __FUNCTION__)

// Макросы для логирования с проверкой условия
#define VRSA_TRACE_IF(condition, category, message) \
    if (condition) VRSA_TRACE(category, message)

#define VRSA_DEBUG_IF(condition, category, message) \
    if (condition) VRSA_DEBUG(category, message)

// Макросы для логирования GDAL ошибок
#define VRSA_LOG_GDAL_ERROR(category, operation) \
    do { \
        const char* gdal_error = CPLGetLastErrorMsg(); \
        if (gdal_error && strlen(gdal_error) > 0) { \
            VRSA_ERROR(category, std::string(operation) + " - GDAL error: " + gdal_error); \
            CPLErrorReset(); \
        } \
    } while(0)

// Макросы для измерения производительности
#define VRSA_START_TIMER(operation) \
    vrsa::common::Logger::getInstance().startTimer(operation)

#define VRSA_STOP_TIMER(operation) \
    vrsa::common::Logger::getInstance().stopTimer(operation)

#define VRSA_SCOPE_TIMER(operation) \
    vrsa::common::Logger::ScopedTimer scoped_timer_##__LINE__(operation)

// Макрос для логирования памяти
#define VRSA_LOG_MEMORY(context) \
    vrsa::common::Logger::getInstance().logMemoryUsage(context)

#endif // LOGGER_H
