#ifndef CYCLE_SIMULATOR_H
#define CYCLE_SIMULATOR_H

#include <iosfwd>
#include <cstdint>

class CycleSimulator
{
    public:
        CycleSimulator(std::istream& instructionsStream);

        int currentCycleNum() const;

        std::int64_t registerValueDuringCurrentCycle() const;

        bool isFinished() const;

        void computeNextCycle();

    private:
        std::istream& m_instructionsStream;

        std::int64_t m_registerValue = 1;

        int m_currentCycleNum = 1;

        bool m_isProcessingAdd = false;
        int m_cycleNumAfterWhichAddFinishes = -1;
        std::int64_t m_numberToAdd = 0;

        void readAndProcessNextInstruction();

};




#endif

