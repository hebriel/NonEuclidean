#pragma once
#include "GameHeader.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "Portal.hpp"
#include "Player.hpp"
#include "Time.hpp"
#include "Scene.hpp"
#include "Sky.hpp"
#include <GL/glew.h>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include <OpenGL/gl.h> //TODO: This is Mac Only, if you are trying to compile this project on Windows or GNU/Linux, adapt this header file.

class Engine
{
public:
  Engine();
  ~Engine();

  int Run();
  void Update(const Time& deltaTime);
  void Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal);
  void LoadScene(int ix);

  [[nodiscard]] const Player& GetPlayer() const { return *player; }
  [[nodiscard]] float NearestPortalDist() const;

private:
  void CreateGLWindow();
  void InitGLObjects();
  void DestroyGLObjects();
  void SetupInputs();
  void ConfineCursor();
  void FreeCursor();
  void ToggleFullscreen();

  SDL_GLContext m_glContext;
  SDL_Window* m_window;			// window

  size_t iWidth;        		// window width
  size_t iHeight;      			// window height
  bool isFullscreen;  			// fullscreen state
  bool m_mouseFocus;

  Camera main_cam;
  Input input;

  std::vector<std::shared_ptr<Object>> vObjects;
  std::vector<std::shared_ptr<Portal>> vPortals;
  std::shared_ptr<Sky> sky;
  std::shared_ptr<Player> player;

  GLint occlusionCullingSupported;

  std::vector<std::shared_ptr<Scene>> vScenes;
  std::shared_ptr<Scene> curScene;
};
