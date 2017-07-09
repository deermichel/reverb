#ifndef REVERB_H
#define REVERB_H

#include "circularbuffer.h"

// reverb implementation based of Jot's Reverberator
class Reverb {

public:
  // creates a new reverb of given order (power of 2)
  Reverb(const int order, const int maxNumSamples);
  ~Reverb(); // destructor

  // process samples
  void process(double *input, double *output, const int numSamples);

  // set amount of dry signal fed into delay lines
  void setFeedDryIn(const double feedDryIn);

  // set amount of wet signal fed from delay lines into main line
  void setFeedWetOut(const double feedWetOut);

  // set amount of dry signal fed into main line
  void setDrySignalMix(const double drySignalMix);

  // set mac delay tap time in ms
  void setDelayTapTime(const int delayTime);

  // set amount of decay of delay lines
  void setDecay(const double decay);

private:
  // input buffer holding the last and current input samples
  CircularBuffer *mInputBuffer;

  // delay buffers holding the last and current calculated samples of delay lines
  CircularBuffer **mDelayBuffer;

  // feedback matrix used for reverb
  double **mFeedbackMatrix;

  // amount of dry signal fed into delay lines
  double *mFeedDryIn;

  // amount of wet signal fed from delay lines into main line
  double *mFeedWetOut;

  // amount of dry signal fed into main line
  double mDrySignalMix;

  // delay in ms of delay lines
  int *mDelayTaps;

  // amount of decay of delay lines
  double *mDecay;

  // order of reverb (number of delay lines)
  const int mOrder;

  // max number of samples passed for processing
  const int mMaxNumSamples;

  // --- private funcs

  // process z-th sample in main line
  double processMainLine(const int z);

  // process z-th sample in n-th delay line
  double processDelayLine(const int z, const int n);

};

#endif
