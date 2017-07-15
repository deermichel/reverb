#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include "multichannelreverb.h"

using namespace cv;
#define NUM_COLOR_CHANNELS 3

// video information
struct VideoInfo {
  int width;
  int height;
  double fps;
  double fourcc;
  int frames;
  int channels;
  int matType;
};

// forward-declarations
double** readVideoFile(const char *fileName, VideoInfo &info);
void saveVideoFile(const char *fileName, double **data, VideoInfo &info);
void playVideos();


// entry point
int main(int argc, char *argv[]) {

  // IDEA: how about a bit of parallelization? or GPU processing :O
  // TODO: do sth against that crazy RAM consumption

  // init
  VideoInfo fileInfo;
  double **inputs = readVideoFile("input.mov", fileInfo);
  double **outputs = new double*[fileInfo.channels];
  for (int c = 0; c < fileInfo.channels; c++) {
    outputs[c] = new double[fileInfo.frames]();
  }

  // setup reverb
	MultiChannelReverb reverb(4, fileInfo.frames, fileInfo.channels);
	reverb.setFeedDryIn(0.2);
	reverb.setFeedWetOut(0.5);
	reverb.setDrySignalMix(0.3);
	reverb.setDelayTapTime(12);
	reverb.setDecay(0.7);

  // process
	std::cout << "Processing..." << std::endl;
	reverb.process(inputs, outputs, fileInfo.frames);
	std::cout << "Done." << std::endl;

	// save results
	saveVideoFile("output.mov", outputs, fileInfo);

	// cleanup
	for (int c = 0; c < fileInfo.channels; c++) {
		delete[] inputs[c];
		delete[] outputs[c];
	}
  delete[] inputs;
  delete[] outputs;

  // finally
  playVideos();
}

// reads a video file
double** readVideoFile(const char *fileName, VideoInfo &info) {
  // open file
  VideoCapture input("input.mov");
	if (!input.isOpened()) {
		std::cout << "Error reading video file." << std::endl;
	}

  // read video info
	info.width = input.get(CV_CAP_PROP_FRAME_WIDTH);
	info.height = input.get(CV_CAP_PROP_FRAME_HEIGHT);
	info.fps = input.get(CV_CAP_PROP_FPS);
	info.fourcc = input.get(CV_CAP_PROP_FOURCC);
	info.frames = input.get(CV_CAP_PROP_FRAME_COUNT);
  info.channels = info.width * info.height * NUM_COLOR_CHANNELS;

	// init channel data array
  double **data = new double*[info.channels];
  for (int c = 0; c < info.channels; c++) {
    data[c] = new double[info.frames]();
  }

  // read frames and split data into channels
  Mat frame;
  for (int i = 0; i < info.frames; i++) {
    input >> frame;

    for (int x = 0; x < frame.rows; x++) {
      for (int y = 0; y < frame.cols; y++) {
        Vec3b color = frame.at<Vec3b>(x, y);
        for (int c = 0; c < NUM_COLOR_CHANNELS; c++) {
          data[x * frame.cols * NUM_COLOR_CHANNELS + y * NUM_COLOR_CHANNELS + c][i] = color.val[c];
        }
      }
    }

    if (i % 10 == 0) {
      std::cout << "Reading file... " << (100.0 * i / info.frames) << "%" << std::endl;
    }
  }
  info.matType = frame.type();

	return data; // don't try this at home
}

// saves a video file
void saveVideoFile(const char *fileName, double **data, VideoInfo &info) {
  // open file
  remove("output.mov"); // avoid bugs
  VideoWriter output("output.mov", info.fourcc, info.fps, Size(info.width, info.height), true);
	if (!output.isOpened()) {
		std::cout << "Error writing video file." << std::endl;
	}

  // write frames
  Mat frame;
  for (int i = 0; i < info.frames; i++) {
    frame = Mat::zeros(info.height, info.width, info.matType);

    for (int x = 0; x < frame.rows; x++) {
      for (int y = 0; y < frame.cols; y++) {
        Vec3b &color = frame.at<Vec3b>(x, y);
        for (int c = 0; c < NUM_COLOR_CHANNELS; c++) {
          color.val[c] = fmax(0, fmin(255, data[x * frame.cols * NUM_COLOR_CHANNELS + y * NUM_COLOR_CHANNELS + c][i]));
        }
      }
    }

    output.write(frame);

    if (i % 10 == 0) {
      std::cout << "Writing file... " << (100.0 * i / info.frames) << "%" << std::endl;
    }
  }
}

// plays the videos
void playVideos() {
  // open files
  VideoCapture input("input.mov");
  VideoCapture output("output.mov");
	if (!input.isOpened() || !output.isOpened()) {
		std::cout << "Error reading video files." << std::endl;
	}

  // display them
  namedWindow("input", CV_WINDOW_AUTOSIZE);
  namedWindow("output", CV_WINDOW_AUTOSIZE);
  Mat inputFrame;
  Mat outputFrame;
  while (true) {
    input >> inputFrame;
    output >> outputFrame;
    if (inputFrame.empty() || outputFrame.empty()) {
      break;
    }

    imshow("input", inputFrame);
    imshow("output", outputFrame);
    waitKey(20);
  }
}
