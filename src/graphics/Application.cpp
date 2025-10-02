//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: Application.cpp
// ========
// Source file for graphics application.
//
// Author: Paulo Pagliosa
// Last revision: 14/12/2022

#include "core/Exception.h"
#include "graphics/Application.h"
#include <filesystem>

namespace cg
{ // begin namespace cg

namespace internal::app
{ // begin namespace internal::app

static int glfwInitialized;

static void
errorCallback(int error, const char* description)
{
  runtimeError("GLFW error (%d): %s", error, description);
}

inline auto
maxWindowSize()
{
  int monitorCount;
  auto monitors = glfwGetMonitors(&monitorCount);

  if (monitorCount == 0)
    runtimeError("No monitor found");

  struct { int w, h; } size{};

  for (int i = 0; i < monitorCount; ++i)
  {
    int x, y, w, h;

    glfwGetMonitorWorkarea(monitors[i], &x, &y, &w, &h);
    if (w > size.w)
      size.w = w;
    if (h > size.h)
      size.h = h;
  }
  return size;
}

inline auto
initializeGlfw()
{
  if (!glfwInitialized)
  {
    glfwSetErrorCallback(errorCallback);
    glfwInitialized = glfwInit();
  }
  return glfwInitialized == GL_TRUE;
}

inline void
terminateGlfw()
{
  if (glfwInitialized == GL_TRUE)
  {
    glfwInitialized = 0;
    glfwTerminate();
  }
}

} // end namespace internal::app


/////////////////////////////////////////////////////////////////////
//
// Application implementation
// ===========
std::string Application::_baseDirectory;
std::string Application::_assetsPath;
int Application::_count;

Application::~Application()
{
  delete _mainWindow;
  if (--_count == 0)
    internal::app::terminateGlfw();
}

Application::Application(GLWindow* mainWindow):
  _mainWindow{mainWindow},
  _id{++_count}
{
  // do nothing
}

int
Application::run(int argc, char** argv)
{
  try
  {
    namespace fs = std::filesystem;

    if (_mainWindow == nullptr)
      runtimeError("Undefined main window");
    if (_count == 1)
    {
      if (!internal::app::initializeGlfw())
        runtimeError("Unable to initialize GLFW");
      if (glfwGetPrimaryMonitor() == nullptr)
        runtimeError("No monitors found");
    }
    if (_assetsPath.empty())
    {
      auto basePath = fs::path{argv[0]}.parent_path();

      _baseDirectory = basePath.empty() ? "./" : basePath.string() + '/';
      _assetsPath = _baseDirectory + "assets/";
    }
    _mainWindow->show(argc - 1, argv + 1);
    return EXIT_SUCCESS;
  }
  catch (const std::exception& e)
  {
    printf("Error: %s\nPress any key to exit...", e.what());
    (void)getchar();
    return EXIT_FAILURE;
  }
}

} // end namespace cg
