/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef __GCANVAS_ARRAY_H__
#define __GCANVAS_ARRAY_H__

#include <cstdlib>

#ifdef DEBUG
void DLog(const char *format, ...);
#ifdef __ANDROID__
#define ASSERT(x)                                                              \
    {                                                                          \
        if (!(x))                                                              \
        {                                                                      \
            gErrorFlag = true;                                                 \
            DLog("ASSERT %s:%d %s", __FILE__, __LINE__, #x);                   \
        }                                                                      \
    }
#else
#include <assert.h>
#define ASSERT(x)                                                              \
    {                                                                          \
        assert(x);                                                             \
    }
#endif
#else
#define ASSERT(x)                                                              \
    {                                                                          \
    }
#endif

template < class T > class DynArray
{
public:
    DynArray(unsigned int initialSize = (unsigned int)kInitialMemSize);
    virtual ~DynArray();

    const T &operator[](int i) const
    {
        ASSERT(i < m_size);
        ASSERT(m_size <= m_allocatedSize);
        return m_entries[i];
    }
    T &operator[](int i)
    {
        ASSERT(i < m_size);
        return m_entries[i];
    }

    int GetSize() const
    {
        ASSERT(m_size <= m_allocatedSize);
        return m_size;
    }
    void SetSize(int size);
    bool IsEmpty() const { return (m_size == 0); }

    void Append(const T *entries, int len);
    void RemoveAt(int index);

    T *GetData() { return m_entries; }
    const T *GetData() const { return m_entries; }

private:
    DynArray(const DynArray &that);            // private, undefined
    DynArray &operator=(const DynArray &that); // private, undefined

    enum
    {
        kInitialMemSize = 8
    };
    T *m_entries;
    int m_size;
    int m_allocatedSize;
};

template < class T > DynArray< T >::DynArray(unsigned int initialSize)
{
    m_size = m_allocatedSize = 0;
    m_entries = NULL;
    SetSize(initialSize);
    SetSize(0);
}

template < class T > DynArray< T >::~DynArray()
{
    if (m_entries)
    {
        free(m_entries);
        m_entries = NULL;
        m_size = 0;
        m_allocatedSize = 0;
    }
}

template < class T > void DynArray< T >::Append(const T *entries, int len)
{
    if (entries && len)
    {
        unsigned int base = m_size;
        SetSize(m_size + len);
        memcpy(m_entries + base, entries, len * sizeof(T));
    }
}

template < class T > void DynArray< T >::RemoveAt(int index)
{
    ASSERT(index < m_size);
    if (index < (m_size - 1))
    {
        memmove(&m_entries[index], &m_entries[index + 1],
                (m_size - (index + 1)) * sizeof(T));
    }
    m_size--;
}

template < class T > void DynArray< T >::SetSize(int size)
{
    ASSERT(size >= 0);
    if (size > m_allocatedSize)
    {
        // Really need to guarantee power of 2 for VBOs.
        int newSize = 16;
        while (newSize < size)
        {
            newSize *= 2;
        }
        // m_entries = (T *) realloc (m_entries, newSize * sizeof(T));
        // ASSERT(m_entries);
        T *newEntries = (T *)realloc(m_entries, newSize * sizeof(T));
        if (newEntries)
        {
            m_entries = newEntries;
        }
        else
        { // could not realloc, but orig still valid
            ASSERT(newEntries);
        }

        memset(m_entries + m_allocatedSize, 0,
               (newSize - m_allocatedSize) * sizeof(T));
        m_allocatedSize = newSize;
    }

    m_size = size;
    ASSERT(m_size < 10 * 1000); // sanity check.
}

#endif
