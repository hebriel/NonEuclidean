#include "Engine.hpp"
#include "Physical.hpp"
#include "Level1.hpp"
#include "Level2.hpp"
#include "Level3.hpp"
#include "Level4.hpp"
#include "Level5.hpp"
#include "Level6.hpp"
#include "Level7.hpp"
#include <GL/glew.h>
#include <cmath>
#include <iostream>
#include <algorithm>

Engine* GH_ENGINE = nullptr;
Player* GH_PLAYER = nullptr;
const Input* GH_INPUT = nullptr;
int GH_REC_LEVEL = 0;
int64_t GH_FRAME = 0;

Engine::Engine() : m_mouseFocus(true) {
  GH_ENGINE = this;
  GH_INPUT = &input;
  isFullscreen = false;

  //SetProcessDPIAware();
  CreateGLWindow();
  InitGLObjects();
  SetupInputs();

  player.reset(new Player);
  GH_PLAYER = player.get();

  vScenes.push_back(std::shared_ptr<Scene>(new Level1));
  vScenes.push_back(std::shared_ptr<Scene>(new Level2(3)));
  vScenes.push_back(std::shared_ptr<Scene>(new Level2(6)));
  vScenes.push_back(std::shared_ptr<Scene>(new Level3));
  vScenes.push_back(std::shared_ptr<Scene>(new Level4));
  vScenes.push_back(std::shared_ptr<Scene>(new Level5));
  vScenes.push_back(std::shared_ptr<Scene>(new Level6));
  vScenes.push_back(std::shared_ptr<Scene>(new Level7));

  LoadScene(0);

  sky.reset(new Sky);
}

Engine::~Engine() {
  //ClipCursor(NULL);
  //wglMakeCurrent(NULL, NULL);
  //ReleaseDC(hWnd, hDC);
  //wglDeleteContext(hRC);
  //DestroyWindow(hWnd);
}

int Engine::Run() {
  //TODO: Window validity check

  //Recieve events from this window
  //SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);

  //Setup the timer
  Clock clock;

  GH_FRAME = 0;

  bool running = true;

  ConfineCursor();

  //Game loop
	while (running)
	{
		clock.restart();
		input.key_press = SDL_GetKeyboardState(NULL);

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			  running = false;
	  		else if(event.type == SDL_MOUSEMOTION && m_mouseFocus)
	  		{
	  			input.mouse_dx = event.motion.xrel * GH_MOUSE_SENSITIVITY;
	  			input.mouse_dy = event.motion.yrel * GH_MOUSE_SENSITIVITY;
	  		}
	  		else if(event.type == SDL_MOUSEBUTTONDOWN)
	  		{
	  			if(event.button.button == SDL_BUTTON_LEFT)
			  	ConfineCursor();
	  		}
	  		else if(event.type == SDL_KEYDOWN)
				if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					FreeCursor();
		}

	if (input.key_press[SDL_SCANCODE_0])
	LoadScene(0);
	else if (input.key_press[SDL_SCANCODE_1])
	LoadScene(1);
	else if (input.key_press[SDL_SCANCODE_2])
	LoadScene(2);
	else if (input.key_press[SDL_SCANCODE_3])
	LoadScene(3);
	else if (input.key_press[SDL_SCANCODE_4])
	LoadScene(4);
	else if (input.key_press[SDL_SCANCODE_5])
	LoadScene(5);
	else if (input.key_press[SDL_SCANCODE_6])
	LoadScene(6);
	else if (input.key_press[SDL_SCANCODE_7])
	LoadScene(7);

	Update(clock.getElapsedTime());
	GH_FRAME += 1;
	input.EndFrame();
	const float n = GH_CLAMP(NearestPortalDist() * 0.5f, GH_NEAR_MIN, GH_NEAR_MAX);
	main_cam.worldView = player->WorldToCam();
	main_cam.SetSize(iWidth, iHeight, n, GH_FAR);
	main_cam.UseViewport();

	GH_REC_LEVEL = GH_MAX_RECURSION;
	Render(main_cam, 0, nullptr);
	SDL_GL_SwapWindow(m_window);
  }

  DestroyGLObjects();
  return 0;
}

void Engine::LoadScene(int ix) {
  //Clear out old scene
  if (curScene) { curScene->Unload(); }
  vObjects.clear();
  vPortals.clear();
  player->Reset();

  //Create new scene
  curScene = vScenes[ix];
  curScene->Load(vObjects, vPortals, *player);
  vObjects.push_back(player);
}

void Engine::Update(const Time& deltaTime) {
  //Update
  for (size_t i = 0; i < vObjects.size(); ++i) {
    assert(vObjects[i].get());
    vObjects[i]->Update();
  }

  //Collisions
  //For each physics object
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (!physical) { continue; }
    Matrix4 worldToLocal = physical->WorldToLocal();

    //For each object to collide with
    for (size_t j = 0; j < vObjects.size(); ++j) {
      if (i == j) { continue; }
      Object& obj = *vObjects[j];
      if (!obj.mesh) { continue; }

      //For each hit sphere
      for (size_t s = 0; s < physical->hitSpheres.size(); ++s) {
        //Brings point from collider's local coordinates to hits's local coordinates.
        const Sphere& sphere = physical->hitSpheres[s];
        Matrix4 worldToUnit = sphere.LocalToUnit() * worldToLocal;
        Matrix4 localToUnit = worldToUnit * obj.LocalToWorld();
        Matrix4 unitToWorld = worldToUnit.Inverse();

        //For each collider
        for (size_t c = 0; c < obj.mesh->colliders.size(); ++c) {
          Vector3 push;
          const Collider& collider = obj.mesh->colliders[c];
          if (collider.Collide(localToUnit, push)) {
            //If push is too small, just ignore
            push = unitToWorld.MulDirection(push);
            vObjects[j]->OnHit(*physical, push);
            physical->OnCollide(*vObjects[j], push);

            worldToLocal = physical->WorldToLocal();
            worldToUnit = sphere.LocalToUnit() * worldToLocal;
            localToUnit = worldToUnit * obj.LocalToWorld();
            unitToWorld = worldToUnit.Inverse();
          }
        }
      }
    }
  }

  //Portals
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (physical) {
      for (size_t j = 0; j < vPortals.size(); ++j) {
        if (physical->TryPortal(*vPortals[j])) {
          break;
        }
      }
    }
  }
}

void Engine::Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal) {
  //Clear buffers
  if (GH_USE_SKY) {
    glClear(GL_DEPTH_BUFFER_BIT);
    sky->Draw(cam);
  } else {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  //Create queries (if applicable)
  GLuint queries[GH_MAX_PORTALS];
  GLuint drawTest[GH_MAX_PORTALS];
  assert(vPortals.size() <= GH_MAX_PORTALS);
  if (occlusionCullingSupported) {
    glGenQueriesARB((GLsizei)vPortals.size(), queries);
  }

  //Draw scene
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->Draw(cam, curFBO);
  }

  //Draw portals if possible
  if (GH_REC_LEVEL > 0) {
    //Draw portals
    GH_REC_LEVEL -= 1;
    if (occlusionCullingSupported && GH_REC_LEVEL > 0) {
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
          vPortals[i]->DrawPink(cam);
          glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }
      }
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &drawTest[i]);
        }
      };
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);
      glDeleteQueriesARB((GLsizei)vPortals.size(), queries);
    }
    for (size_t i = 0; i < vPortals.size(); ++i) {
      if (vPortals[i].get() != skipPortal) {
        if (occlusionCullingSupported && (GH_REC_LEVEL > 0) && (drawTest[i] == 0)) {
          continue;
        } else {
          vPortals[i]->Draw(cam, curFBO);
        }
      }
    }
    GH_REC_LEVEL += 1;
  }
  
#if 0
  //Debug draw colliders
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->DebugDraw(cam);
  }
#endif
}

void Engine::CreateGLWindow()
{
	iWidth = GH_SCREEN_WIDTH;
	iHeight = GH_SCREEN_HEIGHT;

	m_window = SDL_CreateWindow(
			GH_TITLE,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			iWidth, iHeight,
			SDL_WINDOW_OPENGL);

	//Setting some OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	m_glContext = SDL_GL_CreateContext(m_window);

	if (!m_glContext)
		throw std::runtime_error("Error while creating an OpenGL context.");

	SDL_GL_SetSwapInterval(1);

  /*WNDCLASSEX wc;
  hInstance = GetModuleHandle(NULL);
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = (WNDPROC)StaticWindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = GH_CLASS;
  wc.hIconSm = NULL;*/

  /*pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 32;
  pfd.iLayerType = PFD_MAIN_PLANE;*/
}

void Engine::InitGLObjects() {
  //Initialize extensions
	glewExperimental = true;
  glewInit();

  //Basic global variables
  glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  //Check GL functionality
  glGetQueryiv(GL_SAMPLES_PASSED_ARB, GL_QUERY_COUNTER_BITS_ARB, &occlusionCullingSupported);

  //Attempt to enalbe vsync (if failure then oh well)
  //wglSwapIntervalEXT(1);
}

void Engine::DestroyGLObjects() {
  curScene->Unload();
  vObjects.clear();
  vPortals.clear();
}

void Engine::SetupInputs() {
  /*static const int HID_USAGE_PAGE_GENERIC     = 0x01;
  static const int HID_USAGE_GENERIC_MOUSE    = 0x02;
  static const int HID_USAGE_GENERIC_JOYSTICK = 0x04;
  static const int HID_USAGE_GENERIC_GAMEPAD  = 0x05;

  RAWINPUTDEVICE Rid[3];

  //Mouse
  Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
  Rid[0].dwFlags = RIDEV_INPUTSINK;
  Rid[0].hwndTarget = hWnd;

  //Joystick
  Rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Rid[1].usUsage = HID_USAGE_GENERIC_JOYSTICK;
  Rid[1].dwFlags = 0;
  Rid[1].hwndTarget = 0;

  //Gamepad
  Rid[2].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Rid[2].usUsage = HID_USAGE_GENERIC_GAMEPAD;
  Rid[2].dwFlags = 0;
  Rid[2].hwndTarget = 0;

  RegisterRawInputDevices(Rid, 3, sizeof(Rid[0]));*/
  //TODO
}

void Engine::ConfineCursor()
{
	m_mouseFocus = true;
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Engine::FreeCursor()
{
	m_mouseFocus = false;
	SDL_SetRelativeMouseMode(SDL_FALSE);
	SDL_WarpMouseInWindow(m_window,iWidth / 2,iHeight / 2);
}

float Engine::NearestPortalDist() const {
  float dist = FLT_MAX;
  for (size_t i = 0; i < vPortals.size(); ++i) {
    dist = GH_MIN(dist, vPortals[i]->DistTo(player->pos));
  }
  return dist;
}

void Engine::ToggleFullscreen() {
//TODO
}
