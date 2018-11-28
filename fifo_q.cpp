/*******
Code from StackOverflow 
https://codereview.stackexchange.com/questions/171360/thread-safe-bounded-buffer-fifo-queue-using-c11
******/

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

// T must implement operator << 

template<typename T>
class BoundedBuffer
{
public:
    BoundedBuffer(size_t size) :m_size(size), m_front(0), m_rear(-1), m_count(0)
    {
        m_queue = new T[m_size];
    }

    void enqueue( const T& obj )
    {
        std::unique_lock<std::mutex> writerLock(m_mutex);

        while ( m_size == m_count )
        {
            writers.wait(writerLock);
        }
        m_rear = (m_rear + 1) % m_size;
        m_queue[m_rear] = obj;
        ++m_count;
        std::cout << std::this_thread::get_id() << " has Enqueued Object =" << obj << std::endl;
        readers.notify_all();
    }

    void dequeue( T& obj )
    {
        std::unique_lock<std::mutex> readerLock(m_mutex);
        while (0 == m_count)
        {
            readers.wait(readerLock);
        }
        obj = m_queue[m_front];
        m_front = (m_front + 1) % m_size;
        --m_count;
        std::cout << std::this_thread::get_id() << " has Dequeued Object =" << obj << std::endl;
        writers.notify_all();
    }

    ~BoundedBuffer()
    {
        if (m_queue)
        {
            delete[] m_queue;
        }
    }

private:
    size_t m_size;
    T * m_queue;
    int m_front, m_rear, m_count;
    std::mutex m_mutex;
    std::condition_variable readers;
    std::condition_variable writers;
};

/***************************
* Use BoundedBuffer struct in producer and consumer functions
***************************/

void consumer(int id, BoundedBuffer<int>& buffer){
    for (int i = 0; i < 50; ++i)
    {
        int value;
        buffer.dequeue(value);
        std::cout << "Consumer " << id << " fetched" << value << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

void producer(int id, BoundedBuffer<int>& buffer){
    for (int i = 0; i < 75; ++i)
    {
        buffer.enqueue(i);
        std::cout << "Produced " << id << " produced" << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    }
}

int main (){
    BoundedBuffer<int> buffer(200);

    std::thread c1(consumer, 0, std::ref(buffer));
    std::thread c2(consumer, 1, std::ref(buffer));
    std::thread c3(consumer, 2, std::ref(buffer));
    std::thread p1(producer, 0, std::ref(buffer));
    std::thread p2(producer, 1, std::ref(buffer));

    c1.join();
    c2.join();
    c3.join();
    p1.join();
    p2.join();

    return 0;
}