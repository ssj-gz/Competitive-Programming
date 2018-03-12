#define VERIFY_HEIGHT_TRACKER
//#define SUBMISSION
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
                m_heightsModPowerOf2.push_back(vector<VersionedValue>(powerOf2));
                m_makesDigitOneBegin.push_back(powerOf2 >> 1);
                m_makesDigitOneEnd.push_back(powerOf2 - 1);
                m_numHeightsThatMakeDigitOne.push_back(0);

                powerOf2 <<= 1;
            }
        }
        void insertHeight(const int newHeight)
        {
            cout << "insertHeight: " << newHeight << " m_cumulativeHeightAdjustment: " << m_cumulativeHeightAdjustment << endl;
            if (newHeight < m_smallestHeight)
                m_smallestHeight = newHeight;
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
                //m_heightsModPowerOf2[binaryDigitNum][heightModPowerOf2]++;
                numHeightsModPowerOf2(binaryDigitNum, heightModPowerOf2)++;
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
            // This is just  for testing - won't be needed in real-life.
            // TODO - remove this!
            if (m_smallestHeight == 0)
                return false;
            return true;
        }
        void adjustAllHeights(int heightDiff)
        {
            if (heightDiff == 0)
                return;
            assert(heightDiff == 1 || heightDiff == -1);
            cout << "adjustAllHeights: " << heightDiff << endl;
            m_cumulativeHeightAdjustment += heightDiff;
            m_smallestHeight += heightDiff;
            if (heightDiff == 1)
            {
                int powerOf2 = 2;
                for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    m_numHeightsThatMakeDigitOne[binaryDigitNum] -= numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);
                    m_makesDigitOneEnd[binaryDigitNum] = (powerOf2 + m_makesDigitOneEnd[binaryDigitNum] - 1) % powerOf2;

                    m_makesDigitOneBegin[binaryDigitNum] = (powerOf2 + m_makesDigitOneBegin[binaryDigitNum] - 1) % powerOf2;
                    m_numHeightsThatMakeDigitOne[binaryDigitNum] += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);

                    powerOf2 <<= 1;
                } 
            }
            else
            {
                int powerOf2 = 2;
                for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    m_numHeightsThatMakeDigitOne[binaryDigitNum] -= numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);
                    m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) % powerOf2;

                    m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) % powerOf2;
                    m_numHeightsThatMakeDigitOne[binaryDigitNum] += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);

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
        int& numHeightsModPowerOf2(int binaryDigitNum, int modPowerOf2)
        {
            auto& numHeights = m_heightsModPowerOf2[binaryDigitNum][modPowerOf2];
            if (numHeights.versionNumber != m_versionNumber)
            {
                numHeights.value = 0;
                numHeights.versionNumber = m_versionNumber;
            }
            return numHeights.value;
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
            cout << "Clear!" << endl;
            m_versionNumber++;
            int powerOf2 = 2;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                //m_heightsModPowerOf2.push_back(vector<int>(powerOf2, 0));
                m_makesDigitOneBegin.push_back(powerOf2 >> 1);
                m_makesDigitOneEnd.push_back(powerOf2 - 1);
                m_numHeightsThatMakeDigitOne[binaryDigitNum] = 0;

                powerOf2 <<= 1;
            }
            m_grundyNumber = 0;
#ifdef VERIFY_HEIGHT_TRACKER
            m_dbgHeights.clear();
#endif
        }
#ifdef VERIFY_HEIGHT_TRACKER
        vector<int> m_dbgHeights;
#endif
        struct VersionedValue
        {
            int value = 0;
            int versionNumber = -1;
        };
        vector<vector<VersionedValue>> m_heightsModPowerOf2;
        vector<int> m_makesDigitOneBegin;
        vector<int> m_makesDigitOneEnd;

        vector<int> m_numHeightsThatMakeDigitOne;
        int m_cumulativeHeightAdjustment = 0;
        int m_grundyNumber = 0;

        int m_versionNumber = 0;
        int m_smallestHeight = 0;
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

    int numInsertions = 0;
    int numAdjustments = 0;
    while (true)
    {
        if (rand() % 10000 == 0)
        {
            cout << "numInsertions: " << numInsertions << " numAdjustments: " << numAdjustments << endl;
            heightTracker.clear();
            numInsertions = 0;
            numAdjustments = 0;
        }
        if (rand() % 2 == 0)
        {
            const int newHeight = rand() % maxHeight;
            heightTracker.insertHeight(newHeight);
            numInsertions++;
        }
        else
        {
            if (rand() % 2 == 0)
            {
                if (heightTracker.canDecreaseHeights())
                {
                    heightTracker.adjustAllHeights(-1);
                    numAdjustments++;
                }
            }
            else
            {
                heightTracker.adjustAllHeights(1);
                numAdjustments++;
            }
        }
        cout << "blah: " << heightTracker.grundyNumber() << endl;

    }

}
