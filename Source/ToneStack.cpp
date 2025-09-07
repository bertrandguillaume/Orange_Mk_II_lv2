/*
  ==============================================================================

    ToneStack.cpp
    Created: 6 Sep 2025 5:52:44pm
    Author:  bertrand GUILLAUME
    based on https://www.ampbooks.com/mobile/dsp/tonestack/

  ==============================================================================
*/

#include "ToneStack.h"
#include <complex>
#include <iostream>

static constexpr double kPI = 3.14159265358979323846;

ToneStack::ToneStack(double sampleRate) {
    setSampleRate(sampleRate);
    A.fill(0.0);
    B.fill(0.0);
    xHist.fill(0.0);
    yHist.fill(0.0);
}
void ToneStack::setSampleRate(double newFs) {
    fs = newFs;
}

void ToneStack::resetState() {
    std::fill(xHist.begin(), xHist.end(), 0.0);
    std::fill(yHist.begin(), yHist.end(), 0.0);
}

// ---------------- Paramètres ----------------
void ToneStack::setPots(float bass, float treble) {
    potB_ = std::clamp(bass, 0.0f, 1.0f);
    potT_ = std::clamp(treble, 0.0f, 1.0f);
    updateCoefficients(potB_, potT_);
}

// ---------------- Traitement ----------------
float ToneStack::processSample(float x) {
    double xd = static_cast<double>(x);
    double yd =  B[0]*xd
              + B[1]*xHist[0]
              + B[2]*xHist[1]
              + B[3]*xHist[2]
              + B[4]*xHist[3]
              - A[1]*yHist[0]
              - A[2]*yHist[1]
              - A[3]*yHist[2]
              - A[4]*yHist[3];

    // Décaler
   for (int i = 3; i > 0; --i) {
        xHist[i] = xHist[i-1];
        yHist[i] = yHist[i-1];
    }
    xHist[0] = xd;
    yHist[0] = yd;
    return static_cast<float>(yd);
}
void ToneStack::processBlock(float* buffer, size_t numSamples) {
    for (size_t n = 0; n < numSamples; ++n) {
        buffer[n] = processSample(buffer[n]);
    }
}

void ToneStack::updateCoefficients(float bassPos, float treblePos) {
// === Étape 1 : calcul des coefficients analogiques ===
    double a0 = 9.34e10;
    double a1 = (-2.975e9)*bassPos*bassPos + (3.251e9)*bassPos + (7.948e8)*treblePos + 2.934e8;
    double a2 = 2.344e5
              - (7.761e6)*bassPos*bassPos
              + (1.885e7)*bassPos*treblePos
              + (8.434e6)*bassPos
              + (1.593e6)*treblePos
              - (1.403e6)*treblePos*treblePos
              - (1.714e7)*bassPos*bassPos*treblePos;
    double a3 = (-33269.0)*bassPos*treblePos*treblePos + (5667.0)*bassPos
              + (37452.0)*bassPos*treblePos
              - (5311.0)*bassPos*bassPos
              + (335.3)*(treblePos - treblePos*treblePos)
              - (34433.0)*bassPos*bassPos*treblePos
              + (30250.0)*bassPos*bassPos*treblePos*treblePos
              + 39.6;
    double a4 = (7.381)*(bassPos*treblePos + bassPos*bassPos*treblePos*treblePos
              - bassPos*treblePos*treblePos - bassPos*bassPos*treblePos)
              + 0.8712*(bassPos - bassPos*bassPos);

    double b0 = (8.333e10)*bassPos + 1.833e9;
    double b1 = (7.083e8)*bassPos*treblePos - (3.083e8)*bassPos*bassPos + (4.794e8)*bassPos
              + (1.558e7)*treblePos + 2.383e7;
    double b2 = 844320.0*bassPos - (2.808e6)*bassPos*bassPos*treblePos + (232280.0)*treblePos
              + (4.464e6)*bassPos*treblePos - (754230.0)*bassPos*bassPos
              - (1.25e6)*bassPos*treblePos*treblePos
              - 27500.0*treblePos*treblePos + 10010.0;
    double b3 = 220.2*(bassPos - bassPos*bassPos) + (8310.0)*bassPos*treblePos
              - (7409.0)*bassPos*bassPos*treblePos + (100.1)*treblePos
              + (2750.0)*bassPos*bassPos*treblePos*treblePos
              - (60.5)*treblePos*treblePos
              - (3294.5)*bassPos*treblePos*treblePos;
    double b4 = 2.202*(bassPos*treblePos - bassPos*bassPos*treblePos)
              + 1.331*(bassPos*bassPos*treblePos*treblePos - bassPos*treblePos*treblePos);

    // === Étape 2 : calcul des coefficients numériques - transformation bilinéaire ===
    double A0 = a0 + (96000.0)*a1 + (9.216e9)*a2 + (8.84736e14)*a3 + (8.4934656e19)*a4;
    double A1 = 4*a0 + (192000.0)*a1 - (1.769472e15)*a3 - (3.39738624e20)*a4;
    double A2 = 6*a0 - (1.8432e10)*a2 + (5.09607936e20)*a4;
    double A3 = 4*a0 - (192000.0)*a1 + (1.769472e15)*a3 - (3.39738624e20)*a4;
    double A4 = a0 - (96000.0)*a1 + (9.216e9)*a2 - (8.84736e14)*a3 + (8.4934656e19)*a4;

    double B0 = b0 + (96000.0)*b1 + (9.216e9)*b2 + (8.84736e14)*b3 + (8.4934656e19)*b4;
    double B1 = 4*b0 + (192000.0)*b1 - (1.769472e15)*b3 - (3.39738624e20)*b4;
    double B2 = 6*b0 - (1.8432e10)*b2 + (5.09607936e20)*b4;
    double B3 = 4*b0 - (192000.0)*b1 + (1.769472e15)*b3 - (3.39738624e20)*b4;
    double B4 = b0 - (96000.0)*b1 + (9.216e9)*b2 - (8.84736e14)*b3 + (8.4934656e19)*b4;

    // === Étape 3 : normalisation pour filtre IIR ===
    if (A0 == 0.0 || std::isnan(A0) || std::isinf(A0))
        A0 = 1e-12; // éviter les crashs
    A[0] = 1.0;      // pas utilisé
    A[1] = A1 / A0;
    A[2] = A2 / A0;
    A[3] = A3 / A0;
    A[4] = A4 / A0;

    B[0] = B0 / A0;             
    B[1] = B1 / A0;
    B[2] = B2 / A0;
    B[3] = B3 / A0;
    B[4] = B4 / A0;
}