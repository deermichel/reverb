#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

// cyclic array implementation for buffer usage
class CircularBuffer {

public:
  // creates a new circular buffer of given (max) size
  CircularBuffer(const int size);
  ~CircularBuffer(); // destructor

  // insert an array of values
  void insert(const double *values, const int numValues);

  // shift index offset (phase of circle)
  void shift(const int numValues);

  // access value in buffer via operator
  double& operator[](const int index);
  const double& operator[](const int index) const;

  // returns the size of the buffer
  int getSize() const;

private:
  // size of the buffer
  const int mSize;

  // values stored by the buffer
  double *mValues;

  // index where next data will be stored (phase of circle)
  int mOffset;

  // number of most recently added values
  int mRecentNumValues;

};

#endif
