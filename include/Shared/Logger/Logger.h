#pragma once

#include <string>
#include <string_view>
#include <ostream>
#include <mutex>
#include <chrono>
#include <format>
#include <memory>

namespace Logging {
    enum class LogLevel {
        Trace, Debug, Info, Warn, Error, Fatal
    };

    struct LoggerSettings {
        LogLevel minLevel = LogLevel::Info;
        bool includeTimestamp = true;
        bool includeLevel = true;
    };

    class Logger {
    private:
        std::ostream* out;
        LoggerSettings settings;
        std::mutex mutex;

        static std::string_view ToString(LogLevel level) {
            switch (level) {
                case LogLevel::Trace: return "TRACE";
                case LogLevel::Debug: return "DEBUG";
                case LogLevel::Info:  return "INFO";
                case LogLevel::Warn:  return "WARN";
                case LogLevel::Error: return "ERROR";
                case LogLevel::Fatal: return "FATAL";
            }
            return "?";
        }

    public:
        explicit Logger(std::ostream& out, LoggerSettings settings = {})
            : out(&out), settings(std::move(settings)) {}

        void Log(LogLevel level, std::string_view message) {
            if (level < settings.minLevel) return;

            std::lock_guard lock(mutex);

            if (settings.includeTimestamp) {
                auto now = std::chrono::system_clock::now();
                *out << std::format("[{:%H:%M:%S}] ", now);
            }
            if (settings.includeLevel) {
                *out << '[' << ToString(level) << "] ";
            }
            *out << message << '\n';
        }

        void SetLevel(LogLevel level) { settings.minLevel = level; }
        void SetOutput(std::ostream& o) { out = &o; }
        void SetSettings(const LoggerSettings& s) { settings = s; }

        LogLevel GetLevel() const { return settings.minLevel; }
        const LoggerSettings& GetSettings() const { return settings; }
    };

    class LoggerFacade {
    public:
        LoggerFacade() = delete;

        static Logger& Instance() {
            static Logger logger(std::cout);
            return logger;
        }

        static void Trace(std::string_view m) { Instance().Log(LogLevel::Trace, m); }
        static void Debug(std::string_view m) { Instance().Log(LogLevel::Debug, m); }
        static void Info(std::string_view m) { Instance().Log(LogLevel::Info,  m); }
        static void Warn(std::string_view m) { Instance().Log(LogLevel::Warn,  m); }
        static void Error(std::string_view m) { Instance().Log(LogLevel::Error, m); }
        static void Fatal(std::string_view m) { Instance().Log(LogLevel::Fatal, m); }

        static void SetLevel(LogLevel level) { Instance().SetLevel(level); }
        static LogLevel GetLevel() { return Instance().GetLevel(); }
        static void SetOutput(std::ostream& out) { Instance().SetOutput(out); }
        static void SetSettings(const LoggerSettings& s) { Instance().SetSettings(s); }
        static const LoggerSettings& GetSettings() { return Instance().GetSettings(); }
    };
}