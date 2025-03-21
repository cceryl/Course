#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>

class Timer
{
public:
    explicit Timer(std::function<void()> callback);
    virtual ~Timer() noexcept       = default;
    Timer(const Timer &)            = delete;
    Timer &operator=(const Timer &) = delete;
    Timer(Timer &&)                 = default;
    Timer &operator=(Timer &&)      = default;

    void start(std::chrono::milliseconds duration, std::size_t repeat);
    void cancel();

private:
    std::function<void()>              mCallback;
    std::shared_ptr<std::atomic<bool>> mRunning;
};