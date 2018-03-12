#define VERIFY_HEIGHT_TRACKER
#define SUBMISSION
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
            m_makesDigitOneBegin.resize(maxBinaryDigits + 1);
            m_makesDigitOneEnd.resize(maxBinaryDigits + 1);

            int powerOf2 = 2;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_heightsModPowerOf2.push_back(vector<VersionedValue>(powerOf2));
                powerOf2 <<= 1;
            }
            clear();
        }
        void insertHeight(const int newHeight)
        {
            //cout << "insertHeight: " << newHeight << " m_cumulativeHeightAdjustment: " << m_cumulativeHeightAdjustment << endl;
            if (newHeight < m_smallestHeight)
                m_smallestHeight = newHeight;
            int powerOf2 = 2;
            int newHeightAdjusted = newHeight - m_cumulativeHeightAdjustment;
            if (newHeightAdjusted < 0)
            {
                newHeightAdjusted += (1 << (maxBinaryDigits + 1));
            }
            assert(newHeightAdjusted >= 0);
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                const int heightModPowerOf2 = newHeightAdjusted % powerOf2;
                numHeightsModPowerOf2(binaryDigitNum, heightModPowerOf2)++;
                if (m_makesDigitOneBegin[binaryDigitNum] <= m_makesDigitOneEnd[binaryDigitNum])
                {
                    if (heightModPowerOf2 >= m_makesDigitOneBegin[binaryDigitNum] && heightModPowerOf2 <= m_makesDigitOneEnd[binaryDigitNum])
                    {
                        m_grundyNumber ^= (powerOf2 >> 1);
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
                        m_grundyNumber ^= (powerOf2 >> 1);
                    }
                }

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
        bool canIncreaseHeights() const
        {
            // This is just  for testing - won't be needed in real-life.
            // TODO - remove this!
            return m_cumulativeHeightAdjustment <= maxHeight;
        }
        void adjustAllHeights(int heightDiff)
        {
            if (heightDiff == 0)
                return;
            assert(heightDiff == 1 || heightDiff == -1);
            m_cumulativeHeightAdjustment += heightDiff;
            m_smallestHeight += heightDiff;
            if (heightDiff == 1)
            {
                int powerOf2 = 2;
                for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // Scroll the begin/ end of the "makes digit one" zone to the left, updating m_grundyNumber
                    // on-the-fly.
                    int changeToNumberOfHeightsThatMakeDigitsOne = 0;
                    changeToNumberOfHeightsThatMakeDigitsOne -= numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);
                    m_makesDigitOneEnd[binaryDigitNum] = (powerOf2 + m_makesDigitOneEnd[binaryDigitNum] - 1) % powerOf2;

                    m_makesDigitOneBegin[binaryDigitNum] = (powerOf2 + m_makesDigitOneBegin[binaryDigitNum] - 1) % powerOf2;
                    changeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);

                    if (abs(changeToNumberOfHeightsThatMakeDigitsOne) % 2 == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
            }
            else
            {
                int powerOf2 = 2;
                for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // As above, but scroll the "makes digit one" zone to the right.
                    int changeToNumberOfHeightsThatMakeDigitsOne = 0;
                    changeToNumberOfHeightsThatMakeDigitsOne -= numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);
                    m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) % powerOf2;

                    m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) % powerOf2;
                    changeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);

                    if (abs(changeToNumberOfHeightsThatMakeDigitsOne) % 2 == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
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
            //cout << "Clear!" << endl;
            m_versionNumber++;
            int powerOf2 = 2;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                //m_heightsModPowerOf2.push_back(vector<int>(powerOf2, 0));
                m_makesDigitOneBegin[binaryDigitNum] = powerOf2 >> 1;
                m_makesDigitOneEnd[binaryDigitNum] = powerOf2 - 1;

                powerOf2 <<= 1;
            }
            m_grundyNumber = 0;
            m_cumulativeHeightAdjustment = 0;
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

        int m_cumulativeHeightAdjustment = 0;
        int m_grundyNumber = 0;

        int m_versionNumber = 0;
        int m_smallestHeight = 0;
};


int main()
{
#if 1
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
#endif

    HeightTracker heightTracker;

    int numInsertions = 0;
    int numAdjustments = 0;
    int numAdjustmentsUp = 0;
    int numAdjustmentsDown = 0;
    int totalNumInsertions = 0;
    int totalNumAdjustments = 0;
    int64_t blah = 0;
    while (true)
    {
        if (rand() % 100'000 == 0)
        {
            //cout << "numInsertions: " << numInsertions << " numAdjustments: " << numAdjustments << " (up: " << numAdjustmentsUp << " down: " << numAdjustmentsDown << ")" << " totalNumInsertions: " << totalNumInsertions << " totalNumAdjustments: " << totalNumAdjustments << endl;
            heightTracker.clear();
            numInsertions = 0;
            numAdjustments = 0;
            numAdjustmentsUp = 0;
            numAdjustmentsDown = 0;
        }
        if (rand() % 2 == 0)
        {
            const int newHeight = rand() % maxHeight;
            heightTracker.insertHeight(newHeight);
            numInsertions++;
            totalNumInsertions++;
        }
        else
        {
            if (rand() % 3 == 0 && heightTracker.canDecreaseHeights())
            {
                heightTracker.adjustAllHeights(-1);
                numAdjustments++;
                numAdjustmentsDown++;
                totalNumAdjustments++;
            }
            else
            {
                if (heightTracker.canIncreaseHeights())
                {
                    heightTracker.adjustAllHeights(1);
                    numAdjustments++;
                    numAdjustmentsUp++;
                    totalNumAdjustments++;
                }
            }
        }
        //cout << "blah: " << heightTracker.grundyNumber() << endl;
        blah += heightTracker.grundyNumber();

        if (totalNumInsertions + totalNumAdjustments >= 10'000'000)
            break;

    }
    cout << blah << endl;

}
