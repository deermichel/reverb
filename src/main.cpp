#include <iostream>
#include <math.h>
#include <sndfile.h>
#include "multichannelreverb.h"

// forward-declarations
double** readWavFile(const char *fileName, SF_INFO &info);
void saveWavFile(const char *fileName, double **frames, SF_INFO &info);


// entry point
int main(int argc, char *argv[]) {

	// init
	SF_INFO fileInfo;
	double **inputs = readWavFile("input.wav", fileInfo);
	double **outputs = new double*[fileInfo.channels];
	for (int c = 0; c < fileInfo.channels; c++) {
		outputs[c] = new double[fileInfo.frames]();
	}

	// setup reverb
	MultiChannelReverb reverb(8, fileInfo.frames, fileInfo.channels);
	reverb.setFeedDryIn(0.2);
	reverb.setFeedWetOut(0.2);
	reverb.setDrySignalMix(0.7);
	reverb.setDelayTapTime(7012);
	reverb.setDecay(0.95);

	// process
	std::cout << "Processing..." << std::endl;
	reverb.process(inputs, outputs, fileInfo.frames);
	std::cout << "Done." << std::endl;

	// save results
	saveWavFile("output.wav", outputs, fileInfo);

	// cleanup
	for (int c = 0; c < fileInfo.channels; c++) {
		delete[] inputs[c];
		delete[] outputs[c];
	}
	delete[] inputs;
	delete[] outputs;
}

// reads a wave file
double** readWavFile(const char *fileName, SF_INFO &info) {
	// open file
	SNDFILE *file = sf_open(fileName, SFM_READ, &info);
	if (sf_error(file)) {
		std::cout << sf_strerror(file) << std::endl;
	}

	// read frames
	double *mergedFrames = new double[info.frames * info.channels]();
	sf_read_double(file, mergedFrames, info.frames * info.channels);

	// split channels
	double **frames = new double*[info.channels];
	for (int c = 0; c < info.channels; c++) {
		frames[c] = new double[info.frames]();
		for (int f = 0; f < info.frames; f++) {
			frames[c][f] = mergedFrames[info.channels * f + c];
		}
	}

	// cleanup
	sf_close(file);
	delete[] mergedFrames;

	return frames; // don't try this at home
}

// saves a wave file
void saveWavFile(const char *fileName, double **frames, SF_INFO &info) {
	// merge channels
	double *mergedFrames = new double[info.frames * info.channels]();
	for (int f = 0; f < info.frames; f++) {
		for (int c = 0; c < info.channels; c++) {
			mergedFrames[info.channels * f + c] = fmax(-1, fmin(1, frames[c][f])); // avoid clipping - thank me later
		}
	}

	// open file
	const int numFrames = info.frames; // store value before reinit
	SNDFILE *file = sf_open(fileName, SFM_WRITE, &info);
	if (sf_error(file)) {
		std::cout << sf_strerror(file) << std::endl;
	}

	// write frames
	sf_write_double(file, mergedFrames, numFrames * info.channels);
	if (sf_error(file)) {
		std::cout << sf_strerror(file) << std::endl;
	}

	// cleanup
	sf_close(file);
	delete[] mergedFrames;
}
