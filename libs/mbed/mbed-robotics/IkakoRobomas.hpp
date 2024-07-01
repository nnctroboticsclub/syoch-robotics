#pragma once

#include <array>

#include <ikako_c620.h>
#include <robotics/assembly/motor_pair.hpp>
#include <robotics/network/dcan.hpp>

class IkakoRobomasNode {
 public:
  Node<float> velocity;

 public:
  IkakoRobomasNode(
    IkakoM3508 &super//&super=IkakoRobomas
  ) {
    //聖典のshot_mux.output_.SetChangeCallback
    velocity.SetChangeCallback([this](float velo){
      super.velo
    })
  }
};