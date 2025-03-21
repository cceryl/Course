#include "Channel.h"
#include "Checksum.h"
#include "Timer.h"

using Handle  = Channel::Handle;
using Task    = Channel::Task;
using Message = Channel::Message;

using std::exception, std::function;
using std::int64_t, std::uint32_t, std::size_t;
using std::string, std::byte, std::move, std::copy, std::back_inserter;
using std::thread, std::this_thread::sleep_for, std::mutex, std::lock_guard, std::unique_lock;
using std::chrono::high_resolution_clock, std::chrono::duration_cast;
using std::chrono::milliseconds, std::chrono::microseconds;

Channel::Channel(Socket::Domain domain, int port) : mSocket(domain, Socket::Type::Custom), mHandle(0), mRunning(true)
{
    mSocket.bind(port);
    mCorrupted  = 0;
    mLost       = 0;
    mSuccessful = 0;
    mReceiver   = thread([this]() { this->receiver(); });
}

Channel::~Channel() noexcept
{
    mRunning = false;
    mReceiver.join();
}

Handle Channel::send(Task task, milliseconds timeout, size_t retries)
{
    Handle handle;

    {
        lock_guard<mutex> lock(mMutex);
        handle            = mHandle;
        TaskDetail detail = {task, timeout, 0, retries, Timer([this, handle]() { this->sender(handle); })};

        detail.timer.start(timeout, retries + 1);
        mTasks.insert({handle, move(detail)});

        ++mHandle;
    }

    this->sender(handle);
    return handle;
}

void Channel::terminate(Handle handle)
{
    lock_guard<mutex> lock(mMutex);

    auto iter = mTasks.find(handle);
    if (iter == mTasks.end())
        return;

    auto &[_, detail] = *iter;
    detail.timer.cancel();
    mTasks.erase(iter);
}

void Channel::setRetryCallback(std::function<void(Task)> callback) noexcept { mRetryCallback = callback; }

void Channel::setTimeoutCallback(function<void(Task)> callback) noexcept { mTimeoutCallback = callback; }

void Channel::setReceiveCallback(function<void(Message)> callback) noexcept { mReceiveCallback = callback; }

Data Channel::encode(const Data &data, size_t tries) const
{
    static constexpr unsigned size  = 4 + 4 + 8;
    static auto               write = [](Data &data, unsigned offset, auto value) { reinterpret_cast<decltype(value) &>(data[offset]) = value; };

    Data header(size, std::byte{0x00});

    write(header, 4, static_cast<uint32_t>(tries));

    auto now       = high_resolution_clock::now();
    auto timestamp = duration_cast<milliseconds>(now.time_since_epoch()).count();
    write(header, 8, static_cast<int64_t>(timestamp));

    copy(data.cbegin(), data.cend(), back_inserter(header));

    CRC32 checksum;
    checksum.compute(header.cbegin() + 4, header.cend());
    auto digest = checksum.digest();
    write(header, 0, reinterpret_cast<uint32_t &>(*digest.data()));

    return header;
}

auto Channel::decode(Data &data) const
{
    struct Header
    {
        bool     valid;
        uint32_t checksum;
        uint32_t tries;
        int64_t  timestamp;
    };

    static constexpr unsigned size = 4 + 4 + 8;
    static auto read = [](Data &data, unsigned offset, auto &value) { value = reinterpret_cast<const decltype(value) &>(data[offset]); };

    if (data.size() < size)
        return Header{false, 0, 0, 0};

    uint32_t checksum;
    uint32_t tries;
    int64_t  timestamp;
    read(data, 0, checksum);
    read(data, 4, tries);
    read(data, 8, timestamp);

    CRC32 check;
    check.compute(data.cbegin() + 4, data.cend());
    if (checksum != reinterpret_cast<uint32_t &>(*check.digest().data()))
        return Header{false, 0, 0, 0};

    data.erase(data.begin(), data.begin() + size);
    return Header{true, checksum, tries, timestamp};
}

void Channel::sender(Handle handle)
{
    unique_lock<mutex> lock(mMutex);
    auto               iter = mTasks.find(handle);
    if (iter == mTasks.end())
        return;

    auto &[_, detail] = *iter;
    if (detail.tries > detail.retries)
    {
        if (detail.retries != 0 && mTimeoutCallback)
        {
            lock.unlock();
            mTimeoutCallback(detail.task);
        }
        return;
    }

    auto data = this->encode(detail.task.data, detail.tries);
    mSocket.send(data, detail.task.IP, detail.task.port);
    if (detail.tries++ != 0 && mRetryCallback)
    {
        Task task = detail.task;
        lock.unlock();
        mRetryCallback(task);
    }
}

void Channel::receiver() noexcept
{
    static auto interval = microseconds(10);

    while (!mReceiveCallback && mRunning)
        sleep_for(interval);

    while (mRunning)
    {
        string IP;
        int    port;
        try
        {
            Data data = mSocket.receive(IP, port, false);
            if (data.empty())
                continue;

            auto [valid, checksum, tries, timestamp] = this->decode(data);
            if (!valid)
            {
                ++mCorrupted;
                continue;
            }

            if (tries > 0)
                ++mLost;
            ++mSuccessful;

            auto time = high_resolution_clock::time_point(milliseconds(timestamp));
            mReceiveCallback(Message{IP, port, move(data), time, high_resolution_clock::now()});
        }
        catch (const exception &e)
        {
            continue;
        }
    }
}
