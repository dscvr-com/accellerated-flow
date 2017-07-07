#include "bitmap_image.hpp"


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

#include "Constants.h"
#include "ShaderManager.h"
#include "ConvenienceHelper.h"

#include "OpenGL.h";

GLFWwindow* window;

int _imageWidth;
int _imageHeight;

GLuint _framebuffer;

const bool UseAbs = true;
const bool OnlyPositive = false;

GLuint _vertexArrayId;

int main(void)
{
	Init(1800, 1216);

	//For now, later replace with LoadFrames!-------

	//glActiveTexture(GL_TEXTURE0);
	SetTexture(FrameLeft, loadBMP_custom("Murmeln_LEFT.bmp"));
	//glActiveTexture(GL_TEXTURE1);
	SetTexture(FrameRight, loadBMP_custom("Murmeln_RIGHT.bmp"));
	//-----------------------------------------------
	
	do
	{
		ExecuteShaders();

		glfwSwapBuffers(window);
		glfwPollEvents();
		break;
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	CleanUp();

	glfwTerminate();

	return 0;
}

void Init(int imageWidth, int imageHeight)
{
	_imageWidth = imageWidth;
	_imageHeight = imageHeight;

	WindowsInit();

	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return;
	}

	//Stays for testing
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glGenVertexArrays(1, &_vertexArrayId);
	glBindVertexArray(_vertexArrayId);

	glGenFramebuffers(1, &_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

	ShaderInitialize(imageWidth, imageHeight);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GetTexture(Workspace), 0);

	GLenum drawbuffer[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::exception("Framebuffer did not complete loading");
	}
}

void WindowsInit()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(_imageWidth, _imageHeight, "Stitcher Library Test Window", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
}

void LoadInitialFrame(void* frame)
{
	LoadNextFrame(frame);
}

void LoadNextFrame(void* frame)
{
	ChangeToNextFrame();
	glBindTexture(GL_TEXTURE_2D, GetTexture(GetNewFrameSlot()));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _imageWidth, _imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, frame);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	ExecuteVerticalConvolutionShader(GetNewFrameSlot(), Workspace);
	ExecuteHorizontalConvolutionShader(Workspace, GetNewPolyExpSlotA(), GetNewPolyExpSlotB());
}

void CalculateOpticalFlow()
{
	//ExecuteMatrixUpdateShader(GetCurrentPolyExpLeftA(), GetCurrentPolyExpLeftB(), GetCurrentPolyExpRightA(), GetCurrentPolyExpRightB(), Flow, UpdateMatrixA, UpdateMatrixB);
}

void SaveImage(int textureSource, std::string name, float mul, bool useAbs)
{
	int bytesPerPixel = GetDimension(textureSource);
	int size = bytesPerPixel * size_t(_imageWidth) * size_t(_imageHeight);
	float *data = (float*) malloc(size * sizeof(float));

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	int imageType = bytesPerPixel == 2 ? GL_RG : GL_RGB;
	glReadPixels(0, 0, _imageWidth, _imageHeight, imageType, GL_FLOAT, data);

	for (int i = 0; i < bytesPerPixel; i += 1)
	{
		bitmap_image image((unsigned int) _imageWidth, (unsigned int) _imageHeight);
		if (bytesPerPixel == 3)
		{
			image.import_rgb_bw(data, mul * 255, useAbs, i);
		}
		else
		{
			image.import_rg_bw(data, mul * 255, useAbs, i);
		}
		image.vertical_flip();

		std::string filename = "result/";
		filename.append(name);
		filename.append("_Mul_");
		filename.append(ToString(mul));
		if (useAbs)
		{
			filename.append("_Abs");
		}
		filename.append("color");
		filename.append(ToString(i));
		filename.append(".bmp");
		image.save_image(filename);
	}

	free(data); 
}

void ExecuteShaders()
{
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	//Only for testing, later no need to set every time
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
	//-------------------------------------------------

	/*ExecuteCopyShader(GetCurrentFrameLeft());
	SaveImage(GetCurrentFrameLeft(), "Workspace.bmp");*/

	/*ExecuteVerticalGaussBlur(GetCurrentFrameLeft(), GetCurrentFrameLeft(), Workspace, Workspace2);
	ExecuteHorizontalGaussBlur(Workspace, Workspace2, GetCurrentFrameLeft(), GetCurrentPolyExpLeftB());
	ExecuteVerticalGaussBlur(GetCurrentFrameRight(), GetCurrentFrameRight(), Workspace, Workspace2);
	ExecuteHorizontalGaussBlur(Workspace, Workspace2, GetCurrentFrameRight(), GetCurrentPolyExpLeftB());*/

	ExecuteVerticalConvolutionShader(GetCurrentFrameLeft(), Workspace);
	ExecuteHorizontalConvolutionShader(Workspace, GetCurrentPolyExpLeftA(), GetCurrentPolyExpLeftB());
	SaveImage(GetCurrentPolyExpLeftA(), "PolyExpA", 10, UseAbs);
	
	ExecuteCopyShader(GetCurrentPolyExpLeftB(), 1, Workspace);
	SaveImage(Workspace2, "PolyExpB", 100, UseAbs);
	
	ExecuteVerticalConvolutionShader(GetCurrentFrameRight(), Workspace);
	ExecuteHorizontalConvolutionShader(Workspace, GetCurrentPolyExpRightA(), GetCurrentPolyExpRightB());

	SaveImage(GetCurrentPolyExpRightA(), "PolyExpRightA", 10, UseAbs);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GetTexture(Flow), 0);
	/*glClearColor(1, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);*/
	
	ExecuteMatrixUpdateShader(GetCurrentPolyExpLeftA(), GetCurrentPolyExpLeftB(), GetCurrentPolyExpRightA(), GetCurrentPolyExpRightB(), Flow, UpdateMatrixA, UpdateMatrixB, false);
	SaveImage(UpdateMatrixA, "UpdateMatrixA", 10000, OnlyPositive);

	ExecuteCopyShader(UpdateMatrixB, 1, Workspace);
	SaveImage(UpdateMatrixB, "UpdateMatrixB", 10000, OnlyPositive);

	for (int i = 0; i < 5; i += 1) {
		std::string iteration = ToString(i);

		ExecuteVerticalGaussBlur(UpdateMatrixA, UpdateMatrixB, Workspace, Workspace2);
		ExecuteHorizontalGaussBlur(Workspace, Workspace2, BlurA, BlurB);
		//SaveImage(BlurA, "BlurA" + iteration, 100, UseAbs);

		ExecuteCopyShader(BlurB, 1, Workspace);
		//SaveImage(BlurB, "BlurB" + iteration, 100, UseAbs);

		ExecuteFlowUpdateShader(BlurA, BlurB, Flow);
		SaveImage(Flow, "Flow" + iteration, 10000, UseAbs);
		//SaveImage(Flow, "Flow" + iteration, 1000, UseAbs);
		//SaveImage(Flow, "Flow" + iteration, 0.005f, UseAbs);
		//SaveImage(Flow, "Flow" + iteration, 100, OnlyPositive);
		//SaveImage(Flow, "Flow" + iteration, 1000, OnlyPositive);
		//SaveImage(Flow, "Flow" + iteration, 0.005f, OnlyPositive);

		ExecuteMatrixUpdateShader(GetCurrentPolyExpLeftA(), GetCurrentPolyExpLeftB(), GetCurrentPolyExpRightA(), GetCurrentPolyExpRightB(), Flow, UpdateMatrixA, UpdateMatrixB, false);
		SaveImage(UpdateMatrixA, "UpdateMatrixA" + iteration, 10000, OnlyPositive);

		ExecuteCopyShader(UpdateMatrixB, 1, Workspace);
		SaveImage(UpdateMatrixB, "UpdateMatrixB" + iteration, 10000, OnlyPositive);
	}

	//Only for testing, later no need to set every time
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//ExecuteCopyShader(UPDATEMATRIX, UpdateMatrix);
	ExecuteCopyShader(Flow, 1, -1);
	//ExecuteCopyShader(UpdateMatrix);
	//-------------------------------------------------
}

void CleanUp()
{
	glDeleteFramebuffers(1, &_framebuffer);
	glDeleteVertexArrays(1, &_vertexArrayId);

	ShaderCleanup();
}