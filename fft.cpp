#include "fft.h"
#include "ffft/FFTReal.h"
#include <cmath>
#include <iostream>

using namespace FFT;

FFT::FFT::FFT(uint16 bufferSize, Window windowType) {
    bufferFFT = bufferSize;
    windowFFT = windowType;
}

void FFT::FFT::countFFT(double *array) {
    ffft::FFTReal<double> fft (bufferFFT);
    double* tempArray = new double[bufferFFT];
    fft.do_fft(tempArray,array);
    uint16 halfBuffer = bufferFFT/2;
    double* x = tempArray;
    double* y = x + halfBuffer;
    for (uint16 i=0; i<bufferFFT; i++) {
        if (i < halfBuffer) {
            array[i] = sqrt( (*x)*(*x) + (*y)*(*y) );
            x++;
            y++;
        }
        else
            array[i] = 0;
    }
    delete [] tempArray;
}

void FFT::FFT::makeWindow(double *array) {
    switch (windowFFT) {
    case Rectangular: // eatch element of array multiply by 1 so to nothing
        return;
    case Hann: hannWindow(array);
        break;
    case Hamming: hammingWindow(array);
        break;
    case Sine: sineWindow(array);
        break;
    default: std::cerr << "makeWindow: Invalid window type";
        break;
    }
}

void FFT::FFT::hannWindow(double *array) {
    double multiplier;
    double _2pi = 2 * M_PI;
    uint16 x = bufferFFT - 1;
    for (uint16 i=0; i<bufferFFT; i++) {
        multiplier = 1 - cos ( (_2pi*i) / x );
        multiplier *= 0.5;
        array[i] *= multiplier;
    }
}

void FFT::FFT::hammingWindow(double *array) {
    double _2pi = 2 * M_PI;
    uint16 x = bufferFFT - 1;
    for (uint16 i=0; i<bufferFFT; i++)
        array[i] *= 0.54 + 0.46 * cos ( (_2pi*i) / x );
}

void FFT::FFT::sineWindow(double *array) {
    uint16 x = bufferFFT - 1;
    for (uint16 i=0; i<bufferFFT; i++)
        array[i] *= sin(M_PI*i/x);
}

void FFT::FFT::setBufferSize(uint16 v) {
    bufferFFT = v;
}

void FFT::FFT::setWindowType(Window v) {
    windowFFT = v;
}
