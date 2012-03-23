#include "cyclicqueue.h"

//template <typename T>
//CyclicQueue<T>::CyclicQueue() : QQueue() {}

template <typename T>
void CyclicQueue<T>::enqueue(const T &t)
{
    if (this->length() == maxLength_)
        this->removeFirst();
    QQueue<T>::enqueue(t);
}
