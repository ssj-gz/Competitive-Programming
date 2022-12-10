#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

class CycleSimulator
{
    public:
        CycleSimulator(istream& instructionsStream)
            : m_instructionsStream(instructionsStream)
        {
            readAndProcessNextInstruction();
        }

        int currentCycleNum() const
        {
            return m_currentCycleNum;
        }

        int64_t registerValueDuringCurrentCycle() const
        {
            return m_registerValue;
        }

        bool isFinished() const
        {
            return !(m_instructionsStream || m_isProcessingAdd);
        }

        void computeNextCycle()
        {
            if (isFinished())
            {
                m_currentCycleNum++;
                return;
            }

            // Finish off previous cycle.
            bool isReadyForNextInstruction = true;
            if (m_isProcessingAdd)
            {
                if ( m_currentCycleNum == m_cycleNumAfterWhichAddFinished )
                {
                    m_registerValue += m_numberToAdd;
                    std::cout << "added " << m_numberToAdd << " to register after cycle: " << m_currentCycleNum << " to give: " << m_registerValue << std::endl;

                    m_isProcessingAdd = false;
                    m_cycleNumAfterWhichAddFinished = -1;
                    m_numberToAdd = -1;

                }
                else
                {
                    isReadyForNextInstruction = false;
                }
            }

            m_currentCycleNum++;
            if (isReadyForNextInstruction)
                readAndProcessNextInstruction();
        }


    private:
        istream& m_instructionsStream;

        int64_t m_registerValue = 1;

        int m_currentCycleNum = 1;

        bool m_isProcessingAdd = false;
        int m_cycleNumAfterWhichAddFinished = -1;
        int64_t m_numberToAdd = 0;

        void readAndProcessNextInstruction()
        {
            string instructionLine;
            getline(m_instructionsStream, instructionLine);
            if (!instructionLine.empty())
            {
                istringstream instructionLineStream(instructionLine);
                string command;
                instructionLineStream >> command;
                assert(instructionLineStream);
                cout << "command: " << command << endl;
                if (command == "noop")
                {
                    // Do nothing.
                }
                else if (command == "addx")
                {
                    m_isProcessingAdd = true;
                    instructionLineStream >> m_numberToAdd;
                    cout << "Need to add " << m_numberToAdd << std::endl;
                    assert(instructionLineStream);
                    m_cycleNumAfterWhichAddFinished = m_currentCycleNum + 1;
                }
                else
                {
                    assert(false);
                }
            }
            std::cout << "instructionLine: " << instructionLine << " m_instructionsStream: " << static_cast<bool>(m_instructionsStream) << std::endl;
        }

};

int main()
{

    string currentCrtRowPixels;
    int currentCrlCol = 0; // 0-relative.

    CycleSimulator cycleSimulator(cin);

    while (!cycleSimulator.isFinished())
    {
        if (abs(currentCrlCol - cycleSimulator.registerValueDuringCurrentCycle()) <= 1)
            currentCrtRowPixels += "#";
        else
            currentCrtRowPixels += ".";

        currentCrlCol++;
        if (currentCrlCol == 40)
        {
            currentCrlCol = 0;
            std::cout << "Row: " << currentCrtRowPixels << std::endl;
            currentCrtRowPixels.clear();
        }

        cycleSimulator.computeNextCycle();
    }
}
