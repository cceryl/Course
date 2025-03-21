#include "Interface.h"

#include <iostream>
#include <regex>
#include <vector>

using std::bind, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3;
using std::cin, std::cout, std::getline, std::unordered_map, std::pair;
using std::exception, std::out_of_range, std::stoi, std::to_string, std::format;
using std::regex, std::regex_replace, std::unique_ptr, std::make_unique;
using std::vector, std::string, std::string_view, std::isspace;
using std::chrono::milliseconds, std::forward, std::format_string;

Interface::CommandTable Interface::commands = {
    {             "help",             &Interface::help},
    {             "exit",             &Interface::exit},
    {     "client-start",      &Interface::clientStart},
    {   "client-request",    &Interface::clientRequest},
    {"client-statistics", &Interface::clientStatistics},
    {     "server-start",      &Interface::serverStart},
    {      "server-stop",       &Interface::serverStop}
};

void Interface::run()
{
    string command;

    while (mRunning)
    {
        getline(cin, command);
        this->execute(command);
    }
}

void Interface::execute(string_view command)
{
    vector<string> tokens = Interface::tokenize(command);
    if (tokens.empty())
    {
        ArgumentsType message = {
            {"reason", "Empty command"}
        };
        Interface::log("interface", "error", message);
        return;
    }

    auto [commandName, arguments] = Interface::parse(tokens);
    if (!commands.contains(commandName))
    {
        ArgumentsType message = {
            {"reason", "Unknown command"}
        };
        Interface::log("interface", "error", message);
        return;
    }

    auto commandFunction = commands.at(commandName);
    (this->*commandFunction)(arguments);
}


void Interface::help([[maybe_unused]] const ArgumentsType &arguments)
{
    static string messages[] = {"help: Display this message",
                                "exit: Exit the program",
                                "client-start domain=ipv4|ipv6 port=<number> timeout=[milliseconds] retries=[number]: Start the client",
                                "client-request server=<ip> port=<number> file=<path> destination=<path>: Request a file from the server",
                                "client-statistics: Display client statistics",
                                "server-start domain=ipv4|ipv6 port=<number> timeout=[milliseconds] retries=[number]: Start the server",
                                "server-stop: Stop the server"};

    for (const string &message : messages)
        cout << message << '\n';
}

void Interface::exit([[maybe_unused]] const ArgumentsType &arguments) { mRunning = false; }

void Interface::clientStart([[maybe_unused]] const ArgumentsType &arguments)
{
    static unordered_map<string, Socket::Domain> domains = {
        {"ipv4", Socket::Domain::IPv4},
        {"ipv6", Socket::Domain::IPv6}
    };

    try
    {
        if (mClient)
            throw ConnectionError("Client already started");

        auto timeout = arguments.contains("timeout") ? stoi(arguments.at("timeout")) : 5000;
        auto retries = arguments.contains("retries") ? stoi(arguments.at("retries")) : 5;

        mClient = make_unique<Client>(domains.at(arguments.at("domain")), stoi(arguments.at("port")), milliseconds(timeout), retries);
        mClient->setProgressCallback(bind(&Interface::onClientProgress, this, _1, _2));
        mClient->setFinishCallback(bind(&Interface::onClientFinish, this, _1, _2, _3));
        mClient->setErrorCallback(bind(&Interface::onClientError, this, _1));

        ArgumentsType message = {
            {"success", "true"}
        };

        Interface::log("client", "start", message);
    }
    catch (const exception &e)
    {
        ArgumentsType message = {
            {"success",  "false"},
            { "reason", e.what()}
        };
        Interface::log("client", "start", message);
    }
}

void Interface::clientRequest([[maybe_unused]] const ArgumentsType &arguments)
{
    try
    {
        if (!mClient)
            throw ConnectionError("Client not started");

        mClient->download(arguments.at("server"), stoi(arguments.at("port")), arguments.at("file"), arguments.at("destination"));
    }
    catch (const out_of_range &e)
    {
        ArgumentsType message = {
            {"success",              "false"},
            { "reason", "missing parameters"}
        };
        Interface::log("client", "request", message);
    }
    catch (const exception &e)
    {
        ArgumentsType message = {
            {"success",  "false"},
            { "reason", e.what()}
        };
        Interface::log("client", "request", message);
    }
}

void Interface::clientStatistics([[maybe_unused]] const ArgumentsType &arguments)
{
    try
    {
        if (!mClient)
            throw ConnectionError("Client not started");

        auto statistics = mClient->statistics();

        ArgumentsType message = {
            {"successful", to_string(statistics.successful)},
            { "corrupted",  to_string(statistics.corrupted)},
            {      "lost",       to_string(statistics.lost)},
            { "duplicate",  to_string(statistics.duplicate)}
        };
        Interface::log("client", "statistics", message);
    }
    catch (const exception &e)
    {
        ArgumentsType message = {
            {"success",  "false"},
            { "reason", e.what()}
        };
        Interface::log("client", "statistics", message);
    }
}

void Interface::serverStart([[maybe_unused]] const ArgumentsType &arguments)
{
    static unordered_map<string, Socket::Domain> domains = {
        {"ipv4", Socket::Domain::IPv4},
        {"ipv6", Socket::Domain::IPv6}
    };

    try
    {
        if (mServer)
            throw ConnectionError("Server already started");

        auto timeout = arguments.contains("timeout") ? stoi(arguments.at("timeout")) : 5000;
        auto retries = arguments.contains("retries") ? stoi(arguments.at("retries")) : 5;

        mServer = make_unique<Server>(domains.at(arguments.at("domain")), stoi(arguments.at("port")), milliseconds(timeout), retries);
        mServer->setTaskCallback(bind(&Interface::onServerTask, this, _1, _2));
        mServer->setFinishCallback(bind(&Interface::onServerFinish, this, _1, _2));
        mServer->setErrorCallback(bind(&Interface::onServerError, this, _1));
        mServer->start();

        ArgumentsType message = {
            {"success", "true"}
        };
        Interface::log("server", "start", message);
    }
    catch (const out_of_range &e)
    {
        ArgumentsType message = {
            {"success",              "false"},
            { "reason", "missing parameters"}
        };
        Interface::log("server", "start", message);
    }
    catch (const exception &e)
    {
        ArgumentsType message = {
            {"success",  "false"},
            { "reason", e.what()}
        };
        Interface::log("server", "start", message);
    }
}

void Interface::serverStop([[maybe_unused]] const ArgumentsType &arguments)
{
    try
    {
        mServer.reset();

        Interface::log("server", "stop", {});
    }
    catch (const exception &e)
    {
        ArgumentsType message = {
            {"success",  "false"},
            { "reason", e.what()}
        };
        Interface::log("server", "stop", message);
    }
}

void Interface::onClientProgress(size_t total,size_t received)
{
    ArgumentsType message = {
        {   "total",    to_string(total)},
        {"received", to_string(received)}
    };
    Interface::print("\rclient-progress: total={} received={} progress={:.2f}%", total, received, 100.0 * received / total);
    if (total == received)
        Interface::print("\n");
}

void Interface::onClientFinish(string_view file, size_t size, milliseconds duration)
{
    ArgumentsType message = {
        {"file", string(file)},
        {"size", to_string(size)},
        {"duration", to_string(duration.count()) + "ms"}
    };
    Interface::log("client", "finish", message);
}

void Interface::onClientError(string_view error)
{
    ArgumentsType message = {
        {"reason", string(error)}
    };
    Interface::log("client", "error", message);
}

void Interface::onServerTask(const Server::Client &client, string_view path)
{
    ArgumentsType message = {
        {  "IP",              client.IP},
        {"port", to_string(client.port)},
        {"path",           string(path)}
    };
    Interface::log("server", "task", message);
}

void Interface::onServerFinish(const Server::Client &client, string_view path)
{
    ArgumentsType message = {
        {  "IP",              client.IP},
        {"port", to_string(client.port)},
        {"path",           string(path)}
    };
    Interface::log("server", "finish", message);
}

void Interface::onServerError(string_view error)
{
    ArgumentsType message = {
        {"reason", string(error)}
    };
    Interface::log("server", "error", message);
}

vector<string> Interface::tokenize(string_view command)
{
    vector<string> tokens;
    string         token;
    bool           escaping = false;

    for (size_t i = 0; i < command.size(); ++i)
    {
        char current = command[i];

        if (escaping)
        {
            token    += current;
            escaping  = false;
            continue;
        }

        if (current == '\\')
        {
            escaping = true;
            continue;
        }

        if (!isspace(current))
        {
            token += current;
            continue;
        }

        if (!token.empty())
        {
            tokens.push_back(token);
            token.clear();
        }
    }

    if (!token.empty())
        tokens.push_back(token);

    return tokens;
}

pair<string, Interface::ArgumentsType> Interface::parse(vector<string> tokens)
{
    ArgumentsType arguments;

    string command = tokens[0];
    tokens.erase(tokens.begin());

    for (const string &token : tokens)
    {
        size_t pos = token.find('=');
        if (pos == string::npos)
            continue;

        string key   = token.substr(0, pos);
        string value = token.substr(pos + 1);

        arguments[key] = value;
    }

    return {command, arguments};
}

void Interface::log(string_view host, string_view operation, ArgumentsType message)
{
    string buffer;
    buffer += format("{}-{}", host, operation);
    if (!message.empty())
        buffer += ':';
    for (const auto &[key, value] : message)
    {
        string escaped;
        escaped = regex_replace(value, regex(R"(\\)"), "\\\\");
        escaped = regex_replace(escaped, regex(R"(\s)"), "\\ ");

        buffer += format(" {}={}", key, escaped);
    }
    buffer += '\n';
    cout << buffer;
}

template <typename... Args>
inline void Interface::print(format_string<Args...> fmt, Args &&...args)
{
    cout << format(fmt, forward<Args>(args)...);
}