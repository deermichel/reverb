#ifndef __REVERB__
#define __REVERB__

#include "IPlug_include_in_plug_hdr.h"
#include "multichannelreverb.h"

class ReverbPlugin : public IPlug
{
public:
  ReverbPlugin(IPlugInstanceInfo instanceInfo);
  ~ReverbPlugin();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void ReinitEngine();

private:
  MultiChannelReverb *mReverb;
  int mProcessingLock;
};

#endif
