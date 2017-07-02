#include <sstream>

#include "ConvenienceHelper.h"
#include "Constants.h"

int CurrentFrame = FrameLeft;

void ChangeToNextFrame()
{
	if (CurrentFrame == FrameLeft)
	{
		CurrentFrame = FrameRight;
	}
	else
	{
		CurrentFrame = FrameLeft;
	}
}
int GetNewFrameSlot()
{
	return CurrentFrame;
}
int GetCurrentFrameLeft()
{
	if (CurrentFrame == FrameLeft)
	{
		return FrameRight;
	}
	else
	{
		return FrameLeft;
	}
}
int GetCurrentFrameRight()
{
	return CurrentFrame;
}
int GetNewPolyExpSlotA()
{
	return GetCurrentPolyExpLeftA();
}
int GetNewPolyExpSlotB()
{
	return GetCurrentPolyExpLeftB();
}
int GetCurrentPolyExpLeftA()
{
	if (CurrentFrame == FrameLeft)
	{
		return PolyExpLeftA;
	}
	else
	{
		return PolyExpRightA;
	}
}
int GetCurrentPolyExpLeftB()
{
	if (CurrentFrame == FrameLeft)
	{
		return PolyExpLeftB;
	}
	else
	{
		return PolyExpRightB;
	}
}
int GetCurrentPolyExpRightA()
{
	if (CurrentFrame == FrameLeft)
	{
		return PolyExpRightA;
	}
	else
	{
		return PolyExpLeftA;
	}
}
int GetCurrentPolyExpRightB()
{
	if (CurrentFrame == FrameLeft)
	{
		return PolyExpRightB;
	}
	else
	{
		return PolyExpLeftB;
	}
}

int GetTextureActiveSpace(int textureSource)
{
	switch (textureSource)
	{
	case Workspace:
		return WORKSPACE;
	case Workspace2:
		return WORKSPACE2;
	case PolyExpLeftA:
		return POLYEXP_LEFT_A;
	case PolyExpLeftB:
		return POLYEXP_LEFT_B;
	case PolyExpRightA:
		return POLYEXP_RIGHT_A;
	case PolyExpRightB:
		return POLYEXP_RIGHT_B;
	case UpdateMatrixA:
		return UPDATEMATRIX_A;
	case UpdateMatrixB:
		return UPDATEMATRIX_B;
	case Flow:
		return FLOW;
	case FrameLeft:
		return FRAME;
	case FrameRight:
		return FRAME;
	case BlurA:
		return BLURA;
	case BlurB:
		return BLURB;
	}
}

int GetActiveSpaceLocation(int textureSource)
{
	switch (textureSource)
	{
	case Workspace:
		return 0;
	case Workspace2:
		return 1;
	case PolyExpLeftA:
		return 1;
	case PolyExpLeftB:
		return 2;
	case PolyExpRightA:
		return 3;
	case PolyExpRightB:
		return 4;
	case UpdateMatrixA:
		return 5;
	case UpdateMatrixB:
		return 6;
	case Flow:
		return 7;
	case FrameLeft:
		return 0;
	case FrameRight:
		return 0;
	case BlurA:
		return 2;
	case BlurB:
		return 3;
	}
}

int GetDimension(int index)
{
	switch (index)
	{
	case Workspace:
		return 3;
	case Workspace2:
		return 2;
	case PolyExpLeftA:
		return 3;
	case PolyExpLeftB:
		return 2;
	case PolyExpRightA:
		return 3;
	case PolyExpRightB:
		return 2;
	case UpdateMatrixA:
		return 3;
	case UpdateMatrixB:
		return 2;
	case Flow:
		return 3;
	case FrameLeft:
		return 3;
	case FrameRight:
		return 3;
	case BlurA:
		return 3;
	case BlurB:
		return 2;
	}
}

std::string ToString(float number)
{
	std::ostringstream buff;
	buff << number;
	return buff.str();
}

std::string ToString(int number)
{
	std::ostringstream buff;
	buff << number;
	return buff.str();
}

float fmax2(float left, float right)
{
	if (left >= right) return left;
	return right;
}

float fmin2(float left, float right)
{
	if (left <= right) return left;
	return right;
}

float fmax3(float left, float middle, float right)
{
	return fmax2(left, fmax2(middle, right));
}

float fmin3(float left, float middle, float right)
{
	return fmin2(left, fmin2(middle, right));
}