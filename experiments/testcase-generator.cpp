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
        executable.stdin << stdinLine << endl;
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
                return numSecondsRunFor >= m_value;
            }
        }
        string progressString() const
        {
            if (m_type == NumIterations)
            {
                return "Generated " + to_string(m_numTestcasesGenerated) + " of " + to_string(m_value) + " testcases";
            }
            else
            {
                std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                const auto numSecondsRunFor = std::chrono::duration_cast<std::chrono::seconds>(now - m_testcaseGenerationBeginTime).count();
                return "Run for " + to_string(numSecondsRunFor) + " of " + to_string(m_value) + " seconds (" + to_string(m_value - numSecondsRunFor) + " seconds remaining)";
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

struct TestCase
{
    vector<string> testInput;
    vector<string> testRunOutput;
};

class TestCaseReader
{
    public:
        TestCaseReader(istream& testSuiteStream)
            : m_testSuiteStream(testSuiteStream)
        {
        }
        bool hasNext()
        {
            if (m_loadedNextTestCase)
                return true;
            if (attemptLoadNext(m_nextTestCase))
            {
                m_loadedNextTestCase = true;
                return true;
            }
            return false;
        }
        TestCase next()
        {
            assert(hasNext());
            if (m_loadedNextTestCase)
            {
                m_loadedNextTestCase = false;
                return m_nextTestCase;
            }

            // TODO - throw exception on fail? Or just rely on the assert, above?
            attemptLoadNext(m_nextTestCase);
            assert(!m_loadedNextTestCase);
            return m_nextTestCase;
        }
        bool hasError() const
        {
            return m_hasError;
        }
        string errorMessage() const
        {
            return m_errorMessage;
        }
    private:
        istream& m_testSuiteStream;
        int m_numLinesOfInputRead = 0;

        bool m_loadedNextTestCase = false;
        TestCase m_nextTestCase;
        bool m_hasError = false;
        string m_errorMessage;

        bool readNextLine(string& destString)
        {
            const bool successful = static_cast<bool>(std::getline(m_testSuiteStream, destString));
            if (successful)
            {
                m_numLinesOfInputRead++;
            }
            return successful;
        }
        
        bool attemptLoadNext(TestCase& destTestCase)
        {
            if (m_hasError)
                return false;
            destTestCase.testInput.clear();
            destTestCase.testRunOutput.clear();
            string testInputHeader;
            if (!readNextLine(testInputHeader))
            {
                if (!m_testSuiteStream.eof())
                {
                    m_hasError = true;
                    m_errorMessage = "Unknown read error (not eof) when attempting to load next test input header at line: " + to_string(m_numLinesOfInputRead);
                }
                return false;
            }

            std::smatch inputHeaderMatch;
            if (!std::regex_search(testInputHeader, inputHeaderMatch, testInputHeaderRegex))
            {
                m_hasError = true;
                m_errorMessage = "The line " + testInputHeader + " does not match the format of a test input header at line: " + to_string(m_numLinesOfInputRead);
                return false;
            }

            const int numTestInputLines = stoi(inputHeaderMatch[1]);
            for (int i = 0; i < numTestInputLines; i++)
            {
                string testInputLine;
                if (!readNextLine(testInputLine))
                {
                    m_hasError = true;
                    m_errorMessage = "Could not load line " + to_string(i + 1) + "/" + to_string(numTestInputLines) + " of the test input at line: " + to_string(m_numLinesOfInputRead + 1);
                    return false;
                }
                destTestCase.testInput.push_back(testInputLine);
            }

            string testResultHeader;
            if (!readNextLine(testResultHeader))
            {
                m_hasError = true;
                m_errorMessage = "Unknown read error when attempting to load next test result header at line: " + to_string(m_numLinesOfInputRead);
                return false;
            }

            std::smatch resultHeaderMatch;
            if (!std::regex_search(testResultHeader, resultHeaderMatch, testResultHeaderRegex))
            {
                m_hasError = true;
                m_errorMessage = "The line " + testResultHeader + " does not match the format of a test result header at line: " + to_string(m_numLinesOfInputRead);
                return false;
            }

            const int numTestResultLines = stoi(resultHeaderMatch[1]);
            for (int i = 0; i < numTestResultLines; i++)
            {
                string testResultLine;
                if (!readNextLine(testResultLine))
                {
                    m_hasError = true;
                    m_errorMessage = "Could not load line " + to_string(i + 1) + "/" + to_string(numTestResultLines) + " of the test result at line: " + to_string(m_numLinesOfInputRead + 1);
                    return false;
                }
                destTestCase.testRunOutput.push_back(testResultLine);
            }
            return m_testSuiteStream.good();
        }

        static std::regex testInputHeaderRegex;
        static std::regex testResultHeaderRegex;
};

std::regex TestCaseReader::testInputHeaderRegex("^Q:\\s*(\\d+) lines");
std::regex TestCaseReader::testResultHeaderRegex("^A:\\s*(\\d+) lines");

ExecutionResult runTestWithInputAndGetFilteredResult(const string& executableName, const vector<string>& testInput, const std::regex& testResultRegexFilter, int testResultRegexFilterCaptureGroup)
{
    ExecutionResult testRunResult = execute(executableName, {}, testInput);
    const auto originalTestRunOutput = testRunResult.output;
    testRunResult.output.clear();
    for (const auto& testResultLine : originalTestRunOutput)
    {
        std::smatch match;
        cout << "line: " << testResultLine << endl;
        if (std::regex_search(testResultLine, match, testResultRegexFilter))
        {
            cout << " matches regex filter" << endl;
            assert(testResultRegexFilterCaptureGroup < match.size());
            testRunResult.output.push_back(match[testResultRegexFilterCaptureGroup]);
        }
        else
        {
            cout << " does not match regex filter; ignoring" << endl;
        }
    }

    if (testRunResult.output.empty())
    {
        cerr << "No output from executable - this is an error!" << endl;
        if (!originalTestRunOutput.empty())
        {
            cerr << "NB: the original unfiltered output was not empty, so this may be a problem with your regex." << endl;
        }
        testRunResult.status = -1;
    }

    return testRunResult;
}

void writeFailingTestInputAndDie(const vector<string>& failingTestInput, const string& failedTestcaseFilename)
{
    cerr << "Executable returned unsuccessful upon an input testcase - the testcase has been written to " << failedTestcaseFilename << endl;
    ofstream failedTestcaseFile(failedTestcaseFilename);

    for (const auto& x : failingTestInput)
    {
        failedTestcaseFile << x << endl;
    }

    failedTestcaseFile.close();

    exit(EXIT_FAILURE);
}

bool validateTestSuite(const string& testSuiteFileName)
{
    ifstream testSuiteFile(testSuiteFileName);
    TestCaseReader testCaseReader(testSuiteFile);
    int numTestcases = 0;
    while (testCaseReader.hasNext())
    {
        testCaseReader.next();
        numTestcases++;
    }
    if (testCaseReader.hasError())
    {
        cerr << "Validation error: " << testCaseReader.errorMessage() << endl;
        return false;
    }
    cout << "Validation successful - " << numTestcases << " testcases validated as OK!" << endl;
    return true;
}

int main(int argc, char* argv[])
{
    enum Mode {Generate, Verify, Validate, AppendFromStdin};
    Mode mode = Generate;
    bool appendToTestSuiteFile = false;
    string failedTestcaseFilename = "failed_test_case.txt";
    string executableName = "./a.out";
    string testResultRegexFilterPattern = "^.*$"; // Match everything by default.
    int testResultRegexFilterCaptureGroup = 0;
    regex testResultRegexFilter;

    bool verifyModeFlag = false;
    bool validateModeFlag = false;
    bool appendFromStdinFlag = false;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("testsuite-filename,f", po::value< string >()->required(), "testsuite filename")
        ("executable-name,e", po::value< string >(&executableName), "path to the executable that generates/ runs tests.  Defaults to ./a.out")
        ("verify", po::bool_switch(&verifyModeFlag), "verify the executable against the given testsuite inputs and outputs, instead of generating new test cases")
        ("validate", po::bool_switch(&validateModeFlag), "perform simple validation of the testsuite to check for obvious corruption")
        ("append-testcase-from-stdin", po::bool_switch(&appendFromStdinFlag), "read a testcase input from stdin and append it and its calculcated ouptput to the testsuite")
        ("stop-after,s", po::value< string >(), "when to stop - either a number of testcases, or <X>s to stop after X seconds")
        ("append", po::bool_switch(&appendToTestSuiteFile), "append to the testsuite file file instead of overwriting it")
        ("failing-testcase-filename", po::value<string>(&failedTestcaseFilename), "filename to output failed test inputs to")
        ("testcase-gen-regex-filter,r", po::value<string>(&testResultRegexFilterPattern), "when generating expected testcase output, pass all resulting output through this regex")
        ("testcase-gen-regex-filter-capture-group,g", po::value<int>(&testResultRegexFilterCaptureGroup), "Used in conjunction with --testcase-gen-regex-filter - each line of output is passed through the regex, then replaced with the contents of the given capture group")
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

    if (verifyModeFlag)
    {
        mode = Verify;
    }
    if (validateModeFlag)
    {
        mode = Validate;
    }
    if (appendFromStdinFlag)
    {
        mode = AppendFromStdin;
    }


    cout << "testResultRegexFilterPattern: " << testResultRegexFilterPattern << endl;
    testResultRegexFilter.assign(testResultRegexFilterPattern);

    const string testSuiteFileName = vm["testsuite-filename"].as<string>();

    if (mode == Generate)
    {
        if (!vm.count("stop-after"))
        {
            cerr << "the option --stop-after is required but missing" << endl;
            return EXIT_FAILURE;
        }
        string stopAfterString = vm["stop-after"].as<string>();
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
            const ExecutionResult testGenerationResult = execute(executableName, {"--test"}, {});
            if (!testGenerationResult.successful())
            {
                cerr << "Error while generating testcase - aborting" << endl;
                return EXIT_FAILURE;
            }
            const vector<string> generatedTest = testGenerationResult.output;
            cout << "generatedTest size: " << generatedTest.size() << endl;

            const ExecutionResult testRunResult = runTestWithInputAndGetFilteredResult(executableName, generatedTest, testResultRegexFilter, testResultRegexFilterCaptureGroup);
            if (!testRunResult.successful())
            {
                writeFailingTestInputAndDie(generatedTest, failedTestcaseFilename);
            }
            const vector<string> testRunOutput = testRunResult.output;

            testSuiteFile << "Q: " << generatedTest.size() << " lines" << endl;
            for (const auto& x : generatedTest)
            {
                testSuiteFile << x << endl;
            }
            testSuiteFile << "A: " << testRunOutput.size() << " lines" << endl;
            for (const auto& x : testRunOutput)
            {
                testSuiteFile << x << endl;
            }
            stopAfter.notifyTestcaseGenerated();

            cout << stopAfter.progressString() << endl;
        }
        testSuiteFile.close();
    }
    else if (mode == Verify)
    {
        ifstream testSuiteFile(testSuiteFileName);
        TestCaseReader testCaseReader(testSuiteFile);
        int testCaseNum = 0;
        while (testCaseReader.hasNext())
        {
            const auto testCase = testCaseReader.next();
            testCaseNum++;
            cout << "Read test case # " << testCaseNum << " input # lines: " << testCase.testInput.size() << " output # lines: " << testCase.testRunOutput.size() << endl; 
            const ExecutionResult testRunResult = runTestWithInputAndGetFilteredResult(executableName, testCase.testInput, testResultRegexFilter, testResultRegexFilterCaptureGroup);
            if (!testRunResult.successful())
            {
                writeFailingTestInputAndDie(testCase.testInput, failedTestcaseFilename);
            }
            if (testRunResult.output != testCase.testRunOutput)
            {
                writeFailingTestInputAndDie(testCase.testInput, failedTestcaseFilename);
            }
        }
        cout << "All testcases passed!" << endl;
        // All clear!
        return EXIT_SUCCESS;
    }
    else if (mode == Validate)
    {
        return validateTestSuite(testSuiteFileName) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else if (mode == AppendFromStdin)
    {
        vector<string> readTestInput;
        string testInputLine;
        while (std::getline(cin, testInputLine))
        {
            readTestInput.push_back(testInputLine);
        }

        const ExecutionResult testRunResult = runTestWithInputAndGetFilteredResult(executableName, readTestInput, testResultRegexFilter, testResultRegexFilterCaptureGroup);
        if (!testRunResult.successful())
        {
            writeFailingTestInputAndDie(readTestInput, failedTestcaseFilename);
        }
        const vector<string> testRunOutput = testRunResult.output;

        ofstream testSuiteFile(testSuiteFileName, ios_base::app);
        testSuiteFile << "Q: " << readTestInput.size() << " lines" << endl;
        for (const auto& x : readTestInput)
        {
            testSuiteFile << x << endl;
        }
        testSuiteFile << "A: " << testRunOutput.size() << " lines" << endl;
        for (const auto& x : testRunOutput)
        {
            testSuiteFile << x << endl;
        }
        const bool writtenOk = testSuiteFile.good();

        testSuiteFile.close();
        if (!writtenOk)
        {
            cerr << "Error writing to file: " << testSuiteFileName << endl;
        }
        return writtenOk ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else
    {
        assert(false && "Unhandled mode");
    }
    return 0;
}
