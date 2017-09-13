#include "memorypool.h"

template <typename ValueType, typename Compare, typename KeyDecrease>
class Heap
{
    public:
        Heap(int maxElements, Compare comparator, KeyDecrease keyDecreaser)
            : m_maxElements{maxElements}, 
              m_memoryPool{sizeof(ValueType), maxElements},
              m_elements(maxElements),
              m_handles(maxElements),
              m_comparator{comparator},
              m_keyDecreaser{keyDecreaser}
        {
        }
        class Handle
        {
            public:
                Handle()
                {
                }
                const ValueType& value() const
                {
                }
            private:
                bool m_isValid = false;
                ValueType* m_value;
                friend class Heap;
        };
        Handle add(const ValueType& value)
        {
            assert(m_numElements < m_maxElements);
            void* blah = m_memoryPool.allocChunk();
            cout << " blah: " << blah << endl;
            m_elements[m_numElements].value = new (blah) ValueType(value);
            onKeyDecreased(m_numElements);
            m_numElements++;

            return Handle{};
        }
        int size()
        {
            return m_numElements;
        }
        const ValueType& min()
        {
            assert(m_numElements > 0);
            return *(m_elements[0].value);
        }
        void extractMin()
        {
            assert(m_numElements > 0);
            m_memoryPool.dealloc(m_elements[0].value);
            m_elements[0] = m_elements[m_numElements - 1];
            m_numElements--;
            minHeapify(0);
        }
        template <typename DecreaseBy>
        void decreaseKey(const Handle& valueHandle, const DecreaseBy& decreaseBy)
        {
            m_keyDecreaser(*(valueHandle.m_value), decreaseBy);
        }
    private:
        int m_maxElements = -1;
        int m_numElements = 0;
        MemoryPool m_memoryPool;
        struct Element
        {
            ValueType* value;
            Handle* handle;
        };
        vector<Handle> m_handles;
        vector<Element> m_elements;
        Compare m_comparator;
        KeyDecrease m_keyDecreaser;

        void minHeapify(int heapIndex)
        {
            int leftIndex = left(heapIndex);
            int rightIndex = right(heapIndex);
            int indexOfSmallest = -1;
            if (leftIndex < m_numElements && m_comparator(*m_elements[heapIndex].value, *m_elements[leftIndex].value))
            {
                indexOfSmallest = heapIndex;
            }
            else
            {
                indexOfSmallest = leftIndex;
            }
            if (rightIndex < m_numElements && m_comparator(*m_elements[rightIndex].value, *m_elements[indexOfSmallest].value))
            {
                indexOfSmallest = rightIndex;
            }
            if (indexOfSmallest != heapIndex)
            {
                swap(m_elements[heapIndex], m_elements[indexOfSmallest]);
                minHeapify(indexOfSmallest);
            }
        }
        void onKeyDecreased(int keyIndex)
        {
            int index = keyIndex;
            while (index > 0 && m_comparator(*m_elements[index].value, *m_elements[parent(index)].value))
            {
                swap(m_elements[index], m_elements[parent(index)]);
                index = parent(index);
            }
        }
        int parent(int heapIndex)
        {
            assert(heapIndex != 0);
            return (heapIndex - 1) / 2;
        }
        int left(int heapIndex)
        {
            return (heapIndex + 1) * 2 - 1;
        }
        int right(int heapIndex)
        {
            return (heapIndex + 1) * 2;
        }
        void verifyHeap()
        {
        }

};

struct A
{
    public:
        A(int value)
            : keyValue{value}
        {
        }

        int keyValue = -1;
};


int main()
{
    auto decreaseKey = [](A& value, int decreaseKeyBy) { value.keyValue -= decreaseKeyBy; };
    auto comparator = [](const A& lhs, const A& rhs) { return lhs.keyValue < rhs.keyValue; };
    Heap<A, decltype(comparator), decltype(decreaseKey)> heapOfA(100, comparator, decreaseKey);
    using Handle = decltype(heapOfA)::Handle;

    //A a1(3);
    //Handle a1Handle = heapOfA.add(a1);
    //A a2(2);
    //Handle a2Handle = heapOfA.add(a2);
    //cout << "blee: " << heapOfA.min().keyValue << endl;
    vector<A> as;
    for (int i = 0; i < 10; i++)
    {
        as.emplace_back(rand() % 10);
        heapOfA.add(as.back());
    }
    for (int i = 0; i < 10; i++)
    {
        cout << "a[" << i << "] = " << as[i].keyValue << endl;
    }
    for (int i = 0; i < 10; i++)
    {
        cout << " min: " << heapOfA.min().keyValue << endl;
        heapOfA.extractMin();
    }

}
