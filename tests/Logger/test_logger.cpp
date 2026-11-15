#include <gtest/gtest.h>
#include <sstream>
#include "Logger/Logger.h"

using namespace Logging;

TEST(LoggerLevels, AllLevelsDemo) {
    Logger logger(std::cout, LoggerSettings{
        .minLevel = LogLevel::Trace,
        .includeTimestamp = true,
        .includeLevel = true,
        .useColors = true,
        .includeThreadId = false
    });

    std::cout << "\n=== ALL LEVELS ===\n";
    logger.Log(LogLevel::Trace, "trace message — самое подробное");
    logger.Log(LogLevel::Debug, "debug message — отладочная инфа");
    logger.Log(LogLevel::Info,  "info message — обычное событие");
    logger.Log(LogLevel::Warn,  "warn message — что-то подозрительное");
    logger.Log(LogLevel::Error, "error message — что-то сломалось");
    logger.Log(LogLevel::Fatal, "fatal message — всё, приехали");
    std::cout << "\n";
}

TEST(LoggerLevels, FilterByLevel) {
    Logger logger(std::cout, LoggerSettings{
        .minLevel = LogLevel::Warn,
        .includeTimestamp = true,
        .includeLevel = true,
        .useColors = true
    });

    std::cout << "\n=== FILTER: minLevel=Warn (Trace/Debug/Info не покажутся) ===\n";
    logger.Log(LogLevel::Trace, "это НЕ должно появиться");
    logger.Log(LogLevel::Debug, "это НЕ должно появиться");
    logger.Log(LogLevel::Info,  "это НЕ должно появиться");
    logger.Log(LogLevel::Warn,  "а это появится");
    logger.Log(LogLevel::Error, "и это появится");
    logger.Log(LogLevel::Fatal, "и это тоже");
    std::cout << "\n";
}

TEST(LoggerFormatting, LongMessages) {
    Logger logger(std::cout, LoggerSettings{
        .minLevel = LogLevel::Trace,
        .includeTimestamp = true,
        .includeLevel = true,
        .useColors = true
    });

    std::cout << "\n=== LONG MESSAGES ===\n";
    logger.Log(LogLevel::Info, "Очень длинное сообщение, чтобы посмотреть как переносится текст и как выглядят бейджи на фоне длинного текста в консоли");
    logger.Log(LogLevel::Error, "Ещё одно длинное сообщение с ошибкой, проверяем что красный цвет и жирность читаются нормально на длинной строке");
    logger.Log(LogLevel::Warn, "И предупреждение тоже длинное, чтобы посмотреть на жёлтый жирный текст в реальной ситуации");
    std::cout << "\n";
}

TEST(LoggerFormatting, NoColors) {
    Logger logger(std::cout, LoggerSettings{
        .minLevel = LogLevel::Trace,
        .includeTimestamp = true,
        .includeLevel = true,
        .useColors = false
    });

    std::cout << "\n=== NO COLORS (plain fallback) ===\n";
    logger.Log(LogLevel::Info,  "info без цветов");
    logger.Log(LogLevel::Warn,  "warn без цветов");
    logger.Log(LogLevel::Error, "error без цветов");
    logger.Log(LogLevel::Fatal, "fatal без цветов");
    std::cout << "\n";
}

TEST(LoggerFormatting, NoTimestampNoLevel) {
    Logger logger(std::cout, LoggerSettings{
        .minLevel = LogLevel::Trace,
        .includeTimestamp = false,
        .includeLevel = false,
        .useColors = true
    });

    std::cout << "\n=== ONLY MESSAGE (no timestamp, no level) ===\n";
    logger.Log(LogLevel::Info,  "только текст");
    logger.Log(LogLevel::Error, "только текст ошибки");
    std::cout << "\n";
}

TEST(LoggerFormatting, ThreadIds) {
    Logger logger(std::cout, LoggerSettings{
        .minLevel = LogLevel::Trace,
        .includeTimestamp = true,
        .includeLevel = true,
        .useColors = true,
        .includeThreadId = true
    });

    std::cout << "\n=== WITH THREAD IDS ===\n";
    logger.Log(LogLevel::Info, "главный поток");
    std::thread t1([&]{ logger.Log(LogLevel::Info,  "поток 1"); });
    std::thread t2([&]{ logger.Log(LogLevel::Warn,  "поток 2"); });
    std::thread t3([&]{ logger.Log(LogLevel::Error, "поток 3"); });
    t1.join(); t2.join(); t3.join();
    std::cout << "\n";
}

TEST(LoggerFormatting, FatalDivider) {
    Logger logger(std::cout, LoggerSettings{
        .minLevel = LogLevel::Trace,
        .includeTimestamp = true,
        .includeLevel = true,
        .useColors = true
    });

    std::cout << "\n=== FATAL + DIVIDER ===\n";
    logger.Log(LogLevel::Info,  "до фатала");
    logger.Log(LogLevel::Fatal, "ФАТАЛ: посмотри на разделитель после этой строки");
    logger.Log(LogLevel::Info,  "после фатала");
    std::cout << "\n";
}

TEST(LoggerSettings, SetLevelRuntime) {
    Logger logger(std::cout, LoggerSettings{
        .minLevel = LogLevel::Error,
        .includeTimestamp = true,
        .includeLevel = true,
        .useColors = true
    });

    std::cout << "\n=== SET LEVEL RUNTIME ===\n";
    std::cout << "-- current minLevel=Error --\n";
    logger.Log(LogLevel::Info,  "info — не покажется");
    logger.Log(LogLevel::Error, "error — покажется");

    logger.SetLevel(LogLevel::Trace);
    std::cout << "-- set minLevel=Trace --\n";
    logger.Log(LogLevel::Trace, "trace — теперь покажется");
    logger.Log(LogLevel::Debug, "debug — теперь покажется");
    logger.Log(LogLevel::Info,  "info  — теперь покажется");
    std::cout << "\n";
}

TEST(LoggerSettings, SetOutputToStream) {
    std::ostringstream capture;
    Logger logger(capture, LoggerSettings{
        .minLevel = LogLevel::Trace,
        .includeTimestamp = false,
        .includeLevel = true,
        .useColors = false
    });

    logger.Log(LogLevel::Info, "поймано в строку");
    logger.Log(LogLevel::Warn, "и это тоже");

    std::cout << "\n=== CAPTURED OUTPUT ===\n";
    std::cout << capture.str();
    std::cout << "\n";

    EXPECT_NE(capture.str().find("поймано в строку"), std::string::npos);
    EXPECT_NE(capture.str().find("и это тоже"),      std::string::npos);
}

TEST(LoggerSettings, FacadeDemo) {
    std::cout << "\n=== FACADE (глобальный логгер) ===\n";
    LoggerFacade::SetLevel(LogLevel::Trace);
    LoggerFacade::Trace("facade trace");
    LoggerFacade::Debug("facade debug");
    LoggerFacade::Info ("facade info");
    LoggerFacade::Warn ("facade warn");
    LoggerFacade::Error("facade error");
    LoggerFacade::Fatal("facade fatal");
    std::cout << "\n";
}