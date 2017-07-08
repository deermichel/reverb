#include "circularbuffer.h"
#include <assert.h>

// helper method for calculating positive modulos
// TODO: better place for this :)
inline int positiveMod(int i, int n) {
  return (i % n + n) % n;
}


// creates a new circular buffer of given (max) size
CircularBuffer::CircularBuffer(const int size) :
    mSize(size), mOffset(0), mRecentNumValues(0) {

  assert(size > 0);
  mValues = new double[size]();
}

// destructor
CircularBuffer::~CircularBuffer() {
  delete[] mValues; // cleanup
}

// insert an array of values
void CircularBuffer::insert(const double *values, const int numValues) {
  assert(numValues >= 0 && numValues <= mSize);
  // assert(numValues >= 0);             // use these lines to allow overwriting
  // for (int i = std::max(0, numValues - mSize); i < numValues; i++) {
  for (int i = 0; i < numValues; i++) {
    mValues[positiveMod(i + mOffset, mSize)] = values[i];
  }
  shift(numValues);
}

// shift index offset (phase of circle)
void CircularBuffer::shift(const int numValues) {
  mOffset = (mOffset + numValues) % mSize;
  mRecentNumValues = numValues;
}

// access value in buffer via operator
double& CircularBuffer::operator[](const int index) {
  return mValues[positiveMod((mOffset - mRecentNumValues) + index, mSize)];
}

// access value in (const) buffer via operator
const double& CircularBuffer::operator[](const int index) const {
  return mValues[positiveMod((mOffset - mRecentNumValues) + index, mSize)];
}

// returns the size of the buffer
int CircularBuffer::getSize() const {
  return mSize;
}
