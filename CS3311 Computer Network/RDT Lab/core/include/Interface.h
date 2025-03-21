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
    void execute(std::string_view command);

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
    void clientRequest([[maybe_unused]] const ArgumentsType &arguments);
    void clientStatistics([[maybe_unused]] const ArgumentsType &arguments);
    void serverStart([[maybe_unused]] const ArgumentsType &arguments);
    void serverStop([[maybe_unused]] const ArgumentsType &arguments);

    void onClientProgress(std::size_t total, std::size_t received);
    void onClientFinish(std::string_view file, std::size_t size, std::chrono::milliseconds duration);
    void onClientError(std::string_view error);

    void onServerTask(const Server::Client &client, std::string_view path);
    void onServerFinish(const Server::Client &client, std::string_view path);
    void onServerError(std::string_view error);

    static std::vector<std::string>              tokenize(std::string_view command);
    static std::pair<std::string, ArgumentsType> parse(std::vector<std::string> tokens);
    template <typename... Args>
    static void print(std::format_string<Args...> format, Args &&...args);
    static void log(std::string_view host, std::string_view operation, ArgumentsType message);
};