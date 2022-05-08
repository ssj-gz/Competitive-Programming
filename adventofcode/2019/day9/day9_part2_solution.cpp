#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <iterator>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

class ProgramExecutor
{
    public:
        ProgramExecutor(const vector<int64_t>& originalProgram)
        {
            for (int i = 0; i < static_cast<int>(originalProgram.size()); i++)
            {
                m_program[i] = originalProgram[i];
            }
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

                auto nextParamValue = [this, firstParamPosition = m_position, parameterModes]() -> int64_t& 
                {
                    cout << "nextParamValue! m_position: " << m_position << endl;
                    const auto paramIndex = m_position - firstParamPosition;
                    cout << "paramIndex: " << paramIndex << endl;
                    auto parameterMode = parameterModes;
                    for (auto i = 0; i < paramIndex; i++)
                        parameterMode /= 10;
                    parameterMode %= 10;

                    cout << "parameterMode: " << parameterMode << " m_relativeBase: " << m_relativeBase << endl;

                    if (parameterMode == 0)
                    {
                        return m_program[m_program[m_position++]];
                    }
                    else if (parameterMode == 1)
                    {
                        return m_program[m_position++];
                    }
                    else if (parameterMode == 2)
                    {
                        return m_program[m_relativeBase + m_program[m_position++]];
                    }
                    else
                    {
                        assert(false);
                    }

                };

                switch(opCode)
                {
                    case 1:
                        {
                            cout << "Sum!" << endl;
                            const auto sumand1 = nextParamValue();
                            const auto sumand2 = nextParamValue();
                            auto& dest = nextParamValue();
                            cout << " sumand1: " << sumand1 << " sumand2: " << sumand2 << " dest: " << dest << endl;
                            dest = sumand1 + sumand2;
                            break;
                        }
                    case 2:
                        {
                            cout << "Multiplication!" << endl;
                            const auto multiplicand1 = nextParamValue();
                            const auto multiplicand2 = nextParamValue();
                            auto& dest = nextParamValue();
                            cout << " multiplicand1: " << multiplicand1 << " multiplicand2: " << multiplicand2 << " dest: " << dest << endl;
                            dest = multiplicand1 * multiplicand2;
                            cout << "blee: " << m_program[63] << endl;
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
                            auto& dest = nextParamValue();
                            cout << " input: " << input << " dest: " << dest << endl;
                            dest = input;
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
                            auto& dest = nextParamValue();
                            cout << " param1: " << param1 << " param2: " << param2 << " dest: " << dest<< endl;
                            if (param1  < param2)
                               dest = 1;
                            else
                               dest = 0;
                            break;
                        }
                    case 8:
                        {
                            cout << "equals!" << endl;
                            const auto param1 = nextParamValue();
                            const auto param2 = nextParamValue();
                            auto& dest= nextParamValue();
                            cout << " param1: " << param1 << " param2: " << param2 << " dest: " << dest << endl;
                            if (param1 == param2)
                                dest = 1;
                            else
                                dest = 0;
                            break;
                        }
                    case 9:
                        {
                            cout << "Relative base offset!" << endl;
                            const auto adjustment = nextParamValue();
                            m_relativeBase += adjustment;
                            cout << " adjustment: " << adjustment << " new relative base: " << m_relativeBase << endl;
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
        map<int64_t, int64_t> m_program;
        int64_t m_position = 0;
        int64_t m_relativeBase = 0;
        deque<int64_t> m_inputs;
        vector<int64_t> m_output;
};

int main()
{
    vector<int64_t> program;
    int64_t programInput;
    while (true)
    {
        cin >> programInput;
        program.push_back(programInput);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    ProgramExecutor programExecutor(program);
    vector<int64_t> input;
    input.push_back(2);
    programExecutor.addInputs(input);
    const auto status = programExecutor.run();
    assert(status == ProgramExecutor::Terminated);
    cout << "output:" << endl;
    for (const auto x : programExecutor.output())
    {
        cout << x << " ";
    }
    cout << endl;
}
