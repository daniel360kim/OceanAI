



#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <memory>

template <typename T>
class CircularBuffer
{
public:
    CircularBuffer(std::size_t size) : m_buffer(std::unique_ptr<T[]>(new T[size])), m_max_size(size) {}

    void insert(T item);
    T get();
    void get(T& item);

    void reset();
    bool empty() const;
    bool full() const;

    std::size_t capacity() const;
    std::size_t size() const;

private:
    std::unique_ptr<T[]> m_buffer;
    std::size_t m_head = 0;
    std::size_t m_tail = 0;

    const std::size_t m_max_size;

    bool m_full;
};

template <typename T>
void CircularBuffer<T>::insert(T item)
{
    m_buffer[m_head] = item;

    if(m_full)
    {
        m_tail = (m_tail + 1) % m_max_size;
    }

    m_head = (m_head + 1) % m_max_size;
    m_full = m_head == m_tail;
}

template <typename T>
T CircularBuffer<T>::get()
{
    if(empty())
    {
        return T();
    }
    T val = m_buffer[m_tail];
    m_full = false;
    m_tail = (m_tail + 1) % m_max_size;

    return val;
}

template <typename T>
void CircularBuffer<T>::get(T& item)
{
    if(empty())
    {
        return;
    }

    item = m_buffer[m_tail];
    m_full = false;
    m_tail = (m_tail + 1) % m_max_size;
}









#endif