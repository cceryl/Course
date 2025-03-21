#include "Interface.h"

#include <iostream>
#include <regex>
#include <vector>

using std::bind, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3;
using std::cin, std::cout, std::getline, std::unordered_map, std::pair;
using std::exception, std::out_of_range, std::stoi, std::to_string, std::format;
using std::regex, std::regex_replace, std::unique_ptr, std::make_unique;
using std::vector, std::string, std::string_view, std::isspace;

Interface::CommandTable Interface::commands = {
    {             "help",             &Interface::help},
    {             "exit",             &Interface::exit},
    {     "client-start",      &Interface::clientStart},
    {      "client-stop",       &Interface::clientStop},
    {   "client-connect",    &Interface::clientConnect},
    {"client-disconnect", &Interface::clientDisconnect},
    {   "client-request",    &Interface::clientRequest},
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

void Interface::help([[maybe_unused]] const ArgumentsType &arguments)
{
    static string messages[] = {"help: Display this message",
                                "exit: Exit the program",
                                "client-start: Start the client",
                                "client-stop: Stop the client",
                                "client-connect server=<ip> port=<number> domain=ipv4|ipv6: Connect to the server",
                                "client-disconnect server=<ip> port=<number>: Disconnect from the server",
                                "client-request server=<ip> port=<number> remote=<path> local=<path>: Request a file from the server",
                                "server-start port=<number> domain=ipv4|ipv6: Start the server",
                                "server-stop: Stop the server"};

    for (const string &message : messages)
        cout << message << '\n';
}

void Interface::exit([[maybe_unused]] const ArgumentsType &arguments) { mRunning = false; }

void Interface::clientStart([[maybe_unused]] const ArgumentsType &arguments)
{
    try
    {
        if (mClient)
            throw ConnectionError("Client already started");

        mClient = make_unique<Client>();
        mClient->onTaskCompleted(bind(&Interface::onTaskCompleted, this, _1, _2, _3));

        Interface::log("client", "start", {});
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

void Interface::clientStop([[maybe_unused]] const ArgumentsType &arguments)
{
    try
    {
        mClient.reset();

        Interface::log("client", "stop", {});
    }
    catch (const exception &e)
    {
        ArgumentsType message = {
            {"success",  "false"},
            { "reason", e.what()}
        };
        Interface::log("client", "stop", message);
    }
}

void Interface::clientConnect([[maybe_unused]] const ArgumentsType &arguments)
{
    Client::Host host;
    try
    {
        if (!mClient)
            throw ConnectionError("Client not started");

        static unordered_map<string, Socket::Domain> domains = {
            {"ipv4", Socket::Domain::IPv4},
            {"ipv6", Socket::Domain::IPv6}
        };

        host.server = arguments.at("server");
        host.port   = stoi(arguments.at("port"));
        host.domain = domains.at(arguments.at("domain"));

        mClient->connect(host);

        ArgumentsType message = {
            {"success",               "true"},
            { "server",          host.server},
            {   "port", to_string(host.port)}
        };
        Interface::log("client", "connect", message);
    }
    catch (const out_of_range &e)
    {
        ArgumentsType message = {
            {"success",              "false"},
            { "reason", "missing parameters"}
        };
        Interface::log("client", "connect", message);
    }
    catch (const exception &e)
    {
        ArgumentsType message = {
            {"success",              "false"},
            { "server",          host.server},
            {   "port", to_string(host.port)},
            { "reason",             e.what()}
        };
        Interface::log("client", "connect", message);
    }
}

void Interface::clientDisconnect([[maybe_unused]] const ArgumentsType &arguments)
{
    Client::Host host;

    try
    {
        if (!mClient)
            throw ConnectionError("Client not started");

        host.server = arguments.at("server");
        host.port   = stoi(arguments.at("port"));

        mClient->disconnect(host);

        ArgumentsType message = {
            {"success",               "true"},
            { "server",          host.server},
            {   "port", to_string(host.port)}
        };
        Interface::log("client", "disconnect", message);
    }
    catch (const out_of_range &e)
    {
        ArgumentsType message = {
            {"success",              "false"},
            { "reason", "missing parameters"}
        };
        Interface::log("client", "disconnect", message);
    }
    catch (const exception &e)
    {
        ArgumentsType message = {
            {"success",              "false"},
            { "server",          host.server},
            {   "port", to_string(host.port)},
            { "reason",             e.what()}
        };
        Interface::log("client", "disconnect", message);
    }
}

void Interface::clientRequest([[maybe_unused]] const ArgumentsType &arguments)
{
    Client::Host host;
    Client::Task task;

    try
    {
        if (!mClient)
            throw ConnectionError("Client not started");

        host.server         = arguments.at("server");
        host.port           = stoi(arguments.at("port"));
        task.serverFilePath = arguments.at("remote");
        task.localFilePath  = arguments.at("local");

        mClient->request(host, task);

        ArgumentsType message = {
            {"success",               "true"},
            { "server",          host.server},
            {   "port", to_string(host.port)},
            { "remote",  task.serverFilePath},
            {  "local",   task.localFilePath}
        };
        Interface::log("client", "request", message);
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
            {"success",              "false"},
            { "server",          host.server},
            {   "port", to_string(host.port)},
            { "remote",  task.serverFilePath},
            {  "local",   task.localFilePath},
            { "reason",             e.what()}
        };
        Interface::log("client", "request", message);
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

        mServer = make_unique<Server>(stoi(arguments.at("port")), domains.at(arguments.at("domain")));
        mServer->onConnect(bind(&Interface::onConnect, this, _1));
        mServer->onDisconnect(bind(&Interface::onDisconnect, this, _1));
        mServer->onRequest(bind(&Interface::onRequest, this, _1, _2));
        mServer->onResponse(bind(&Interface::onResponse, this, _1, _2));
        mServer->onClientError(bind(&Interface::onClientError, this, _1, _2));
        mServer->onServerError(bind(&Interface::onServerError, this, _1));

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

void Interface::onTaskCompleted(const Client::Host &host, const Client::Task &task, const string &result)
{
    ArgumentsType message = {
        {"server",          host.server},
        {  "port", to_string(host.port)},
        {"remote",  task.serverFilePath},
        { "local",   task.localFilePath},
        {"result",               result}
    };
    Interface::log("client", "task", message);
}

void Interface::onConnect(const Socket &client)
{
    ArgumentsType message = {
        {"client",              client.getIP()},
        {  "port", to_string(client.getPort())},
    };
    Interface::log("server", "connect", message);
}

void Interface::onDisconnect(const Socket &client)
{
    ArgumentsType message = {
        {"client",              client.getIP()},
        {  "port", to_string(client.getPort())},
    };
    Interface::log("server", "disconnect", message);
}

void Interface::onRequest(const Socket &client, const string &message)
{
    ArgumentsType arguments = {
        {"client",              client.getIP()},
        {  "port", to_string(client.getPort())},
        {  "path",                     message}
    };
    Interface::log("server", "request", arguments);
}

void Interface::onResponse(const Socket &client, const string &message)
{
    ArgumentsType arguments = {
        { "client",              client.getIP()},
        {   "port", to_string(client.getPort())},
        {"message",                     message}
    };
    Interface::log("server", "response", arguments);
}

void Interface::onClientError(const Socket &client, const string &message)
{
    ArgumentsType arguments = {
        {"client",              client.getIP()},
        {  "port", to_string(client.getPort())},
        {"reason",                     message}
    };
    Interface::log("server", "error", arguments);
}

void Interface::onServerError(const string &error)
{
    ArgumentsType arguments = {
        {"reason", error}
    };
    Interface::log("server", "error", arguments);
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