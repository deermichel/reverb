#ifndef MULTICHANNELREVERB_H
#define MULTICHANNELREVERB_H

#include "reverb.h"

// multi-channel-reverberation by using multiple reverb instances
// TODO: use a spacy fancy matrix to spill sound chunks across the channels/universe
class MultiChannelReverb {

public:
  // creates a new multi-channel-reverb of given order
  MultiChannelReverb(const int order, const int maxNumSamples, const int numChannels);
  ~MultiChannelReverb(); // destructor

  // process samples
  void process(double **inputs, double **outputs, const int numSamples);

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
  // reverb instances
  Reverb **mReverbs;

  // number of channels
  const int mNumChannels;

};

#endif
