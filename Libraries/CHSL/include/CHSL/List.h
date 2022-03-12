#pragma once

#include "Debug.h"

namespace cs
{

    template<typename T>
    class ListIterator
    {
    public:
        ListIterator(T* target);

        T& operator*();
        ListIterator& operator++();
        bool operator==(const ListIterator&);
        bool operator!=(const ListIterator&);

    private:
        T* m_target;
    };

    template<typename T>
    class List
    {
    public:
        List();
        ~List();
        List(const List<T>& l_val);
        List(List<T>&& r_val);

        List<T>& operator=(const List<T>& l_val);
        List<T>& operator=(List<T>&& r_val);

        T& operator[](int index) const;

        int Size() const;
        int Capacity() const;
        bool Empty() const;
        T& At(int index) const;
        T& Front() const;
        T& Back() const;

        void Insert(int index, const T& value);
        void Remove(int index);
        void Add(const T& value);
        void Clear();
        T Pop();

        const T* Data() const;

        ListIterator<T> begin() const;
        ListIterator<T> end() const;

    private:
        void BoundArray();
        void ShrinkArray();
        void GrowArray();

    private:
        static constexpr int c_dCapacity = 8;

        T* m_elements;
        int m_size;
        int m_capacity;
    };





    // Implementation

    // Constructors

    template<typename T>
    inline List<T>::List()
        :
        m_elements(new T[c_dCapacity]),
        m_size(0),
        m_capacity(c_dCapacity)
    {

    }

    template<typename T>
    inline List<T>::~List()
    {
        delete[] m_elements;
    }

    template<typename T>
    inline List<T>::List(const List<T>& lVal)
        :
        m_elements(nullptr)
    {
        *this = lVal;
    }

    template<typename T>
    inline List<T>::List(List<T>&& rVal)
        :
        m_elements(nullptr)
    {
        *this = static_cast<List<T>&&>(rVal);
    }





    // Operators

    template<typename T>
    List<T>& List<T>::operator=(const List<T>& l_val)
    {
        if (this == &l_val)
        {
            return *this;
        }

        m_size = l_val.m_size;
        m_capacity = l_val.m_capacity;

        if (m_elements != nullptr)
        {
            delete[] m_elements;
        }

        m_elements = new T[m_capacity];
        for (int i = 0; i < m_size; ++i)
        {
            m_elements[i] = l_val.m_elements[i];
        }

        // memcpy(m_elements, l_val.m_elements, sizeof(T) * m_size);

        return *this;
    }
    template<typename T>
    List<T>& List<T>::operator=(List<T>&& r_val)
    {
        if (this == &r_val)
        {
            return *this;
        }

        m_size = r_val.m_size;
        m_capacity = r_val.m_capacity;

        if (m_elements != nullptr)
        {
            delete[] m_elements;
        }

        m_elements = r_val.m_elements;

        r_val.m_elements = nullptr;
        r_val.Clear();

        return *this;
    }

    template<typename T>
    inline T& List<T>::operator[](int index) const
    {
        if (index < 0 || index >= m_size)
        {
            throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Cannot access member outside bounds of List.");
        }

        return m_elements[index];
    }





    // Information and accessors

    template<typename T>
    inline int List<T>::Size() const
    {
        return m_size;
    }

    template<typename T>
    inline int List<T>::Capacity() const
    {
        return m_capacity;
    }

    template<typename T>
    inline bool List<T>::Empty() const
    {
        return m_size == 0;
    }

    template<typename T>
    inline T& List<T>::At(int index) const
    {
        if (index < 0 || index >= m_size)
        {
            throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Cannot access member outside bounds of List.");
        }

        return m_elements[index];
    }

    template<typename T>
    inline T& List<T>::Front() const
    {
        if (m_size == 0)
        {
            throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Cannot access front member of empty List.");
        }

        return m_elements[0];
    }

    template<typename T>
    inline T& List<T>::Back() const
    {
        if (m_size == 0)
        {
            throw cs::ExceptionGeneral(__FILE__, __FUNCTION__, __LINE__, "Cannot access rear member of empty list.");
        }

        return m_elements[m_size - 1];
    }

    template<typename T>
    inline const T* List<T>::Data() const
    {
        return m_elements;
    }

    template<typename T>
    inline ListIterator<T> List<T>::begin() const
    {
        return ListIterator<T>(m_elements);
    }

    template<typename T>
    inline ListIterator<T> List<T>::end() const
    {
        return ListIterator<T>(m_elements + m_size);
    }





    // Operations

    template<typename T>
    inline void List<T>::Insert(int index, const T& value)
    {
        BoundArray();
        if (index < 0 || index > m_size)
        {
            return;
        }

        for (int i = m_size; i > index; --i)
        {
            m_elements[i] = m_elements[i - 1];
        }

        m_elements[index] = value;
        m_size++;

    }

    template<typename T>
    inline void List<T>::Remove(int index)
    {
        if (index < 0 || index >= m_size)
        {
            return;
        }

        for (int i = index; i < m_size - 1; ++i)
        {
            m_elements[i] = m_elements[i + 1];
        }

        m_size--;

        //BoundArray();
    }

    template<typename T>
    inline void List<T>::Add(const T& value)
    {
        BoundArray();
        m_elements[m_size] = value;

        m_size++;

    }

    template<typename T>
    inline T List<T>::Pop()
    {
        if (m_size == 0)
        {
            return;
        }

        m_size--;

        return m_elements[m_size];

        //BoundArray();
    }

    template<typename T>
    inline void List<T>::Clear()
    {
        m_size = 0;
        m_capacity = c_dCapacity;

        if (m_elements != nullptr)
        {
            delete[] m_elements;
        }

        m_elements = new T[c_dCapacity];
    }





    // Privates

    template<typename T>
    inline void List<T>::BoundArray()
    {
        if (m_size == m_capacity)
        {
            GrowArray();
            return;
        }

        if (m_size < m_capacity / 2 && m_capacity > c_dCapacity)
        {
            ShrinkArray();
        }
    }

    template<typename T>
    inline void List<T>::ShrinkArray()
    {
        T* newElements = new T[m_capacity / 2];
        for (int i = 0; i < m_size; ++i)
        {
            newElements[i] = m_elements[i];
        }

        delete[] m_elements;
        m_elements = newElements;
        m_capacity /= 2;
    }

    template<typename T>
    inline void List<T>::GrowArray()
    {
        T* newElements = new T[m_capacity * 2];
        for (int i = 0; i < m_size; ++i)
        {
            newElements[i] = m_elements[i];
        }

        delete[] m_elements;
        m_elements = newElements;
        m_capacity *= 2;
    }

    template<typename T>
    inline ListIterator<T>::ListIterator(T* target)
    {
        m_target = target;
    }

    template<typename T>
    inline T& ListIterator<T>::operator*()
    {
        return *m_target;
    }

    template<typename T>
    inline ListIterator<T>& ListIterator<T>::operator++()
    {
        m_target++;
        return *this;
    }

    template<typename T>
    inline bool ListIterator<T>::operator==(const ListIterator& rhs)
    {
        return m_target == rhs.m_target;
    }

    template<typename T>
    inline bool ListIterator<T>::operator!=(const ListIterator& rhs)
    {
        return m_target != rhs.m_target;
    }

}