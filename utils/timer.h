#pragma once
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

#if defined(_WIN32)
    #include <windows.h>
    #include <processthreadsapi.h>
#elif defined(__unix__) || defined(__APPLE__)
    #include <time.h>
    #include <sys/resource.h>
#endif

namespace Timer{
    class HighResTimer {
    public:

        // Real time timing
        static double global_now(){
            #if defined(_WIN32)
                LARGE_INTEGER freq, counter;
                QueryPerformanceFrequency(&freq);
                QueryPerformanceCounter(&counter);
                return double(counter.QuadPart) / double(freq.QuadPart);
            #elif defined(__unix__) || defined(__APPLE__)
                timespec ts;
                clock_gettime(CLOCK_MONOTONIC, &ts);
                return ts.tv_sec + ts.tv_nsec * 1e-9;
            #else
                auto tp = std::chrono::steady_clock::now().time_since_epoch();
                return std::chrono::duration<double>(tp).count();
            #endif
        }

        // Process timer, ignore waits and syscalls
        static double cpu_process_now() {
            #if defined(_WIN32)
                FILETIME create, exit, kernel, user;
                if (GetProcessTimes(GetCurrentProcess(), &create, &exit, &kernel, &user)) {
                    ULARGE_INTEGER u;
                    u.LowPart  = user.dwLowDateTime;
                    u.HighPart = user.dwHighDateTime;
                    return double(u.QuadPart) * 1e-7; // 100-ns units → seconds
                }
                return 0.0;
            #elif defined(__unix__) || defined(__APPLE__)
                timespec ts;
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
                return ts.tv_sec + ts.tv_nsec * 1e-9;
            #else
                return double(std::clock()) / CLOCKS_PER_SEC;
            #endif
        }

        // Similar to process time but for a single thread
        static double cpu_thread_now() {
            #if defined(_WIN32)
                FILETIME create, exit, kernel, user;
                if (GetThreadTimes(GetCurrentThread(), &create, &exit, &kernel, &user)) {
                    ULARGE_INTEGER u;
                    u.LowPart  = user.dwLowDateTime;
                    u.HighPart = user.dwHighDateTime;
                    return double(u.QuadPart) * 1e-7;
                }
                return 0.0;
            #elif defined(__unix__) || defined(__APPLE__)
                timespec ts;
                clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
                return ts.tv_sec + ts.tv_nsec * 1e-9;
            #else
                return double(std::clock()) / CLOCKS_PER_SEC; // fallback
            #endif
        }
    };



    enum class TimerMode {
        Global,
        Process,
        Thread
    };


    inline double now(TimerMode mode) {
        switch (mode) {
            case TimerMode::Global:       return HighResTimer::global_now();
            case TimerMode::Process: return HighResTimer::cpu_process_now();
            case TimerMode::Thread:  return HighResTimer::cpu_thread_now();
        }
        return 0.0;
    }

    template <typename Func>
    double measure_time(Func&& f, TimerMode mode = TimerMode::Global, int runs = 1) {
        double total = 0.0;
        for (int i = 0; i < runs; i++) {
            double start = now(mode);
            f();
            double end = now(mode);
            total += (end - start);
        }
        return total / runs;
    }

    inline std::string formatTime(double seconds) {
        const char* units[] = {"s", "ms", "µs", "ns"};
        double values[] = {seconds, seconds * 1e3, seconds * 1e6, seconds * 1e9};

        // Find the best sub-second units
        if (seconds < 1.0) {
            for (int i = 1; i < 4; i++) {
                if (values[i] >= 1.0 && values[i] < 1000.0) {
                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(3) << values[i] << " " << units[i];
                    return oss.str();
                }
            }
            // If it's extremely small (<1ns), default to ns
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3) << values[3] << " ns";
            return oss.str();
        }

        // Find the best super-second units
        const char* bigUnits[] = {"s", "min", "h", "d"};
        double bigValues[] = {seconds, seconds / 60.0, seconds / 3600.0, seconds / 86400.0};

        for (int i = 3; i >= 0; --i) {
            if (bigValues[i] >= 1.0) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(3) << bigValues[i] << " " << bigUnits[i];
                return oss.str();
            }
        }

        std::ostringstream oss;
        oss << seconds << " s";
        return oss.str();;
    }

    class ScopedTimer {
        std::string name;
        TimerMode mode;
        double start;

    public:
        ScopedTimer(std::string label, TimerMode m = TimerMode::Global)
            : name(std::move(label)), mode(m), start(now(m)) {}

        ~ScopedTimer() {
            double end = now(mode);
            double elapsed = end - start;
            std::string modeName = (mode == TimerMode::Global) ? "Global"
                                : (mode == TimerMode::Process) ? "CPU(Process)"
                                : "CPU(Thread)";
            std::cout << "[Timer] " << name << " (" << modeName << "): "
                    << formatTime(elapsed) << "\n";
        }
    };

}