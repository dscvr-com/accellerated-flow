#pragma once

int GetTextureActiveSpace(int textureSource);
int GetActiveSpaceLocation(int textureSource);
int GetDimension(int index);

int GetNewFrameSlot();
int GetCurrentFrameLeft();
int GetCurrentFrameRight();
int GetNewPolyExpSlotA();
int GetNewPolyExpSlotB();
int GetCurrentPolyExpLeftA();
int GetCurrentPolyExpLeftB();
int GetCurrentPolyExpRightA();
int GetCurrentPolyExpRightB();

float fmax2(float left, float right);
float fmax3(float left, float middle, float right);
float fmin2(float left, float right);
float fmin3(float left, float middle, float right);

std::string ToString(float number);
std::string ToString(int number);