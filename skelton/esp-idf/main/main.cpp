#include "app.hpp"

extern "C" void app_main(void) {
  App app;
  app.Init();
  app.Main();
}
