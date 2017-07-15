## reverb

As a music fan, I always wanted to implement my own reverb. Here it is - experimental as usual :P.
It is a basic variant of the
[Jot's FDN Late Reverberator](https://ccrma.stanford.edu/realsimple/Reverb/Jot_s_FDN_Late_Reverberators.html)
using a [Hadamard matrix](https://en.wikipedia.org/wiki/Hadamard_matrix) as feedback matrix. Depending on how you choose
the parameters and matrix order, you can get a decent reverb out of it. But if you have any ideas for an even better
result, feel free to contribute.

![Jot's FDN Late Reverberator](https://ccrma.stanford.edu/realsimple/Reverb/img63.png)

### Parameters

    pattern: [param name] ([label in schematic])     // [description]

    feedDryIn     (b)     // amount of dry signal that is fed into the delay lines (>= 0)
    feedDryOut    (c)     // amount of wet signal that is fed back into the main line (>= 0)
    drySignalMix  (d)     // amount of dry signal that is fed into the main line (>= 0)
    delayTapTime  (M)     // the maximum delay tap time in ms (> 0)
    decay         (g)     // amount of decay of the delay lines (>= 0 && < 1)

### Components

* [CircularBuffer](https://github.com/DeerMichel/reverb/blob/master/include/circularbuffer.h) - cyclic array
  implementation used as buffer
* [Reverb](https://github.com/DeerMichel/reverb/blob/master/include/reverb.h) - main part doing the algorithm work
  and holding all parameters
* [MultiChannelReverb](https://github.com/DeerMichel/reverb/blob/master/include/multichannelreverb.h) - helper class
  to create and manage multiple reverb instances at once (e.g. for stereo application)

### Fun stuff

One day, I came up with the idea of putting a video through the reverberator. With OpenCV, it is easy (in C++ ^^) to
extract the pixels of all frames of a video and so, the raw numbers are treated as an ordinary audio signal.
Experimenting a bit with the parameters, it gets a "trippy" experience. A demo usage can be found in the 'video' branch.
Get creative too - I'm looking forward for more things being reverbed :D.

### Building

    // make sure to have cmake and libsndfile (if you want to use the demo main.cpp) installed
    // on Mac, this can be simply done via Homebrew: brew install cmake libsndfile

    git clone https://github.com/DeerMichel/reverb/
    cd reverb
    mkdir bin
    cd bin
    cmake ..
    make
    cp ../misc/input.wav .    // or use your own audio file
    ./ReverbMain
    afplay output.wav         // or equivalent

For a sample usage, take a look at the [main.cpp](https://github.com/DeerMichel/reverb/blob/master/src/main.cpp).

### Missing features

* Better control over all parameters (to achieve a nicer overall sound)
* Spreading multichannel reverb
* Consistent DAW plugin integration (or a GUI at all)
* Tests (right, just kidding)
* And much more... sure...

### Third-party

* [libsndfile](http://www.mega-nerd.com/libsndfile/) (LGPL) - optional, only used in demo main.cpp
* [OpenCV](http://opencv.org/) (BSD) - optional, only used in video demo main.cpp

_ps: input.wav is a quick and dirty production by myself - no need to worry about licensing as long as it is used
in the context of this project._
