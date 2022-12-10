#include "CycleSimulator.h"
#include <iostream>
#include <sstream>
#include <cassert>

CycleSimulator::CycleSimulator(std::istream& instructionsStream)
    : m_instructionsStream(instructionsStream)
{
    readAndProcessNextInstruction();
}

int CycleSimulator::currentCycleNum() const
{
    return m_currentCycleNum;
}

int64_t CycleSimulator::registerValueDuringCurrentCycle() const
{
    return m_registerValue;
}

bool CycleSimulator::isFinished() const
{
    return !(m_instructionsStream || m_isProcessingAdd);
}

void CycleSimulator::computeNextCycle()
{
    if (isFinished())
    {
        m_currentCycleNum++;
        return;
    }

    // Finish off current cycle.
    bool isReadyForNextInstruction = true;
    if (m_isProcessingAdd)
    {
        if ( m_currentCycleNum == m_cycleNumAfterWhichAddFinishes )
        {
            m_registerValue += m_numberToAdd;

            m_isProcessingAdd = false;
            m_cycleNumAfterWhichAddFinishes = -1;
            m_numberToAdd = -1;

        }
        else
        {
            isReadyForNextInstruction = false;
        }
    }

    // Previous cycle complete; onto the next cycle!
    m_currentCycleNum++;
    if (isReadyForNextInstruction)
        readAndProcessNextInstruction();
}

void CycleSimulator::readAndProcessNextInstruction()
{
    std::string instructionLine;
    if (std::getline(m_instructionsStream, instructionLine))
    {
        std::istringstream instructionLineStream(instructionLine);
        std::string command;
        instructionLineStream >> command;
        assert(instructionLineStream);
        if (command == "noop")
        {
            // Do nothing.
        }
        else if (command == "addx")
        {
            m_isProcessingAdd = true;
            m_cycleNumAfterWhichAddFinishes = m_currentCycleNum + 1;
            instructionLineStream >> m_numberToAdd;
            assert(instructionLineStream);
        }
        else
        {
            assert(false);
        }
    }
}

