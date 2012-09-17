/*
 * profile_config.h
 *
 *  Created on: Feb 29, 2012
 *      Author: jschober
 */

// check: http://glew.sourceforge.net/install.html


#ifndef PLATFORM_SUPPORT_PROFILE_CONFIG_H
#define PLATFORM_SUPPORT_PROFILE_CONFIG_H

#ifdef _WIN32
 #define VENDOR "Microsoft"
 #define PLATFORM_NAME "Windows"
#ifndef _HAS_WX_WIDGETS_
 #define _HAS_WX_WIDGETS_
#endif
 // desktop GL version (incl. shader/fixed pipeline)
#ifndef _HAS_OPENGL_
 #define _HAS_OPENGL_
#endif
 // #define _HAS_EGL1_ -- I might enable this, too in the desktop version
 // #define _HAS_EGL2_ -- I might enable this, too in the desktop version
 #ifdef _HAS_GLSL_
 #include <GL/glew.h>
 #endif
 #define _GL_EXT_
 #include <GL/gl.h>
 #include <GL/glu.h>
 #include <GL/glext.h>
#endif

#ifdef __linux__
 #define VENDOR "Linux"
 #define PLATFORM_NAME "Generic Linux"
 #define _HAS_XWINDOWS_
 #define _HAS_SDL_
 #define _HAS_SDL_IMAGE_
 #define _HAS_SDL_TTF_
 #define _HAS_WX_WIDGETS_
 #define _HAS_OPENGL_
 #define _GL_EXT_
 #include <GL/gl.h>
 #include <GL/glu.h>
#endif

#ifdef __APPLE__
 #define VENDOR "APPLE"
 #define PLATFORM_NAME "Generic MacOS-X"
#ifndef _HAS_SDL_
 #define _HAS_SDL_
#endif
// #define _HAS_WX_WIDGETS_
#ifndef _HAS_SDL_IMAGE_
 #define _HAS_SDL_IMAGE_
#endif
#ifndef _HAS_SDL_TTF_
 #define _HAS_SDL_TTF_
#endif
#ifndef _HAS_OPENGL_
 #define _HAS_OPENGL_
#endif
 #define _GL_EXT_
 #include <GL/gl.h>
 #include <GL/glu.h>
#endif

#ifdef __QNXNTO__
 #define VENDOR "RIM"
 #define PLATFORM_NAME "BlackBerry(c) PlayBook"
 #define _PLAYBOOK_
 // only one can be define - use -Dxx instead!
 // #define _HAS_EGL1_
 // #define _HAS_EGL2_
 // must be EGL1 or EGL2!
 #ifdef _HAS_OPENGL_
  #undef _HAS_OPENGL_
 #endif
 // no wxWidgets in playbook
 #ifdef _HAS_WX_WIDGETS_
  #undef _HAS_WX_WIDGETS_
 #endif
 // disable SDL (for now)
 #ifdef _HAS_SDL_
  #undef _HAS_SDL_
 #endif
 #ifdef _HAS_EGL1_
 # include <GLES/gl.h>
 #endif
 #ifdef _HAS_EGL2_
 # include <GLES2/gl2.h>
 #endif
#endif


#ifdef _HAS_SDL_
// SDL specific configs
#else
//#ifdef _HAS_SDL_IMAGE_
// #undef _HAS_SDL_IMAGE_
//#endif
//#ifdef _HAS_SDL_TTF_
// #undef _HAS_SDL_TTF_
//#endif
#endif

// Application should choose either one of the render profiles - not all of them!
#ifdef _HAS_EGL1_
#ifndef RENDER_PROFILE
#define RENDER_PROFILE "OpenGL/ES 1.0"
#endif
#endif
#ifdef _HAS_EGL2_
#ifndef RENDER_PROFILE
#define RENDER_PROFILE "OpenGL/ES 2.0"
#endif
#endif
#ifdef _HAS_OPENGL_
#ifndef RENDER_PROFILE
#define RENDER_PROFILE "OpenGL/Glew"
#endif
// OpenGL spec config
#else
#ifdef _GL_EXT_
 #undef _GL_EXT_
#endif
#endif
#ifdef _HAS_WX_WIDGETS_
// WX widget specifc config
#endif


namespace pei {

    namespace psp {

    }; // namespace psp
} //namespace pei


#endif /* PLATFORM_SUPPORT_PROFILE_CONFIG_H */
