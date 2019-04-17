//<LICENCE>

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

namespace kipl { namespace containers {
template <typename T>
class RingBuffer
{
public:
        RingBuffer(size_t len) : L(len), index(0), isFull(false),buffer(nullptr) {buffer=new T[L];}
        ~RingBuffer() {delete [] buffer;}
        void Reset() {index=0; isFull=false;}
        T const * const DataPtr() {return buffer;}
        size_t Push(T val) {
                buffer[index]=val;
                index++;
                if (L<=index) {
                        index=0;
                        isFull=true;
                }
                return index;
        }
        bool IsFull() {return isFull;}
        size_t Size() {return L;}
private:
        const size_t L;
        size_t index;
        bool isFull;
        T *buffer;
};

}}
#endif // RINGBUFFER_H

