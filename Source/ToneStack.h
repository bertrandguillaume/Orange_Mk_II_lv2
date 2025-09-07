/*
  ==============================================================================
    ToneStack.h
    Created: 31 Aug 2025 5:52:35pm
    Author:  bertrand GUILLAUME
  ==============================================================================
*/

#pragma once
#include <cstddef>
#include <array>
#include <cmath>
#include <algorithm>

class ToneStack
{
public:
    explicit ToneStack(double sampleRate = 48000.0);
    void setSampleRate(double fs);
    void updateCoefficients(float bassPos, float treblePos);
    void setPots(float bass, float treble);
    void resetState();    
    float processSample(float x);
    void processBlock(float* buffer, size_t numSamples);

private:
    double fs = 48000.0;
    std::array<double, 5> A; // coefficients feedforward
    std::array<double, 5> B; // coefficients feedback
    std::array<double, 4> xHist; // historique des entrées 
    std::array<double, 4> yHist; // historique des sorties

    float potB_ = 0.5, potT_ = 0.5;

};
