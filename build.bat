@echo off

mkdir .\bin
pushd .\bin

cl /MD /std:c++17 ^
	/EHsc /Zi ^
	/I"..\src" /I"..\libs" ^
	/DGLFW_EXPOSE_NATIVE_WIN32 ^
	..\src\main.cc ^
    ..\libs\glad.cc ^
	kernel32.lib  shell32.lib user32.lib gdi32.lib comdlg32.lib glu32.lib glfw3.lib opengl32.lib assimp.lib^
	/link /LIBPATH:C:\work\projects\playgl\libs

popd