#include <cstring>
#include <functional>
#include <map>
#include <string>
#include <vector>
// WARNING: NO more headers allowed!

using std::function;
using std::map;
using std::pair;
using std::string;
using std::vector;
namespace final
{
    class arguments
    {
    private:
        // WARNING: You cannot add more member variables.
        int    _argc;
        char **_argv;

    public:
        arguments() : _argc(0), _argv(nullptr) {}
        explicit arguments(const string &cmd)
        {
            // todo implement constructor
            vector<int> argvPos;
            argvPos.push_back(0);
            for (int i = 1; cmd[i] != '\0'; i++)
                if (cmd[i] == ' ' && cmd[i + 1] != ' ' && cmd[i + 1] != '\0')
                    argvPos.push_back(i + 1);
            _argc = argvPos.size();
            _argv = new char *[_argc];
            for (int i = _argc - 1; i >= 0; i--)
            {
                int pos = argvPos[i], length = 0;
                for (int j = pos; cmd[j] != ' ' && cmd[j] != '\0'; j++)
                    length++;
                _argv[i] = new char[length + 1];
                for (int j = pos; cmd[j] != ' ' && cmd[j] != '\0'; j++)
                    _argv[i][j - pos] = cmd[j];
                _argv[i][length] = '\0';
            }
        }
        ~arguments()
        {
            // todo implement destructor
            for (int i = 0; i < _argc; i++)
                delete[] _argv[i];
            delete[] _argv;
        }
        // WARNING: You cannot modify the following functions
        int    argc() const { return _argc; }
        char **argv() const { return _argv; }
    };

    // You don't need to modify shell.
    class shell
    {
    private:
        map<int, arguments> running_list;

    public:
        shell() = default;

        void run(int pid, const string &cmd, const function<void(int, char **)> &invoked)
        {
            running_list.emplace(pid, cmd);
            invoked(running_list[pid].argc(), running_list[pid].argv());
        }

        int subprocessExit(int pid, int return_value)
        {
            running_list.erase(pid);
            return return_value;
        }
        vector<int> getRunningList() const
        {
            vector<int> rt;
            for (auto &pair : running_list)
                rt.push_back(pair.first);
            return rt;
        }
    };
} // namespace final