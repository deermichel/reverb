#include "ReverbPlugin.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kOrder = 0,
  kFeedDryIn,
  kFeedWetOut,
  kDrySignalMix,
  kDelayTapTime,
  kDecay,
  kNumParams
};

ReverbPlugin::ReverbPlugin(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mProcessingLock(0)
{
  TRACE;
  
  mReverb = new MultiChannelReverb(4, 4096, 2);

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kOrder)->InitInt("Order", 2, 0, 5);
  GetParam(kFeedDryIn)->InitDouble("Feed Dry In", 0.2, 0, 1, 0.01, "%");
  GetParam(kFeedDryIn)->SetShape(2.);
  GetParam(kFeedWetOut)->InitDouble("Feed Wet Out", 0.2, 0, 1, 0.01, "%");
  GetParam(kFeedWetOut)->SetShape(2.);
  GetParam(kDrySignalMix)->InitDouble("Dry Signal Mix", 0.6, 0, 1, 0.01, "%");
  GetParam(kDrySignalMix)->SetShape(2.);
  GetParam(kDelayTapTime)->InitInt("Delay Tap Time", 6000, 1, 16384, "samples");
  GetParam(kDecay)->InitDouble("Decay", 0.95, 0, 0.9999, 0.01, "%");

  //MakePreset("preset 1", ... );
  //MakeDefaultPreset((char *) "-", kNumPrograms);
}

ReverbPlugin::~ReverbPlugin() {
  delete mReverb;
}

void ReverbPlugin::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  
  if (mProcessingLock == 0) {
    mProcessingLock = 1;
    mReverb->process(inputs, outputs, nFrames);
    mProcessingLock = 0;
  }
}

void ReverbPlugin::Reset()
{
  TRACE;
  IMutexLock lock(this);
  ReinitEngine();
}

void ReverbPlugin::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kOrder: {
      ReinitEngine();
      break;
    }
  
    case kFeedDryIn: {
      mReverb->setFeedDryIn(GetParam(kFeedDryIn)->Value());
      break;
    }
      
    case kFeedWetOut: {
      mReverb->setFeedWetOut(GetParam(kFeedWetOut)->Value());
      break;
    }
      
    case kDrySignalMix: {
      mReverb->setDrySignalMix(GetParam(kDrySignalMix)->Value());
      break;
    }
      
    case kDelayTapTime: {
      while (mProcessingLock == 1); // wait for release
      mProcessingLock = 1; // lock
      mReverb->setDelayTapTime(GetParam(kDelayTapTime)->Value());
      mProcessingLock = 0; // release
      break;
    }
      
    case kDecay: {
      mReverb->setDecay(GetParam(kDecay)->Value());
      break;
    }

    default:
      break;
  }
}

void ReverbPlugin::ReinitEngine()
{
  while (mProcessingLock == 1); // wait for release
  mProcessingLock = 1; // lock
  
  delete mReverb;
  mReverb = new MultiChannelReverb(pow(2, (int) GetParam(kOrder)->Value()), GetBlockSize(), 2);
  
  // set params
  mReverb->setFeedDryIn(GetParam(kFeedDryIn)->Value());
  mReverb->setFeedWetOut(GetParam(kFeedWetOut)->Value());
  mReverb->setDrySignalMix(GetParam(kDrySignalMix)->Value());
  mReverb->setDelayTapTime(GetParam(kDelayTapTime)->Value());
  mReverb->setDecay(GetParam(kDecay)->Value());
  
  mProcessingLock = 0; // release
}
