#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <iomanip>
#include <string>
#include <memory>
#include <typeinfo>
#include <cxxabi.h>

namespace Tests {
    struct TestResult {
        std::string testName;
        bool passed;
        std::string errorMessage;
        double durationMs;
        
        TestResult(const std::string& name, bool pass, const std::string& error = "", double duration = 0.0)
            : testName(name), passed(pass), errorMessage(error), durationMs(duration) {}
    };

    class TestSuite {
    private:
        std::vector<std::pair<std::string, std::function<bool()>>> tests;
        std::vector<TestResult> results;
        int passedCount = 0;
        int failedCount = 0;
        
    public:
        void AddTest(const std::string& name, std::function<bool()> test) {
            tests.push_back({name, test});
        }
        
        void RunAll() {
            std::cout << "  ЗАПУСК ТЕСТОВОЙ СУИТЫ." << std::endl;
            
            for (const auto& [name, test] : tests) {
                std::cout << "Выполняется: " << name << "... ";
                std::cout.flush();
                
                auto start = std::chrono::high_resolution_clock::now();
                bool passed = false;
                std::string error;
                
                try {
                    passed = test();
                } catch (const std::exception& e) {
                    error = e.what();
                } catch (...) {
                    error = "Неизвестное исключение";
                }
                
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double, std::milli>(end - start).count();
                
                if (passed) {
                    std::cout << "\033[32mПРОЙДЕН\033[0m ✓ (" << std::fixed << std::setprecision(2) << duration << "ms)" << std::endl;
                    passedCount++;
                } else {
                    std::cout << "\033[31mПРОВАЛЕН\033[0m ✗" << std::endl;
                    if (!error.empty()) {
                        std::cout << "Ошибка: " << error << std::endl;
                    }
                    failedCount++;
                }
                
                results.push_back(TestResult(name, passed, error, duration));
            }
            
            PrintSummary();
        }
        
        void PrintSummary() {
            std::cout << "  ИТОГИ ТЕСТИРОВАНИЯ" << std::endl;
            std::cout << "Пройдено: " << passedCount << std::endl;
            std::cout << "Провалено: " << failedCount << std::endl;
            std::cout << "Всего: " << (passedCount + failedCount) << std::endl;
            
            if (failedCount > 0) {
                std::cout << "\nПроваленные тесты:" << std::endl;
                for (const auto& result : results) {
                    if (!result.passed) {
                        std::cout << "  ✗ " << result.testName << std::endl;
                        if (!result.errorMessage.empty()) {
                            std::cout << "    → " << result.errorMessage << std::endl;
                        }
                    }
                }
            }
        }
        
        bool AllPassed() const { return failedCount == 0; }
    };

    #define TEST_ASSERT(condition) \
        if (!(condition)) { \
            std::string msg = "Assertion failed: " #condition; \
            throw std::runtime_error(msg); \
        }

    #define TEST_ASSERT_EQUAL(a, b) \
        if ((a) != (b)) { \
            std::string msg = "Expected " + std::to_string(b) + ", got " + std::to_string(a); \
            throw std::runtime_error(msg); \
        }

    #define TEST_ASSERT_FLOAT_EQUAL(a, b, eps) \
        if (std::abs((a) - (b)) > (eps)) { \
            std::string msg = "Expected " + std::to_string(b) + ", got " + std::to_string(a); \
            throw std::runtime_error(msg); \
        }

    #define TEST_ASSERT_VECTOR_EQUAL(a, b, eps) \
        if (std::abs((a).x - (b).x) > (eps) || \
            std::abs((a).y - (b).y) > (eps) || \
            std::abs((a).z - (b).z) > (eps)) { \
            throw std::runtime_error("Vector mismatch"); \
        }

    #define TEST_ASSERT_THROWS(expr, exception) \
        try { expr; throw std::runtime_error("Expected exception did not throw"); } \
        catch (const exception&) {} \
        catch (...) { throw std::runtime_error("Wrong exception type thrown"); }
}