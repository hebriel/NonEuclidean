#pragma once
#include "Object.hpp"
#include "Portal.hpp"
#include "Player.hpp"

class Scene {
public:
  virtual void Load(PObjectVec& objs, PPortalVec& portals, Player& player)=0;
  virtual void Unload() {};
};
