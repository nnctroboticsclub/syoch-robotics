#include <mbed.h>

// ST-Link との UART 接続を表すオブジェクトを作成する
// USBTX, USBRX は ST-Link に接続されているピン
mbed::UnbufferedSerial pc(USBTX, USBRX, 961200);

int main() {}