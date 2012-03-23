#ifndef CYCLICQUEUE_H
#define CYCLICQUEUE_H

#include <QQueue>

template <typename T>
class CyclicQueue : public QQueue <T>
{
public:
//    CyclicQueue();
    void enqueue(const T &t);
    int maxLength() const { return maxLength_; }
    void setMaxLength(int length) { maxLength_ = length; }

private:
    int maxLength_;
};

#endif // CYCLICQUEUE_H
