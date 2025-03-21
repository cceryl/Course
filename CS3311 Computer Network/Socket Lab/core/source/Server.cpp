#include "Server.h"

#include <format>
#include <fstream>

using std::exception, std::format;
using std::function, std::move;
using std::ios, std::ifstream;
using std::thread, std::mutex, std::lock_guard;
using std::unique_ptr, std::make_unique;

Server::Server(int port, Socket::Domain domain)
{
    mSocket = make_unique<Socket>(domain, Socket::Type::TCP);
    mSocket->bind(port);
    mSocket->listen();
    mRunning  = true;
    mListener = thread(&Server::listener, this);
}

Server::~Server()
{
    mRunning = false;
    mListener.join();
}

void Server::onConnect(ConnectionCallbackType &&callback) { mOnConnect = callback; }

void Server::onDisconnect(ConnectionCallbackType &&callback) { mOnDisconnect = callback; }

void Server::onRequest(MessageCallbackType &&callback) { mOnRequest = callback; }

void Server::onResponse(MessageCallbackType &&callback) { mOnResponse = callback; }

void Server::onClientError(MessageCallbackType &&callback) { mOnClientError = callback; }

void Server::onServerError(ErrorCallbackType &&callback) { mOnServerError = callback; }

void Server::listener()
{
    try
    {
        while (mRunning)
        {
            auto client = mSocket->accept(false);
            if (!client->isConnected())
                continue;

            if (mOnConnect)
                mOnConnect(*client);

            thread(&Server::client, this, move(client)).detach();
        }
    }
    catch (const exception &e)
    {
        if (mOnServerError)
            mOnServerError(e.what());
    }
}

void Server::client(unique_ptr<Socket> client)
{
    Data buffer;

    try
    {
        while (mRunning)
        {
            auto request = client->receive(false);
            if (request.empty())
                continue;

            buffer.insert(buffer.end(), request.begin(), request.end());
            auto distilled = Protocol::distill(buffer);
            if (distilled.empty())
                continue;

            auto message = Protocol::deserialize(distilled);
            switch (message->type())
            {
            case Protocol::Type::Get:
            {
                auto get = dynamic_cast<GetProtocol *>(message.get());
                if (mOnRequest)
                    mOnRequest(*client, get->path());

                ifstream file(get->path(), ios::binary);
                if (!file)
                {
                    client->send(ErrorProtocol("File not found").serialize());
                    if (mOnResponse)
                        mOnResponse(*client, "File not found");
                    break;
                }

                file.seekg(0, ios::end);
                size_t size = file.tellg();
                file.seekg(0, ios::beg);
                Data buffer(size);
                file.read(reinterpret_cast<char *>(buffer.data()), size);
                client->send(FileProtocol(buffer).serialize());

                if (mOnResponse)
                    mOnResponse(*client, "Success");

                break;
            }
            default: throw ProtocolError("Invalid message type");
            }
        }
    }
    catch (const exception &e)
    {
        if (mOnClientError)
            mOnClientError(*client, e.what());
    }
}
