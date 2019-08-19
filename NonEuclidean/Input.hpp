#pragma once

#include <SDL2/SDL.h>

struct tagRAWINPUT;
class Input {
public:
  Input();

  void EndFrame();

  //Keyboard
  const Uint8* key;
  const Uint8* key_press;

  //Mouse
  bool mouse_button[3];
  bool mouse_button_press[3];
  float mouse_dx;
  float mouse_dy;
  float mouse_ddx;
  float mouse_ddy;

  //Joystick
  //TODO:

  //Bindings
  //TODO:

  //Calibration
  //TODO:
};
