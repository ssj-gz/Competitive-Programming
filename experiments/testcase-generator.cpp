// The cpipe and spawn classes are taken from  https://gist.github.com/konstantint/d49ab683b978b3d74172
// (credit to Konstantin Tretyakov!).
#include <iostream>
#include <ext/stdio_filebuf.h> // NB: Specific to libstdc++
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <memory>
#include <exception>

// Wrapping pipe in a class makes sure they are closed when we leave scope
class cpipe {
    private:
        int fd[2];
    public:
        const inline int read_fd() const { return fd[0]; }
        const inline int write_fd() const { return fd[1]; }
        cpipe() { if (pipe(fd)) throw std::runtime_error("Failed to create pipe"); }
        void close() { ::close(fd[0]); ::close(fd[1]); }
        ~cpipe() { close(); }
};


//
// Usage:
//   spawn s(argv)
//   s.stdin << ...
//   s.stdout >> ...
//   s.send_eol()
//   s.wait()
//
class spawn {
    private:
        cpipe write_pipe;
        cpipe read_pipe;
    public:
        int child_pid = -1;
        std::unique_ptr<__gnu_cxx::stdio_filebuf<char> > write_buf = NULL; 
        std::unique_ptr<__gnu_cxx::stdio_filebuf<char> > read_buf = NULL;
        std::ostream stdin;
        std::istream stdout;

        spawn(const char* const argv[], bool with_path = false, const char* const envp[] = 0): stdin(NULL), stdout(NULL) {
            child_pid = fork();
            if (child_pid == -1) throw std::runtime_error("Failed to start child process"); 
            if (child_pid == 0) {   // In child process
                dup2(write_pipe.read_fd(), STDIN_FILENO);
                dup2(read_pipe.write_fd(), STDOUT_FILENO);
                write_pipe.close(); read_pipe.close();
                int result;
                if (with_path) {
                    if (envp != 0) result = execvpe(argv[0], const_cast<char* const*>(argv), const_cast<char* const*>(envp));
                    else result = execvp(argv[0], const_cast<char* const*>(argv));
                }
                else {
                    if (envp != 0) result = execve(argv[0], const_cast<char* const*>(argv), const_cast<char* const*>(envp));
                    else result = execv(argv[0], const_cast<char* const*>(argv));
                }
                if (result == -1) {
                    // Note: no point writing to stdout here, it has been redirected
                    std::cerr << "Error: Failed to launch program" << std::endl;
                    exit(1);
                }
            }
            else {
                close(write_pipe.read_fd());
                close(read_pipe.write_fd());
                write_buf = std::unique_ptr<__gnu_cxx::stdio_filebuf<char> >(new __gnu_cxx::stdio_filebuf<char>(write_pipe.write_fd(), std::ios::out));
                read_buf = std::unique_ptr<__gnu_cxx::stdio_filebuf<char> >(new __gnu_cxx::stdio_filebuf<char>(read_pipe.read_fd(), std::ios::in));
                stdin.rdbuf(write_buf.get());
                stdout.rdbuf(read_buf.get());
            }
        }

        void send_eof() { write_buf->close(); }

        int wait() {
            int status;
            waitpid(child_pid, &status, 0);
            return status;
        }
};

#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace std;

struct ExecutionResult
{
    int status = -1;
    vector<string> output;
};

ExecutionResult execute(const string& exeName, const vector<string>& exeArgs = {}, const vector<string>& stdinInput = {})
{
    ExecutionResult result;

    vector<const char*> exeNameAndArgs;
    exeNameAndArgs.push_back(exeName.c_str());
    for (const auto& arg : exeArgs)
    {
        exeNameAndArgs.push_back(arg.c_str());
    }
    exeNameAndArgs.push_back(static_cast<const char*>(0));
    spawn executable(exeNameAndArgs.data());
    for (const auto& stdinLine : stdinInput)
    {
        executable.stdin << stdinLine;
    }
    executable.send_eof();
    string outputLine;
    while (getline(executable.stdout, outputLine))
    {
        result.output.push_back(outputLine);
    }
    cout << "Waiting for executable to terminate" << endl;
    result.status = executable.wait();
    
    cout << "Executable status: " << result.status << endl;

    return result;
}


int main(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
       ("output-file", po::value< vector<string> >()->required(), "output file")
    ;
    po::positional_options_description p;
    p.add("output-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
                      options(desc).positional(p).run(), vm);
    if (vm.count("help"))
    {
        std::cout << desc << endl;
        return 0;
    }
    po::notify(vm);


    vector<string> generatedTest = execute("./a.out", {"--test"}, {}).output;
    cout << "generatedTest size: " << generatedTest.size() << endl;
    vector<string> result = execute("./a.out", {}, generatedTest).output;
    cout << "Q: " << endl;
    for (const auto& x : generatedTest)
    {
        cout << x << endl;
    }
    cout << "A: " << endl;
    for (const auto& x : result)
    {
        cout << x << endl;
    }
    return 0;
}
