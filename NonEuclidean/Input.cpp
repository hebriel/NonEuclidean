#include "Input.hpp"
#include "GameHeader.hpp"
#include <memory>

Input::Input() {
  memset(this, 0, sizeof(Input));
}

void Input::EndFrame() {
  mouse_dx = mouse_dx * GH_MOUSE_SMOOTH + mouse_ddx * (1.0f - GH_MOUSE_SMOOTH);
  mouse_dy = mouse_dy * GH_MOUSE_SMOOTH + mouse_ddy * (1.0f - GH_MOUSE_SMOOTH);
  mouse_ddx = 0.0f;
  mouse_ddy = 0.0f;
}