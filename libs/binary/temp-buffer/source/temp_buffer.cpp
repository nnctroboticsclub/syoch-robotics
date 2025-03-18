#include <robotics/binary/context_detector.hpp>
#include <robotics/binary/temp_buffer.hpp>

static char buf_b[0x80];
static char buf_app[0x80];

void* robotics::binary::GetTemporaryBuffer() {
  return buf_b;
}

void* robotics::binary::GetTemporaryAppBuffer() {
  return buf_app;
}