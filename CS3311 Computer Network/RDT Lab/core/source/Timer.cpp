#include "Timer.h"

#include <thread>

using std::function, std::atomic, std::move;
using std::shared_ptr, std::make_shared;
using std::thread, std::chrono::milliseconds, std::this_thread::sleep_for;

Timer::Timer(function<void()> callback) : mCallback(callback) { mRunning = make_shared<atomic<bool>>(false); }

void Timer::start(milliseconds duration, size_t repeat)
{
    if (*mRunning)
        return;

    *mRunning = true;
    thread(
        [this, duration, repeat, running = mRunning, callback = mCallback]()
        {
            for (size_t i = 0; i < repeat && *running; ++i)
            {
                sleep_for(duration);
                if (!*running)
                    break;
                callback();
            }
        })
        .detach();
}

void Timer::cancel() { *mRunning = false; }