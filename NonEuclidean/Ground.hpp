#pragma once
#include "Object.hpp"
#include "Resources.hpp"

class Ground : public Object {
public:
  Ground(bool slope=false) {
    if (slope) {
      mesh = AquireMesh("ground_slope.obj");
    } else {
      mesh = AquireMesh("ground.obj");
    }
    shader = AquireShader("texture");
    texture = AquireTexture("checker_green.bmp");
    scale = Vector3(10, 1, 10);
  }
  virtual ~Ground() {}
};
