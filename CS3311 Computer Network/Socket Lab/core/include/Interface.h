#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Client.h"
#include "Server.h"

class Interface
{
public:
    explicit Interface() = default;
    virtual ~Interface() = default;

    void run();

private:
    using ArgumentsType = std::unordered_map<std::string, std::string>;
    using CommandTable  = std::unordered_map<std::string, void (Interface::*)(const ArgumentsType &)>;

    std::unique_ptr<Client> mClient;
    std::unique_ptr<Server> mServer;
    bool                    mRunning = true;
    static CommandTable     commands;

    void help([[maybe_unused]] const ArgumentsType &arguments);
    void exit([[maybe_unused]] const ArgumentsType &arguments);
    void clientStart([[maybe_unused]] const ArgumentsType &arguments);
    void clientStop([[maybe_unused]] const ArgumentsType &arguments);
    void clientConnect([[maybe_unused]] const ArgumentsType &arguments);
    void clientDisconnect([[maybe_unused]] const ArgumentsType &arguments);
    void clientRequest([[maybe_unused]] const ArgumentsType &arguments);
    void serverStart([[maybe_unused]] const ArgumentsType &arguments);
    void serverStop([[maybe_unused]] const ArgumentsType &arguments);

    void onTaskCompleted(const Client::Host &host, const Client::Task &task, const std::string &result);
    void onConnect(const Socket &client);
    void onDisconnect(const Socket &client);
    void onRequest(const Socket &client, const std::string &message);
    void onResponse(const Socket &client, const std::string &message);
    void onClientError(const Socket &client, const std::string &message);
    void onServerError(const std::string &error);

    static std::vector<std::string>              tokenize(std::string_view command);
    static std::pair<std::string, ArgumentsType> parse(std::vector<std::string> tokens);
    static void                                  log(std::string_view host, std::string_view operation, ArgumentsType message);
    void                                         execute(std::string_view command);
};