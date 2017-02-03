#if defined(__APPLE__)
  #include <OpenGL/gl3.h>
  // macOS doesn't need glew since OpenGL functions are resolved at link-time.
  #define glewInit()
  #undef GLAPI
  #include <SDL_opengl.h>
#else
  #define GLEW_STATIC
  #include <GL/glew.h>
  #include <GL/gl.h>
  #include <SDL_opengl.h>
#endif
