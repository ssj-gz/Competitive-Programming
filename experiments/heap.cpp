#include "memorypool.h"

template <typename ValueType, typename Compare, typename KeyDecrease>
class Heap
{
    private:
        struct HandleImpl;
    public:
        Heap(int maxElements, Compare comparator, KeyDecrease keyDecreaser)
            : m_maxElements{maxElements}, 
              m_valueMemoryPool{sizeof(ValueType), maxElements},
              m_handleImplMemoryPool{sizeof(HandleImpl), maxElements},
              m_elements(maxElements),
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
                ~Handle()
                {
                    m_handleImpl->decRefCount();
                }
                bool isValid() const
                {
                    return m_handleImpl->isValid();
                }
                const ValueType& value() const
                {
                    assert(m_handleImpl->isValid());
                    return m_handleImpl->value();
                }
            private:
                Handle(HandleImpl* handleImpl)
                    : m_handleImpl(handleImpl)
                {
                    m_isValid = true;
                    m_handleImpl->incRefCount();
                };
                bool m_isValid = false;
                HandleImpl* m_handleImpl = nullptr;
                friend class Heap;
        };
        Handle add(const ValueType& value)
        {
            assert(m_numElements < m_maxElements);
            m_elements[m_numElements].value = new (m_valueMemoryPool.allocChunk()) ValueType(value);
            m_elements[m_numElements].handleImpl = new (m_handleImplMemoryPool.allocChunk()) HandleImpl(&(m_elements[m_numElements]), m_handleImplMemoryPool);
            Handle newValueHandle{m_elements[m_numElements].handleImpl};
            onKeyDecreased(m_numElements);
            m_numElements++;
            verifyHeap();

            return newValueHandle;
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
            m_valueMemoryPool.dealloc(m_elements[0].value);
            m_elements[0].handleImpl->m_heapElement = nullptr;
            m_elements[0] = m_elements[m_numElements - 1];
            m_numElements--;
            minHeapify(0);
            verifyHeap();
        }
        template <typename DecreaseBy>
        void decreaseKey(const Handle& valueHandle, const DecreaseBy& decreaseBy)
        {
            assert(valueHandle.isValid());
            m_keyDecreaser(valueHandle.m_handleImpl->value(), decreaseBy);
            const int heapIndex = valueHandle.m_handleImpl->m_heapElement - m_elements.data();
            onKeyDecreased(heapIndex);
        }
    private:
        struct Element;
        struct HandleImpl
        {
            HandleImpl(Element* heapElement, MemoryPool& memoryPool)
                : m_heapElement{heapElement}, m_memoryPool(memoryPool)
            {
            }
            bool isValid() const
            {
                return (m_heapElement != nullptr);
            }
            ValueType& value()
            {
                assert(isValid());
                return *m_heapElement->value;

            }
            void incRefCount()
            {
                m_refCount++;
            }
            void decRefCount()
            {
                m_refCount--;
                if (m_refCount == 0)
                {
                    m_memoryPool.dealloc(this);
                }
                this->~HandleImpl();
            }
        private:
            int m_refCount = 0;
            Element* m_heapElement = nullptr;
            MemoryPool& m_memoryPool;
            friend class Heap;
        };
        int m_maxElements = -1;
        int m_numElements = 0;
        MemoryPool m_valueMemoryPool;
        MemoryPool m_handleImplMemoryPool;
        struct Element
        {
            ValueType* value;
            HandleImpl* handleImpl;
        };
        vector<Element> m_elements;
        Compare m_comparator;
        KeyDecrease m_keyDecreaser;

        void minHeapify(int heapIndex)
        {
            int leftIndex = left(heapIndex);
            int rightIndex = right(heapIndex);
            int indexOfSmallest = -1;
            if (leftIndex < m_numElements && m_comparator(*m_elements[leftIndex].value, *m_elements[heapIndex].value))
            {
                indexOfSmallest = leftIndex;
            }
            else
            {
                indexOfSmallest = heapIndex;
            }
            if (rightIndex < m_numElements && m_comparator(*m_elements[rightIndex].value, *m_elements[indexOfSmallest].value))
            {
                indexOfSmallest = rightIndex;
            }
            assert(indexOfSmallest != -1);
            if (indexOfSmallest != heapIndex)
            {
                swapElements(heapIndex, indexOfSmallest);
                minHeapify(indexOfSmallest);
            }
        }
        void onKeyDecreased(int keyIndex)
        {
            int index = keyIndex;
            while (index > 0 && m_comparator(*m_elements[index].value, *m_elements[parent(index)].value))
            {
                swapElements(index, parent(index));
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
#ifndef NDEBUG
            for (int i = 0; i < m_numElements; i++)
            {
                if (left(i) < m_numElements && right(i) < m_numElements)
                {
                    assert(!m_comparator(*m_elements[left(i)].value, *m_elements[i].value));
                    assert(!m_comparator(*m_elements[right(i)].value, *m_elements[i].value));
                }
            }
#endif
        }
        void swapElements(int element1Index, int element2Index)
        {
            m_elements[element1Index].handleImpl->m_heapElement = &(m_elements[element2Index]);
            m_elements[element2Index].handleImpl->m_heapElement = &(m_elements[element1Index]);
            swap(m_elements[element1Index], m_elements[element2Index]);
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
    Handle wee = heapOfA.add(10);
    for (int i = 0; i < 99; i++)
    {
        as.emplace_back(rand() % 20);
        heapOfA.add(as.back());
    }
    for (int i = 0; i < 99; i++)
    {
        cout << "a[" << i << "] = " << as[i].keyValue << endl;
    }
    cout << " gloop: " << wee.value().keyValue << endl;
    heapOfA.decreaseKey(wee, 5);
    cout << " gloop: " << wee.value().keyValue << endl;
    for (int i = 0; i < 100; i++)
    {
        cout << " min: " << heapOfA.min().keyValue << endl;
        heapOfA.extractMin();
        if (wee.isValid())
            cout << " gloop: " << wee.value().keyValue << endl;
        else
            cout << " gloop: " << " not valid!" << endl;
    }

}
