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
              m_elements(maxElements),
              m_comparator{comparator},
              m_keyDecreaser{keyDecreaser}
        {
            m_handleImpls.reserve(maxElements);
        }
        class Handle
        {
            public:
                Handle()
                {
                }
                ~Handle()
                {
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
                int id() const
                {
                    assert(m_handleImpl->isValid());
                    return m_handleImpl->id();
                }
            private:
                Handle(HandleImpl* handleImpl)
                    : m_handleImpl(handleImpl)
                {
                    m_isValid = true;
                };
                bool m_isValid = false;
                HandleImpl* m_handleImpl = nullptr;
                friend class Heap;
        };
        Handle add(const ValueType& value)
        {
            assert(m_totalElementsAdded < m_maxElements);
            m_elements[m_numElements].value = new (m_valueMemoryPool.allocChunk()) ValueType(value);
            m_handleImpls.emplace_back(&(m_elements[m_numElements]), m_totalElementsAdded);
            m_elements[m_numElements].handleImpl = &(m_handleImpls.back());
            Handle newValueHandle{m_elements[m_numElements].handleImpl};
            onKeyDecreased(m_numElements);

            m_numElements++;
            m_totalElementsAdded++;
            verifyHeap();

            return newValueHandle;
        }
        Handle handleById(int handleId)
        {
            return Handle{&(m_handleImpls[handleId])};
        }
        int size() const
        {
            return m_numElements;
        }
        bool empty() const
        {
            return (m_numElements == 0);
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
            HandleImpl(Element* heapElement, int id)
                : m_heapElement{heapElement}, m_id(id)
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
            int id() const
            {
                return m_id;
            }
        private:
            Element* m_heapElement = nullptr;
            int m_id = -1;
            friend class Heap;
        };
        int m_totalElementsAdded = 0;
        int m_maxElements = -1;
        int m_numElements = 0;
        MemoryPool m_valueMemoryPool;
        struct Element
        {
            ValueType* value;
            HandleImpl* handleImpl;
        };
        vector<Element> m_elements;
        vector<HandleImpl> m_handleImpls;
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

