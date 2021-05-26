@echo off

mkdir .\bin
pushd .\bin

cl /MD /std:c++17 ^
	/EHsc /Zi ^
	/I"..\src" /I"..\libs" ^
	/DGLFW_EXPOSE_NATIVE_WIN32 ^
	..\examples\example_scene.cc ^
    ..\libs\glad.cc ^
	..\libs\fmt\format.cc ^
	..\libs\imgui\imgui.cpp ^
	..\libs\imgui\imgui_widgets.cpp ^
	..\libs\imgui\imgui_draw.cpp ^
	..\libs\imgui\imgui_impl_opengl2.cpp ^
	..\libs\imgui\imgui_impl_glfw.cpp ^
	kernel32.lib  shell32.lib user32.lib gdi32.lib comdlg32.lib glu32.lib glfw3.lib opengl32.lib assimp.lib^
	/link /LIBPATH:C:\work\projects\playgl\libs

popd