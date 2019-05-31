// The cpipe and spawn classes are taken from  https://gist.github.com/konstantint/d49ab683b978b3d74172
// (credit to Konstantin Tretyakov!).
#include <iostream>
#include <ext/stdio_filebuf.h> // NB: Specific to libstdc++
#include <sys/wait.h>
#include <unistd.h>
#include <cstddef>
#include <iostream>
#include <vector>
#include <memory>
#include <exception>
#include <chrono>
#include <regex>

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
    bool successful() const
    {
        return (status == 0);
    }
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

class StopAfter
{
    public:
        enum Type { NumIterations, ElapsedSeconds };
        void setType(Type type)
        {
            m_type = type;
        }
        void setValue(int value)
        {
            m_value = value;
            cout << "StopAfter::setValue: " << value << endl;
        }
        bool shouldStop() const
        {
            if (m_type == NumIterations)
            {
                return m_numTestcasesGenerated >= m_value;
            }
            else
            {
                std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                const auto numSecondsRunFor = std::chrono::duration_cast<std::chrono::seconds>(now - m_testcaseGenerationBeginTime).count();
                cout << "numSecondsRunFor: " << numSecondsRunFor << endl;
                return numSecondsRunFor >= m_value;
            }
        }
        void notifyGenerationStarted()
        {
            m_testcaseGenerationBeginTime = std::chrono::steady_clock::now();
        }
        void notifyTestcaseGenerated()
        {
            m_numTestcasesGenerated++;
        }
    private:
        Type m_type = NumIterations;
        int m_value = -1;

        int m_numTestcasesGenerated = 0;
        std::chrono::steady_clock::time_point m_testcaseGenerationBeginTime;

};

int main(int argc, char* argv[])
{
    bool appendToTestSuiteFile = false;
    string failedTestcaseFilename = "failed_test_case.txt";
    string testResultRegexFilterPattern;
    int testResultRegexFilterCaptureGroup = 0;
    regex testResultRegexFilter;

    bool verifyMode = false;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("testsuite-filename", po::value< string >()->required(), "testsuite filename")
        ("verify", po::bool_switch(&verifyMode), "verify the executable against the given testsuite inputs and outputs, instead of generating new test cases")
        ("stop-after", po::value< string >()->required(), "when to stop - either a number of testcases, or <X>s to stop after X seconds")
        ("append", po::bool_switch(&appendToTestSuiteFile), "append to the testsuite file file instead of overwriting it")
        ("failing-testcase-filename", po::value<string>(&failedTestcaseFilename), "filename to output failed test inputs to")
        ("testcase-gen-regex-filter", po::value<string>(&testResultRegexFilterPattern), "when generating expected testcase output, pass all resulting output through this regex")
        ("testcase-gen-regex-filter-capture-group", po::value<int>(&testResultRegexFilterCaptureGroup), "Used in conjunction with --testcase-gen-regex-filter - each line of output is passed through the regex, then replaced with the contents of the given capture group")
        ;
    po::positional_options_description p;
    p.add("testsuite-filename", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
    if (vm.count("help"))
    {
        std::cout << desc << endl;
        return 0;
    }
    po::notify(vm);
    cout << "testResultRegexFilterPattern: " << testResultRegexFilterPattern << endl;
    if (!testResultRegexFilterPattern.empty())
    {
        testResultRegexFilter.assign(testResultRegexFilterPattern);
    }

    const string testSuiteFileName = vm["testsuite-filename"].as<string>();
    string stopAfterString = vm["stop-after"].as<string>();

    if (!verifyMode)
    {
        // TODO - this still allows e.g. --stop-after=3dinosaur!garbageXXX(**&*
        StopAfter stopAfter;
        if (stopAfterString.back() == 's')
        {
            stopAfterString.pop_back();
            stopAfter.setType(StopAfter::ElapsedSeconds);
        }
        try
        {
            stopAfter.setValue(stoi(stopAfterString));
        }
        catch (std::exception&)
        {
            cerr << "Expected: either integer, or integer followed by 's' for --stop-after  - got \"" << stopAfterString << "\" instead" << endl;
            return EXIT_FAILURE;
        }

        ofstream testSuiteFile(testSuiteFileName, appendToTestSuiteFile ? ios_base::app : ios_base::out);

        stopAfter.notifyGenerationStarted();
        while (!stopAfter.shouldStop())
        {
            const ExecutionResult testGenerationResult = execute("./a.out", {"--test"}, {});
            if (!testGenerationResult.successful())
            {
                cerr << "Error while generating testcase - aborting" << endl;
                return EXIT_FAILURE;
            }
            const vector<string> generatedTest = testGenerationResult.output;
            cout << "generatedTest size: " << generatedTest.size() << endl;

            const ExecutionResult testRunResult = execute("./a.out", {}, generatedTest);
            if (!testRunResult.successful())
            {
                cerr << "Executable returned unsuccessful upon an input testcase - the testcase has been written to " << failedTestcaseFilename << endl;
                ofstream failedTestcaseFile(failedTestcaseFilename);

                for (const auto& x : generatedTest)
                {
                    failedTestcaseFile << x << endl;
                }

                failedTestcaseFile.close();

                return EXIT_FAILURE;

            }
            vector<string> testRunOutput;
            if (!testResultRegexFilterPattern.empty())
            {
                for (const auto& testResultLine : testRunResult.output)
                {
                    std::smatch match;
                    cout << "line: " << testResultLine << endl;
                    if (std::regex_search(testResultLine, match, testResultRegexFilter))
                    {
                        cout << " matches" << endl;
                        assert(testResultRegexFilterCaptureGroup < match.size());
                        testRunOutput.push_back(match[testResultRegexFilterCaptureGroup]);
                    }
                    else
                    {
                        cout << " does not match" << endl;
                    }
                }
            }
            else
            {
                testRunOutput = testRunResult.output;
            }


            testSuiteFile << "Q: " << endl;
            for (const auto& x : generatedTest)
            {
                testSuiteFile << x << endl;
            }
            testSuiteFile << "A: " << endl;
            for (const auto& x : testRunOutput)
            {
                testSuiteFile << x << endl;
            }
            stopAfter.notifyTestcaseGenerated();
        }
        testSuiteFile.close();
    }
    return 0;
}
