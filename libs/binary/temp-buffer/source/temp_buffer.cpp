#include <robotics/binary/temp_buffer.hpp>

static char buf[0x100];

void* robotics::binary::GetTemporaryBuffer() {
  return buf;
}