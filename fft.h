#ifndef FFT_H
#define FFT_H

typedef unsigned short uint16;

namespace FFT {
    enum Window {
        Rectangular = 0,
        Hamming = 1,
        Hann = 2,
        Sine = 3,    // also known as Cosine
        Cosine = Sine
    };

    class FFT {
    public:
        FFT(uint16 bufferSize = 512, Window windowType = Hann);
        void countFFT(double* array);
        void makeWindow(double* array);

        // Acces to private fields
        uint16 bufferSize() const { return bufferFFT; }
        void setBufferSize(uint16);
        Window windowType() const { return windowFFT; }
        void setWindowType(Window);

    private:
        uint16 bufferFFT;
        Window windowFFT;

        // Window functions
        void hannWindow(double* array);
        void hammingWindow(double* array);
        void sineWindow(double* array);
    };
}

#endif // FFT_H
