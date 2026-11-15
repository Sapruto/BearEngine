#pragma once

#include <string_view>
#include <ostream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <format>
#include <iomanip>
#include <thread>
#include <functional>

namespace Logging {
    enum class LogLevel {
        Trace, Debug, Info, Warn, Error, Fatal
    };

    struct LoggerSettings {
        LogLevel minLevel = LogLevel::Info;
        bool includeTimestamp = true;
        bool includeLevel = true;
        bool useColors = true;
        bool includeThreadId = false;
    };

    class Logger {
    private:
        std::ostream* out;
        LoggerSettings settings;
        std::mutex mutex;

        static constexpr const char* RESET = "\033[0m";
        static constexpr const char* BOLD = "\033[1m";
        static constexpr const char* DIM = "\033[2m";
        static constexpr const char* BLINK = "\033[5m";

        static constexpr const char* FG_GRAY = "\033[38;5;245m";
        static constexpr const char* FG_CYAN = "\033[38;5;51m";
        static constexpr const char* FG_BLUE = "\033[38;5;39m";
        static constexpr const char* FG_YELLOW = "\033[38;5;226m";
        static constexpr const char* FG_RED = "\033[38;5;196m";
        static constexpr const char* FG_MAGENTA = "\033[38;5;201m";
        static constexpr const char* FG_WHITE = "\033[38;5;255m";

        static constexpr const char* BG_GRAY = "\033[48;5;240m";
        static constexpr const char* BG_CYAN = "\033[48;5;30m";
        static constexpr const char* BG_BLUE = "\033[48;5;25m";
        static constexpr const char* BG_YELLOW = "\033[48;5;178m";
        static constexpr const char* BG_RED = "\033[48;5;160m";
        static constexpr const char* BG_MAGENTA = "\033[48;5;127m";

        struct LevelStyle {
            const char* name;
            const char* fg;
            const char* bg;
        };

        static LevelStyle StyleOf(LogLevel level) {
            switch (level) {
                case LogLevel::Trace: return { "TRACE", FG_GRAY,    BG_GRAY    };
                case LogLevel::Debug: return { "DEBUG", FG_CYAN,    BG_CYAN    };
                case LogLevel::Info:  return { "INFO ", FG_BLUE,    BG_BLUE    };
                case LogLevel::Warn:  return { "WARN ", FG_YELLOW,  BG_YELLOW  };
                case LogLevel::Error: return { "ERROR", FG_RED,     BG_RED     };
                case LogLevel::Fatal: return { "FATAL", FG_MAGENTA, BG_MAGENTA };
            }
            return { "?????", FG_WHITE, BG_GRAY };
        }

        static std::string_view ToString(LogLevel level) {
            return StyleOf(level).name;
        }

        void WriteBadge(std::ostream& o, LogLevel level) const {
            auto s = StyleOf(level);
            if (settings.useColors) {
                o << BOLD << s.fg << "\u2590" << RESET;
                o << BOLD << s.fg << s.bg << "  " << s.name << "  " << RESET;
                o << BOLD << s.fg << "\u258C" << RESET;
            }
            else {
                o << "[ " << s.name << " ]";
            }
        }

        void WriteTimestamp(std::ostream& o) const {
            using namespace std::chrono;
            auto now = system_clock::now();
            auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

            if (settings.useColors) {
                o << DIM << FG_GRAY << "[" << RESET;
                o << FG_GRAY << std::format("{:%H:%M:%S}", now);
                o << '.' << std::setfill('0') << std::setw(3) << ms.count();
                o << DIM << "]" << RESET;
            }
            else {
                o << std::format("[{:%H:%M:%S}", now) << '.'
                    << std::setfill('0') << std::setw(3) << ms.count() << ']';
            }
            o << ' ';
        }

        void WriteThreadId(std::ostream& o) const {
            auto id = std::hash<std::thread::id>{}(std::this_thread::get_id()) & 0xFFF;
            if (settings.useColors) {
                o << DIM << FG_MAGENTA << "[T:" << id << "]" << RESET << ' ';
            } else {
                o << "[T:" << id << "] ";
            }
        }

        void WriteMessage(std::ostream& o, LogLevel level, std::string_view msg) const {
            if (!settings.useColors) { o << msg; return; }
            switch (level) {
                case LogLevel::Trace: o << DIM << FG_GRAY << msg << RESET;              break;
                case LogLevel::Debug: o << FG_CYAN << msg << RESET;                     break;
                case LogLevel::Info:  o << FG_WHITE << msg << RESET;                    break;
                case LogLevel::Warn:  o << BOLD << FG_YELLOW << msg << RESET;           break;
                case LogLevel::Error: o << BOLD << FG_RED << msg << RESET;              break;
                case LogLevel::Fatal: o << BOLD << BLINK << FG_MAGENTA << msg << RESET; break;
            }
        }

    public:
        explicit Logger(std::ostream& out, LoggerSettings settings = {})
            : out(&out), settings(std::move(settings)) {}

        void Log(LogLevel level, std::string_view message) {
            if (level < settings.minLevel) return;

            std::lock_guard lock(mutex);

            if (settings.includeTimestamp) WriteTimestamp(*out);
            if (settings.includeThreadId)  WriteThreadId(*out);
            if (settings.includeLevel) {
                WriteBadge(*out, level);
                *out << ' ';
            }
            WriteMessage(*out, level, message);
            *out << '\n';

            if (level == LogLevel::Fatal) {
                if (settings.useColors) {
                    *out << BOLD << FG_MAGENTA
                        << "================================================================="
                        << RESET << '\n';
                }
                else {
                    *out << "-----------------------------------------------------------------\n";
                }
                out->flush();
            }
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
        static void Info(std::string_view m)  { Instance().Log(LogLevel::Info,  m); }
        static void Warn(std::string_view m)  { Instance().Log(LogLevel::Warn,  m); }
        static void Error(std::string_view m) { Instance().Log(LogLevel::Error, m); }
        static void Fatal(std::string_view m) { Instance().Log(LogLevel::Fatal, m); }

        static void SetLevel(LogLevel level) { Instance().SetLevel(level); }
        static LogLevel GetLevel() { return Instance().GetLevel(); }
        static void SetOutput(std::ostream& out) { Instance().SetOutput(out); }
        static void SetSettings(const LoggerSettings& s) { Instance().SetSettings(s); }
        static const LoggerSettings& GetSettings() { return Instance().GetSettings(); }
    };
}