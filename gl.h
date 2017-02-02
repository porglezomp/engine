#ifdef __APPLE__
  #include <OpenGL/gl.h>
  // macOS doesn't need glew since OpenGL functions are resolved at link-time.
  #define glewInit()
#else
  #define GLEW_STATIC
  #include <GL/glew.h>
  #include <GL/gl.h>
#endif
