#include <bd621x/BD621xFull.hpp>

using namespace std::chrono_literals;

namespace robotics::node {

BD621xFull::BD621xFull(std::shared_ptr<robotics::driver::Dout> rin,
                       std::shared_ptr<robotics::driver::Dout> fin)
    : rin_(rin), fin_(fin) {}
void BD621xFull::SetSpeed(float speed) {
  if (0.05 < speed && speed < 0.05) speed = 0;

  if (speed == 0) {
    fin_->Write(1);
    rin_->Write(1);
  } else if (speed > 0) {
    fin_->Write(1);
    rin_->Write(0);
  } else if (speed < 0) {
    fin_->Write(0);
    rin_->Write(1);
  }
}

}  // namespace robotics::node