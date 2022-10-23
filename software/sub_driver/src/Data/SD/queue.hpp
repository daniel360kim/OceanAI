/**
 * @file queue.hpp
 * @author Daniel Kim
 * @brief Simple, dynamically allocated queue in FIFO order
 * @version 0.1
 * @date 2022-10-22
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */
#ifndef RingBuffer_h
#define RingBuffer_h

#include <memory>
#include <cstddef>

template <typename T>
class CircularBuffer
{
public:
    explicit CircularBuffer(std::size_t size) : m_buffer(std::unique_ptr<T[]>(new T[size])), m_maxsize(size) {}
    
    void insert(T item);
    T get();
	void get(T& item); // get item by reference
    void reset();
    bool empty() const;
    bool full() const;
    std::size_t capacity() const;
    std::size_t size() const;

private:
	std::unique_ptr<T[]> m_buffer;
	std::size_t m_head = 0;
	std::size_t m_tail = 0;
	const std::size_t m_maxsize;
	bool m_full = 0;
};

/**
 * @brief Reset/clear the queue
 * 
 * @tparam T data type of the queue
 */
template <typename T>
void CircularBuffer<T>::reset()
{
    m_head = m_tail;
    m_full = false;
}

/**
 * @brief Check if the queue is empty
 * 
 * @tparam T data type of the queue
 * @return true queue is empty
 * @return false queue is not empty
 */
template<typename T>
bool CircularBuffer<T>::empty() const
{
	//if head and tail are equal, we are empty
	return (!m_full && (m_head == m_tail));
}

/**
 * @brief Check if the queue is full
 * 
 * @tparam T data type of the queue
 * @return true queue is full
 * @return false queue is not full
 */
template<typename T>
bool CircularBuffer<T>::full() const
{
	//If tail is ahead the head by 1, we are full
	return m_full;
}

/**
 * @brief Get the capacity of the queue
 * 
 * @tparam T data type of the queue
 * @return std::size_t size of the queue
 */
template<typename T>
std::size_t CircularBuffer<T>::capacity() const
{
	return m_maxsize;
}

/**
 * @brief Get the size of the queue (number of elements)
 * 
 * @tparam T data type of the queue
 * @return std::size_t number of elements
 */
template<typename T>
std::size_t CircularBuffer<T>::size() const
{
	size_t size = m_maxsize;

	if(!m_full)
	{
		if(m_head >= m_tail)
		{
			size = m_head - m_tail;
		}
		else
		{
			size = m_maxsize + m_head - m_tail;
		}
	}

	return size;
}

/**
 * @brief insert an element into the queue
 * 
 * @tparam T data type of the queue
 * @param item the data we want to insert
 */
template <typename T>
void CircularBuffer<T>::insert(T item)
{
	m_buffer[m_head] = item;

	if(m_full)
	{
		m_tail = (m_tail + 1) % m_maxsize;
	}

	m_head = (m_head + 1) % m_maxsize;

	m_full = m_head == m_tail;
}

/**
 * @brief get an element from the queue, automatically pops
 * 
 * @tparam T data type of the queue
 * @return T return of the data
 */
template <typename T>
T CircularBuffer<T>::get()
{
	if(empty())
	{
		return T();
	}

	//Read data and advance the tail (we now have a free space)
	auto val = m_buffer[m_tail];
	m_full = false;
	m_tail = (m_tail + 1) % m_maxsize;

	return val;
}

/**
 * @brief get an element from the queue, automatically pops
 * Returned by reference: used for large data types
 * 
 * @tparam T data type of the queue
 * @param item reference to the data that was popped
 */
template <typename T>
void CircularBuffer<T>::get(T& item)
{
	if(empty())
	{
		return;
	}

	//Read data and advance the tail (we now have a free space)
	item = m_buffer[m_tail];
	m_full = false;
	m_tail = (m_tail + 1) % m_maxsize;
}



#endif