#include "reverb.h"
#include <assert.h>
#include <math.h>

// helper method for generating hadamard matrices
// TODO: better place for this ;)
void generateHadamardMatrix(double **result, const int order, const double value) {
  result[0][0] = value; // set initial value

  // expand recursively according to hadamard rules
  for (int n = 2; n <= order; n *= 2) {
    for (int i = 0; i < n / 2; i++) {
      for (int j = 0; j < n / 2; j++) {
        result[i][j + n / 2] = result[i][j];
        result[i + n / 2][j] = result[i][j];
        result[i + n / 2][j + n / 2] = -result[i][j];
      }
    }
  }
}


// creates a new reverb of given order (power of 2)
Reverb::Reverb(const int order, const int maxNumSamples):
    mOrder(order), mMaxNumSamples(maxNumSamples) {

  assert((order & (order - 1)) == 0); // order -> power of two
  assert(maxNumSamples >= 0);

  // set default parameters (and dynamic allocation)
  mInputBuffer = new CircularBuffer(maxNumSamples);
  mDelayBuffer = new CircularBuffer*[order];
  mFeedbackMatrix = new double*[order];
  mFeedDryIn = new double[order]();
  mFeedWetOut = new double[order]();
  mDrySignalMix = 1.0;
  mDelayTaps = new int[order]();
  mDecay = new double[order]();
  for (int i = 0; i < order; i++) {
    mDelayBuffer[i] = new CircularBuffer(maxNumSamples);
    mFeedbackMatrix[i] = new double[order]();
  }

  // feedback matrix is a hadamard matrix (for now)
  generateHadamardMatrix(mFeedbackMatrix, order, 1.0 / sqrt(order));
}

// destructor
Reverb::~Reverb() {
  // cleanup
  for (int i = 0; i < mOrder; i++) {
    delete mDelayBuffer[i];
    delete[] mFeedbackMatrix[i];
  }
  delete mInputBuffer;
  delete[] mDelayBuffer;
  delete[] mFeedbackMatrix;
  delete[] mFeedDryIn;
  delete[] mFeedWetOut;
  delete[] mDelayTaps;
  delete[] mDecay;
}

// process samples
void Reverb::process(double *input, double *output, const int numSamples) {
  assert(numSamples >= 0);

  // update buffers
  mInputBuffer->insert(input, numSamples);
  for (int n = 0; n < mOrder; n++) {
    (*mDelayBuffer[n]).shift(numSamples);
  }

  // process samples
  for (int z = 0; z < numSamples; z++) {
    output[z] = processMainLine(z);
  }
}

// set amount of dry signal fed into delay lines
void Reverb::setFeedDryIn(const double feedDryIn) {
  assert(feedDryIn >= 0);
  // TODO: consider distribute differently over lines?
  for (int i = 0; i < mOrder; i++) {
    mFeedDryIn[i] = feedDryIn;
  }
}

// set amount of wet signal fed from delay lines into main line
void Reverb::setFeedWetOut(const double feedWetOut) {
  assert(feedWetOut >= 0);
  // TODO: consider distribute differently over lines?
  for (int i = 0; i < mOrder; i++) {
    mFeedWetOut[i] = feedWetOut;
  }
}

// set amount of dry signal fed into main line
void Reverb::setDrySignalMix(const double drySignalMix) {
  assert(drySignalMix >= 0);
  mDrySignalMix = drySignalMix;
}

// set max delay tap time in ms
void Reverb::setDelayTapTime(const int delayTime) {
  assert(delayTime > 0);

  // distribute delay taps exponentially over time
  double distribution = log(delayTime) / mOrder;
  for (int i = 0; i < mOrder; i++) {
    mDelayTaps[i] = exp(distribution * (i + 1));
  }

  // update buffer sizes
  // TODO: no need for reconstruction -> variable buffer size
  delete mInputBuffer;
  mInputBuffer = new CircularBuffer(mMaxNumSamples + delayTime);
  for (int i = 0; i < mOrder; i++) {
    delete mDelayBuffer[i];
    mDelayBuffer[i] = new CircularBuffer(mMaxNumSamples + delayTime);
  }
}

// set amount of decay of delay lines
void Reverb::setDecay(const double decay) {
  assert(decay >= 0 && decay < 1);
  // TODO: consider distribute differently over lines?
  for (int i = 0; i < mOrder; i++) {
    mDecay[i] = decay;
  }
}

// --- private funcs

// process z-th sample in main line
double Reverb::processMainLine(const int z) {
  double result = mDrySignalMix * (*mInputBuffer)[z];
  for (int n = 0; n < mOrder; n++) {
    result += mFeedWetOut[n] * processDelayLine(z, n);
  }
  return result;
}

// process z-th sample in n-th delay line
double Reverb::processDelayLine(const int z, const int n) {
  double result = mFeedDryIn[n] * (*mInputBuffer)[z - mDelayTaps[n]];
  for (int i = 0; i < mOrder; i++) {
    result += mFeedbackMatrix[n][i] * (*mDelayBuffer[i])[z - mDelayTaps[n]] * mDecay[i];
  }
  (*mDelayBuffer[n])[z] = result;
  return result;
}
