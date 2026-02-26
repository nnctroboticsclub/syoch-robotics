#include <mbed.h>
#include <MyLib/MyLib.hpp>

// ST-Link との UART 接続を表すオブジェクトを作成する
// USBTX, USBRX は ST-Link に接続されているピン
mbed::UnbufferedSerial pc(USBTX, USBRX, 961200);

int main() {
  printf("2 + 3 = %d\n", mylib::Add(2, 3));
}