﻿#pragma once
#include "External/gl/glcorearb.h"
#include "External/gl/wglext.h"

struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;
typedef HINSTANCE HMODULE;

extern HMODULE                           gGlLibrary;
extern PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLBINDBUFFERPROC               glBindBuffer;
extern PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
extern PFNGLBUFFERDATAPROC               glBufferData;
extern PFNGLCLEARPROC                    glClear;
extern PFNGLCLEARCOLORPROC               glClearColor;
extern PFNGLCREATESHADERPROC             glCreateShader;
extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLCOMPILESHADERPROC            glCompileShader;
extern PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLDELETESHADERPROC             glDeleteShader;
extern PFNGLDETACHSHADERPROC             glDetachShader;
extern PFNGLDRAWARRAYSPROC               glDrawArrays;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLGENBUFFERSPROC               glGenBuffers;
extern PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC              glGetShaderiv;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLSHADERSOURCEPROC             glShaderSource;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;

void bindNewWGLFunctions();

void bindGLFunctions();