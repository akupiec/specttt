#ifndef CYCLICQUEUE_H
#define CYCLICQUEUE_H

#include <QQueue>
#include <QPair>

class QImage;

struct ImageData
{
    QImage *img;
    QPair<int,int> fftRange;
    int fftSamples;
};

template <typename T>
class CyclicQueue : public QQueue <T>
{
public:
    CyclicQueue() : QQueue<T>::QQueue() { maxLength_ = 3; }
    void enqueue(const T &t);
    int maxLength() const { return maxLength_; }
    void setMaxLength(int length) { maxLength_ = length; }

private:
    int maxLength_;
};

template <typename T>
void CyclicQueue<T>::enqueue(const T &t)
{
    if (this->length() == maxLength_)
        delete this->takeFirst().img;
    this->push_back(t);
}

#endif // CYCLICQUEUE_H
