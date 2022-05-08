#include <iostream>
#include <vector>
#include <deque>
#include <iterator>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

class ProgramExecutor
{
    public:
        ProgramExecutor(const vector<int64_t>& originalProgram)
            : m_program(originalProgram), m_originalProgram(originalProgram)
        {
        }
        enum ExitStatus { Terminated, WaitingForInput};
        ExitStatus run()
        {
            if (m_isTerminated)
                return Terminated;
            while (true)
            {
                const auto instruction = m_program[m_position++];
                const auto opCode = instruction % 100;
                const auto parameterModes = instruction / 100;

                auto nextParamValue = [this, firstParamPosition = m_position, parameterModes]() mutable
                {
                    cout << "nextParamValue! m_position: " << m_position << endl;
                    const auto paramIndex = m_position - firstParamPosition;
                    cout << "paramIndex: " << paramIndex << endl;
                    auto parameterMode = parameterModes;
                    for (auto i = 0; i < paramIndex; i++)
                        parameterMode /= 10;
                    parameterMode %= 10;

                    assert(parameterMode == 0 || parameterMode == 1);
                    cout << "parameterMode: " << parameterMode << endl;

                    if (parameterMode == 0)
                    {
                        const auto returnVal = m_program[m_program[m_position++]];
                        cout << "Returning " << returnVal << endl;
                        return returnVal;
                    }
                    else
                    {
                        const auto returnVal = m_program[m_position++];
                        cout << "Returning " << returnVal << endl;
                        return returnVal;
                    }

                };

                switch(opCode)
                {
                    case 1:
                        {
                            cout << "Sum!" << endl;
                            const auto sumand1 = nextParamValue();
                            const auto sumand2 = nextParamValue();
                            const auto destPos = m_program[m_position++];
                            cout << " sumand1: " << sumand1 << " sumand2: " << sumand2 << " destPos: " << destPos << endl;
                            m_program[destPos] = sumand1 + sumand2;
                            break;
                        }
                    case 2:
                        {
                            cout << "Multiplication!" << endl;
                            const auto multiplicand1 = nextParamValue();
                            const auto multiplicand2 = nextParamValue();
                            const auto destPos = m_program[m_position++];
                            cout << " multiplicand1: " << multiplicand1 << " multiplicand2: " << multiplicand2 << " destPos: " << destPos << endl;
                            m_program[destPos] = multiplicand1 * multiplicand2;
                            break;
                        }
                    case 3:
                        {
                            cout << "Input!" << endl;
                            if (m_inputs.empty())
                            {
                                cout << " no input: exiting with WaitingForInput" << endl;
                                m_position--;
                                return WaitingForInput;
                            }
                            const auto input = m_inputs.front();
                            m_inputs.pop_front();
                            const auto destPos = m_program[m_position++];
                            cout << " input: " << input << " destPos: " << destPos << endl;
                            m_program[destPos] = input;
                            break;
                        }
                    case 4:
                        {
                            cout << "Output!" << endl;
                            const auto output = nextParamValue();
                            cout << " outputting: " << output << endl;
                            m_output.push_back(output);
                            break;
                        }
                    case 5:
                        {
                            cout << "Jump-if-true!" << endl;
                            const auto toCheck = nextParamValue();
                            const auto positionIfNonZero = nextParamValue();
                            cout << " toCheck: " << toCheck << " positionIfNonZero: " << positionIfNonZero << endl;
                            if (toCheck != 0)
                                m_position = positionIfNonZero;
                            break;
                        }
                    case 6:
                        {
                            cout << "Jump-if-false!" << endl;
                            const auto toCheck = nextParamValue();
                            const auto positionIfZero = nextParamValue();
                            cout << " toCheck: " << toCheck << " positionIfZero: " << positionIfZero << endl;
                            if (toCheck == 0)
                                m_position = positionIfZero;
                            break;
                        }
                    case 7:
                        {
                            cout << "less-than!" << endl;
                            const auto param1 = nextParamValue();
                            const auto param2 = nextParamValue();
                            const auto destPos = m_program[m_position++];
                            cout << " param1: " << param1 << " param2: " << param2 << " destPos: " << destPos << endl;
                            if (param1  < param2)
                                m_program[destPos] = 1;
                            else
                                m_program[destPos] = 0;
                            break;
                        }
                    case 8:
                        {
                            cout << "equals!" << endl;
                            const auto param1 = nextParamValue();
                            const auto param2 = nextParamValue();
                            const auto destPos = m_program[m_position++];
                            cout << " param1: " << param1 << " param2: " << param2 << " destPos: " << destPos << endl;
                            if (param1 == param2)
                                m_program[destPos] = 1;
                            else
                                m_program[destPos] = 0;
                            break;
                        }
                    case 99:
                        cout << "Terminate!" << endl;
                        m_isTerminated = true;
                        return Terminated;
                    default:
                        cout << "Unknown opCode: " << opCode << endl;
                        assert(false);
                }
            }
            assert(false && "Unreachable");
            return Terminated;
        }
        void addInputs(const vector<int64_t>& input)
        {
            m_inputs.insert(m_inputs.end(), input.begin(), input.end());
        }
        vector<int64_t> output() const
        {
            return m_output;
        }
        void clearOutput()
        {
            m_output.clear();
        }
    private:
        bool m_isTerminated = false;
        vector<int64_t> m_program;
        vector<int64_t> m_originalProgram;
        int64_t m_position = 0;
        deque<int64_t> m_inputs;
        vector<int64_t> m_output;
};

int main()
{
    vector<int64_t> program;
    int64_t input;
    while (true)
    {
        cin >> input;
        program.push_back(input);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    vector<int64_t> phaseSettingPermutation = {5,6,7,8,9};
    const auto originalPhaseSettingPermutation = phaseSettingPermutation;

    constexpr int numAmplifiers = 5;

    int64_t largestThrust = std::numeric_limits<int64_t>::min();
    do
    {
        cout << "Trying permutation" << endl;
        for (const auto x : phaseSettingPermutation)
            cout << x << " ";
        // Run all amplifiers to completion with current phaseSettingPermutation.
        ProgramExecutor programExecutors[] = { ProgramExecutor(program), ProgramExecutor(program), ProgramExecutor(program), ProgramExecutor(program), ProgramExecutor(program) };
        assert(phaseSettingPermutation.size() == std::size(programExecutors));
        for (int i = 0; i < numAmplifiers; i++)
        {
            vector<int64_t> phaseSettingInput;
            phaseSettingInput.push_back(phaseSettingPermutation[i]);
            programExecutors[i].addInputs(phaseSettingInput);
        }
        vector<int64_t> initialInput;
        initialInput.push_back(0);
        programExecutors[0].addInputs(initialInput);

        int64_t thrust = -1;
        int iterationNum = 1;
        bool allTerminated = true;
        do
        {
            allTerminated = true;
            cout << "iterationNum: " << iterationNum << endl;
            for (int i = 0; i < numAmplifiers; i++)
            {
                cout << "Executing amplifier #" << i << endl;
                const ProgramExecutor::ExitStatus status = programExecutors[i].run();
                if (status != ProgramExecutor::Terminated)
                    allTerminated = false;
                cout << " status: " << (status == ProgramExecutor::Terminated ? "Terminated" : "WaitingForInput") << endl;
                const auto output = programExecutors[i].output();
                if (i == numAmplifiers - 1 && output.size() == 1)
                {
                    thrust = output.front();
                }
                programExecutors[i].clearOutput();
                programExecutors[(i + 1) % numAmplifiers].addInputs(output);
            }
            iterationNum++;

        } while (!allTerminated);

        if (thrust > largestThrust)
        {
            largestThrust = max(largestThrust, thrust);
            cout << "New largest thrust: " << largestThrust << endl;
            cout << "From phaseSettingPermutation: " << endl;
            for (const auto x : phaseSettingPermutation)
                cout << x << " ";
            cout << endl;
        }

        std::next_permutation(phaseSettingPermutation.begin(), phaseSettingPermutation.end());
    } while (phaseSettingPermutation != originalPhaseSettingPermutation);

    cout << "largestThrust: " << largestThrust << endl;
}
