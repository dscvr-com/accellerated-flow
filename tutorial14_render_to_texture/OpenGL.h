#pragma once



void Init(int imageWidth, int imageHeight);
void WindowsInit();

void LoadInitialFrame(void *frame);
void LoadNextFrame(void *frame);
void ChangeToNextFrame();

void ExecuteShaders();
void CleanUp();

int GetNewFrameSlot();
int GetNewPolyExpSlotA();
int GetNewPolyExpSlotB();
int GetCurrentPolyExpLeftA();
int GetCurrentPolyExpLeftB();

void SaveImage(int textureSource, std::string name, float mul, bool useAbs);

float fmax2(float left, float right);
float fmax3(float left, float middle, float right);
float fmin2(float left, float right);
float fmin3(float left, float middle, float right);

std::string ToString(float number);
std::string ToString(int number);
