#ifndef StaticQueue_h
#define StaticQueue_h

#include <stddef.h>
#include <array>

template<typename T, size_t N>
class StaticCircularBuffer
{
public:
    StaticCircularBuffer() {}

    void insert(T item);
    T get();
    void reset();
    bool empty() const;
    bool is_full() const;
    size_t capacity() const;
    size_t size() const;

private:
    std::array<T, N> buf; 
    size_t head_ = 0;
    size_t tail_ = 0;
    bool full;

};

template<typename T, size_t N>
void StaticCircularBuffer<T, N>::insert(T item)
{
    buf[head_] = item;
    head_ = (head_ + 1) % N;
    if (head_ == tail_)
    {
        full = true;
    }
}

template<typename T, size_t N>
T StaticCircularBuffer<T, N>::get()
{
    T item = buf[tail_];
    tail_ = (tail_ + 1) % N;
    full = false;
    return item;
}

template<typename T, size_t N>
void StaticCircularBuffer<T, N>::reset()
{
    head_ = tail_;
    full = false;
}

template<typename T, size_t N>
bool StaticCircularBuffer<T, N>::empty() const
{
    //if head and tail are equal, we are empty
    return (!full && (head_ == tail_));
}

template<typename T, size_t N>
bool StaticCircularBuffer<T, N>::is_full() const
{
    //If tail is ahead the head by 1, we are full
    return full;
}

template<typename T, size_t N>
size_t StaticCircularBuffer<T, N>::capacity() const
{
    return N;
}

template<typename T, size_t N>
size_t StaticCircularBuffer<T, N>::size() const
{
    size_t size = N;

    if (!full)
    {
        if (head_ >= tail_)
        {
            size = head_ - tail_;
        }
        else
        {
            size = N + head_ - tail_;
        }
    }
    return size;
}


#endif