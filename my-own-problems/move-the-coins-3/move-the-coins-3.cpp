#define VERIFY_HEIGHT_TRACKER
#ifdef SUBMISSION
#define NDEBUG
#undef VERIFY_HEIGHT_TRACKER
#endif
#include <iostream>
#include <vector>
#include <cassert>
#include <sys/time.h>


using namespace std;

constexpr auto maxHeight = 100'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
        return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr auto maxBinaryDigits = log2(maxHeight);

class HeightTracker
{
    public:
        HeightTracker()
        {
            int powerOf2 = 2;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_heightsModPowerOf2.push_back(vector<int>(powerOf2, 0));
                m_makesDigitOneBegin.push_back(powerOf2 >> 1);
                m_makesDigitOneEnd.push_back(powerOf2 - 1);
                m_numHeightsThatMakeDigitOne.push_back(0);

                powerOf2 <<= 1;
            }
        }
        void insertHeight(const int newHeight)
        {
            cout << "insertHeight: " << newHeight << " m_cumulativeHeightAdjustment: " << m_cumulativeHeightAdjustment << endl;
            int powerOf2 = 2;
            m_grundyNumber = 0;
            int newHeightAdjusted = newHeight - m_cumulativeHeightAdjustment;
            if (newHeightAdjusted < 0)
            {
                newHeightAdjusted += (1 << (maxBinaryDigits + 1));
            }
            assert(newHeightAdjusted >= 0);
            //cout << "newHeightAdjusted: " << newHeightAdjusted << endl;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                const int heightModPowerOf2 = newHeightAdjusted % powerOf2;
                m_heightsModPowerOf2[binaryDigitNum][heightModPowerOf2]++;
                if (m_makesDigitOneBegin[binaryDigitNum] <= m_makesDigitOneEnd[binaryDigitNum])
                {
                    if (heightModPowerOf2 >= m_makesDigitOneBegin[binaryDigitNum] && heightModPowerOf2 <= m_makesDigitOneEnd[binaryDigitNum])
                    {
                        m_numHeightsThatMakeDigitOne[binaryDigitNum]++;
                    }
                }
                else
                {
                    const int makeDigitZeroBegin = m_makesDigitOneEnd[binaryDigitNum] + 1;
                    const int makeDigitZeroEnd = m_makesDigitOneBegin[binaryDigitNum] - 1;
                    assert(makeDigitZeroBegin <= makeDigitZeroEnd);
                    assert(0 <= makeDigitZeroEnd < powerOf2);
                    assert(0 <= makeDigitZeroBegin < powerOf2);
                    if (!(heightModPowerOf2 >= makeDigitZeroBegin && heightModPowerOf2 <= makeDigitZeroEnd))
                    {
                        m_numHeightsThatMakeDigitOne[binaryDigitNum]++;
                    }
                }


                m_grundyNumber ^= ((powerOf2 >> 1) * (m_numHeightsThatMakeDigitOne[binaryDigitNum] % 2));

                powerOf2 <<= 1;
            }
#ifdef VERIFY_HEIGHT_TRACKER
            m_dbgHeights.push_back(newHeight);
#endif
        };
        bool canDecreaseHeights() const
        {
            // This is just for debugging - situation won't occurr in real-life.
            for (const auto height : m_dbgHeights)
            {
                if (height <= 0)
                    return false;
            }
            return true;
        }
        void adjustAllHeights(int heightDiff)
        {
            if (heightDiff == 0)
                return;
            assert(heightDiff == 1 || heightDiff == -1);
            cout << "adjustAllHeights: " << heightDiff << endl;
            m_cumulativeHeightAdjustment += heightDiff;
            if (heightDiff == 1)
            {
                int powerOf2 = 2;
                for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    m_numHeightsThatMakeDigitOne[binaryDigitNum] -= m_heightsModPowerOf2[binaryDigitNum][m_makesDigitOneEnd[binaryDigitNum]];
                    m_makesDigitOneEnd[binaryDigitNum] = (powerOf2 + m_makesDigitOneEnd[binaryDigitNum] - 1) % powerOf2;

                    m_makesDigitOneBegin[binaryDigitNum] = (powerOf2 + m_makesDigitOneBegin[binaryDigitNum] - 1) % powerOf2;
                    m_numHeightsThatMakeDigitOne[binaryDigitNum] += m_heightsModPowerOf2[binaryDigitNum][m_makesDigitOneBegin[binaryDigitNum]];

                    powerOf2 <<= 1;
                } 
            }
            else
            {
                int powerOf2 = 2;
                for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    m_numHeightsThatMakeDigitOne[binaryDigitNum] -= m_heightsModPowerOf2[binaryDigitNum][m_makesDigitOneBegin[binaryDigitNum]];
                    m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) % powerOf2;

                    m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) % powerOf2;
                    m_numHeightsThatMakeDigitOne[binaryDigitNum] += m_heightsModPowerOf2[binaryDigitNum][m_makesDigitOneEnd[binaryDigitNum]];

                    powerOf2 <<= 1;
                } 
            }
            int powerOf2 = 2;
            m_grundyNumber = 0;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_grundyNumber ^= ((powerOf2 >> 1) * (m_numHeightsThatMakeDigitOne[binaryDigitNum] % 2));
                powerOf2 <<= 1;
            }
#ifdef VERIFY_HEIGHT_TRACKER
            for (auto& height : m_dbgHeights)
            {
                height += heightDiff;
                assert(height >= 0);
            }
#endif
        }
        int grundyNumber() const
        {
#ifdef VERIFY_HEIGHT_TRACKER
            int dbgGrundyNumber = 0;
            for (const auto height : m_dbgHeights)
            {
                dbgGrundyNumber ^= height;
            }
            cout << "dbgGrundyNumber: " << dbgGrundyNumber << " m_grundyNumber: " << m_grundyNumber << endl;
            assert(dbgGrundyNumber == m_grundyNumber);
#endif
            return m_grundyNumber;
        }
        void clear()
        {
            // TODO - use a versionNumber to make this O(logN).
            int powerOf2 = 2;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_heightsModPowerOf2.push_back(vector<int>(powerOf2, 0));
                m_makesDigitOneBegin.push_back(powerOf2 >> 1);
                m_makesDigitOneEnd.push_back(powerOf2 - 1);
                m_numHeightsThatMakeDigitOne[binaryDigitNum] = 0;

                powerOf2 <<= 1;
            }
            m_grundyNumber = 0;
        }
#ifdef VERIFY_HEIGHT_TRACKER
        vector<int> m_dbgHeights;
#endif
        vector<vector<int>> m_heightsModPowerOf2;
        vector<int> m_makesDigitOneBegin;
        vector<int> m_makesDigitOneEnd;

        vector<int> m_numHeightsThatMakeDigitOne;
        int m_cumulativeHeightAdjustment = 0;
        int m_grundyNumber = 0;
};


int main()
{
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    HeightTracker heightTracker;
    heightTracker.adjustAllHeights(-1);
    heightTracker.insertHeight(15);
    //cout << "blah: " << heightTracker.grundyNumber() << endl;
#if 0
    heightTracker.insertHeight(4);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.insertHeight(6);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.insertHeight(7);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
    heightTracker.adjustAllHeights(1);
    cout << "blah: " << heightTracker.grundyNumber() << endl;
#endif

    while (true)
    {
        if (rand() % 2 == 0)
        {
            const int newHeight = rand() % maxHeight;
            heightTracker.insertHeight(newHeight);
        }
        else
        {
            if (rand() % 2 == 0)
            {
                if (heightTracker.canDecreaseHeights())
                    heightTracker.adjustAllHeights(-1);
            }
#if 1
            else
            {
                heightTracker.adjustAllHeights(1);
            }
#endif
        }
        cout << "blah: " << heightTracker.grundyNumber() << endl;

    }

}
