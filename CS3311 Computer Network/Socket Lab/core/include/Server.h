#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "Protocol.h"
#include "Socket.h"

class Server
{
public:
    using ConnectionCallbackType = std::function<void(const Socket &client)>;
    using MessageCallbackType    = std::function<void(const Socket &client, const std::string &message)>;
    using ErrorCallbackType      = std::function<void(const std::string &error)>;

public:
    explicit Server(int port, Socket::Domain domain);
    virtual ~Server();

    void onConnect(ConnectionCallbackType &&callback);
    void onDisconnect(ConnectionCallbackType &&callback);
    void onRequest(MessageCallbackType &&callback);
    void onResponse(MessageCallbackType &&callback);
    void onClientError(MessageCallbackType &&callback);
    void onServerError(ErrorCallbackType &&callback);

private:
    std::unique_ptr<Socket> mSocket;
    std::thread             mListener;
    bool                    mRunning;

    ConnectionCallbackType mOnConnect;
    ConnectionCallbackType mOnDisconnect;
    MessageCallbackType    mOnRequest;
    MessageCallbackType    mOnResponse;
    MessageCallbackType    mOnClientError;
    ErrorCallbackType      mOnServerError;

    void listener();
    void client(std::unique_ptr<Socket> client);
};