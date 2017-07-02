#pragma once

void Init(int imageWidth, int imageHeight);
void WindowsInit();

void LoadInitialFrame(void *frame);
void LoadNextFrame(void *frame);
void ChangeToNextFrame();

void ExecuteShaders();
void CleanUp();

void SaveImage(int textureSource, std::string name, float mul, bool useAbs);
