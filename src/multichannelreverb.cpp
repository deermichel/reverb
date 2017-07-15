#include "multichannelreverb.h"
#include <assert.h>
#include <iostream>

// creates a new multi-channel-reverb of given order
MultiChannelReverb::MultiChannelReverb(const int order, const int maxNumSamples, const int numChannels):
    mNumChannels(numChannels) {

  assert(numChannels > 0);
  mReverbs = new Reverb*[numChannels];
  for (int i = 0; i < numChannels; i++) {
    mReverbs[i] = new Reverb(order, maxNumSamples);
  }
}

// destructor
MultiChannelReverb::~MultiChannelReverb() {
  // cleanup
  for (int i = 0; i < mNumChannels; i++) {
    delete mReverbs[i];
  }
  delete[] mReverbs;
}

// process samples
void MultiChannelReverb::process(double **inputs, double **outputs, const int numSamples) {
  for (int i = 0; i < mNumChannels; i++) {
    mReverbs[i]->process(inputs[i], outputs[i], numSamples);

    if (i % 10000 == 0) {
      std::cout << (100.0 * i / mNumChannels) << "%" << std::endl;
    }
  }
}

// set amount of dry signal fed into delay lines
void MultiChannelReverb::setFeedDryIn(const double feedDryIn) {
  for (int i = 0; i < mNumChannels; i++) {
    mReverbs[i]->setFeedDryIn(feedDryIn);
  }
}

// set amount of wet signal fed from delay lines into main line
void MultiChannelReverb::setFeedWetOut(const double feedWetOut) {
  for (int i = 0; i < mNumChannels; i++) {
    mReverbs[i]->setFeedWetOut(feedWetOut);
  }
}

// set amount of dry signal fed into main line
void MultiChannelReverb::setDrySignalMix(const double drySignalMix) {
  for (int i = 0; i < mNumChannels; i++) {
    mReverbs[i]->setDrySignalMix(drySignalMix);
  }
}

// set mac delay tap time in ms
void MultiChannelReverb::setDelayTapTime(const int delayTime) {
  for (int i = 0; i < mNumChannels; i++) {
    mReverbs[i]->setDelayTapTime(delayTime);
  }
}

// set amount of decay of delay lines
void MultiChannelReverb::setDecay(const double decay) {
  for (int i = 0; i < mNumChannels; i++) {
    mReverbs[i]->setDecay(decay);
  }
}
