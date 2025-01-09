#include <cctype>
#include <robotics/binary/linked-allocator.hpp>

#include <cstddef>

using robotics::binary::LinkedAllocator;

extern void sy_put_ch(char ch);

namespace {
void show_ch(char ch) {
  sy_put_ch(ch);
}

void show_str(const char* str) {
  while (*str) {
    show_ch(*str++);
  }
}

void show_hex(uint32_t val, uint8_t digits, bool fill_zero = false) {
  for (int i = 0; i < digits; i++) {
    int shift = (digits - i - 1) * 4;
    int digit = (val >> shift) & 0xF;
    if (digit == 0 && !fill_zero && i != digits - 1) {
      show_ch(' ');
    } else {
      show_ch(digit < 10 ? '0' + digit : 'A' + digit - 10);
    }
  }
}

void show_dec(uint32_t val) {
  char buf[16];
  int i = 0;
  do {
    buf[i++] = '0' + val % 10;
    val /= 10;
  } while (val);
  while (i--) {
    show_ch(buf[i]);
  }
}

void show_ptr(const void* ptr) {
  auto ptr_val = reinterpret_cast<uint32_t>(ptr);
  show_str("0x");
  show_hex(ptr_val, 8, true);
}

void line(int size) {
  for (int i = 0; i < size; i++) {
    show_ch('=');
  }
  show_ch('\n');
}

void line_big() {
  line(20);
}

void line_short() {
  line(10);
}

void hex_dump(const void* ptr, size_t length) {
  for (unsigned int i = 0; i < length / 16; i++) {
    auto row_ptr = static_cast<const uint8_t*>(ptr) + 16 * i;
    show_ptr(row_ptr);
    show_str(" | ");

    // hex
    for (int j = 0; j < 16; j++) {
      show_hex(row_ptr[j], 2, true);
      if (j % 2 == 1) {
        show_ch(' ');
      }
    }

    show_str(" | ");

    // ASCII
    for (int j = 0; j < 16; j++) {
      char c = row_ptr[j];
      show_ch((isprint(c) && c != '.') ? c : '.');
    }
    show_ch('\n');
  }

  show_ch('\n');
}

}  // namespace

extern "C" uint8_t* heap_start asm("mbed_heap_start");
#include <mbed.h>
extern "C" void* malloc(size_t size) {
  auto ptr = LinkedAllocator::GetInstance().Allocate(size);

  if (!ptr) {
    show_str("Failed to allocate memory: ");
    show_hex(size, 8);
    show_ch('B');
    show_ch('\n');

    show_ch(0xff);
    asm("bkpt #0");

    return nullptr;
  }

  return ptr;
}

extern "C" void free(void* ptr) {
  LinkedAllocator::GetInstance().Deallocate(ptr);
}