#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

//#define VERIFY_DEALLOCS

using namespace std;

class MemoryPool
{
    public:
        MemoryPool(int chunkSize, int numChunks)
            : m_chunkSize{chunkSize}, m_numChunks{numChunks}
        {
            m_memoryVector.resize(m_chunkSize * m_numChunks);
            m_freeChunks.reserve(m_numChunks);
            m_memoryBegin = static_cast<unsigned char*>(m_memoryVector.data());
            cout << " m_memoryBegin: " << static_cast<void*>(m_memoryBegin) << endl;
            for (int i = 0; i < m_numChunks; i++)
            {
                m_freeChunks.push_back(m_memoryBegin + i * m_chunkSize);
            }
            cout << " floop: " << m_freeChunks.front() << endl;

        }
        void* allocChunk()
        {
            assert(m_freeChunks.size() > 0);
            void *allocedChunk = static_cast<unsigned char*>(m_freeChunks.front());
            m_freeChunks.front() = m_freeChunks.back();
            m_freeChunks.pop_back();
            return allocedChunk;
        }
        void dealloc(void* chunkToDealloc)
        {
#ifdef VERIFY_DEALLOCS
            assert(std::find(m_freeChunks.begin(), m_freeChunks.end(), static_cast<unsigned char*>(chunkToDealloc)) == m_freeChunks.end());
#endif
            m_freeChunks.push_back(static_cast<unsigned char*>(chunkToDealloc));
        }
    private:
        int m_chunkSize = -1;
        int m_numChunks = -1;
        vector<unsigned char> m_memoryVector;
        vector<unsigned char*> m_freeChunks;
        unsigned char* m_memoryBegin = nullptr;
};

