#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

//#include <png.h>

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

#include "OpenGL.h";

GLFWwindow* window;

GLuint _quadVertexbuffer;

int _imageWidth;
int _imageHeight;

const int WORKSPACE = GL_TEXTURE0;
const int WORKSPACE2 = GL_TEXTURE1;
const int POLYEXP_LEFT_A = GL_TEXTURE1;
const int POLYEXP_LEFT_B = GL_TEXTURE2;
const int POLYEXP_RIGHT_A = GL_TEXTURE3;
const int POLYEXP_RIGHT_B = GL_TEXTURE4;
const int UPDATEMATRIX_A = GL_TEXTURE5;
const int UPDATEMATRIX_B = GL_TEXTURE6;
const int FLOW = GL_TEXTURE7;
const int FRAME = GL_TEXTURE0;
const int BLURA = GL_TEXTURE2;
const int BLURB = GL_TEXTURE3;

GLuint _framebuffer;
GLuint* _textures;
const int Workspace = 0;
const int Workspace2 = 1;
const int PolyExpLeftA = 2;
const int PolyExpLeftB = 3;
const int PolyExpRightA = 4;
const int PolyExpRightB = 5;
const int UpdateMatrixA = 6;
const int UpdateMatrixB = 7;
const int Flow = 8;
const int FrameLeft = 9;
const int FrameRight = 10;
const int BlurA = 11;
const int BlurB = 12;

int CurrentFrame = FrameLeft;

GLenum* _attachments;

int _numTextures = 13;


GLuint _vertexArrayId;

GLuint _verticalShader;
GLuint _verticalShader_srcId;
GLuint _verticalShader_gId;
GLuint _verticalShader_xgId;
GLuint _verticalShader_xxgId;
GLuint _verticalShader_nId;
GLuint _verticalShader_heightId;
GLuint _verticalShader_hsId;

GLuint _horizontalShader;
GLuint _horizontalShader_srcId;
GLuint _horizontalShader_gId;
GLuint _horizontalShader_xgId;
GLuint _horizontalShader_xxgId;
GLuint _horizontalShader_id11Id;
GLuint _horizontalShader_id03Id;
GLuint _horizontalShader_id33Id;
GLuint _horizontalShader_id55Id;
GLuint _horizontalShader_nId;
GLuint _horizontalShader_widthId;

GLuint _matrixUpdateShader;
GLuint _matrixUpdateShader_srcLeftA_Id;
GLuint _matrixUpdateShader_srcLeftB_Id;
GLuint _matrixUpdateShader_srcRightA_Id;
GLuint _matrixUpdateShader_srcRightB_Id;
GLuint _matrixUpdateShader_flowId;
GLuint _matrixUpdateShader_heightId;
GLuint _matrixUpdateShader_widthId;
GLuint _matrixUpdateShader_hsId;
GLuint _matrixUpdateShader_wsId;

GLuint _verticalGaussBlur;
GLuint _verticalGaussBlur_srcA_Id;
GLuint _verticalGaussBlur_srcB_Id;
GLuint _verticalGaussBlur_hs_Id;
GLuint _verticalGaussBlur_kernel_Id;
GLuint _verticalGaussBlur_m_Id;

GLuint _horizontalGaussBlur;
GLuint _horizontalGaussBlur_srcA_Id;
GLuint _horizontalGaussBlur_srcB_Id;
GLuint _horizontalGaussBlur_ws_Id;
GLuint _horizontalGaussBlur_kernel_Id;
GLuint _horizontalGaussBlur_m_Id;

GLuint _flowUpdateShader;
GLuint _flowUpdateShader_srcA_Id;
GLuint _flowUpdateShader_srcB_Id;

int _kernelSize = 10; //If changed, adjust accordingly in GaussBlur Shaders (uniform float kernel[])
float* _kernel;

GLuint _copyShader;
GLuint _copyShader_srcId;

int main(void)
{
	Init(1800, 1216);

	//For now, later replace with LoadFrames!-------

	//glActiveTexture(GL_TEXTURE0);
	_textures[FrameLeft] = loadBMP_custom("Murmeln_LEFT.bmp");
	//glActiveTexture(GL_TEXTURE1);
	_textures[FrameRight] = loadBMP_custom("Murmeln_RIGHT.bmp");
	//-----------------------------------------------
	
	do
	{
		ExecuteShaders();

		glfwSwapBuffers(window);
		glfwPollEvents();
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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glGenVertexArrays(1, &_vertexArrayId);
	glBindVertexArray(_vertexArrayId);

	glGenFramebuffers(1, &_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

	_textures = new GLuint[_numTextures];
	_attachments = new GLenum[2];
	_attachments[0] = GL_COLOR_ATTACHMENT0;
	_attachments[1] = GL_COLOR_ATTACHMENT1;
	glGenTextures(_numTextures, _textures);

	//Init textures
	//glActiveTexture(GL_TEXTURE2);
	for (int i = _numTextures - 1; i >= 0; i -= 1)
	{
		glBindTexture(GL_TEXTURE_2D, _textures[i]);
		easyTexImage2D(_imageWidth, _imageHeight, GetDimension(i));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[Workspace], 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	InitQuadVertexBuffer();
	InitGaussKernel();

	/*_shader0 = LoadShaders("Passthrough.vertexshader", "TestShader.fragmentshader");
	_shader0param_sourceLeftId = glGetUniformLocation(_shader0, "sourceLeft");
	_shader0param_sourceRightId = glGetUniformLocation(_shader0, "sourceRight");
	_shader0param_countId = glGetUniformLocation(_shader0, "c");
	glUniform1i(_shader0param_sourceLeftId, 0);
	glUniform1i(_shader0param_sourceRightId, 1);

	_shader1 = LoadShaders("Passthrough.vertexshader", "WobblyTexture.fragmentshader");
	_shader1param_sourceId = glGetUniformLocation(_shader1, "source");
	glUniform1i(_shader1param_sourceId, 2);*/

	//_verticalShader = LoadShaders("Passthrough.vertexshader", "WobblyTexture.fragmentshader");
	_verticalShader = LoadShaders("Passthrough.vertexshader", "VerticalConvolution.fragmentshader");
	_verticalShader_srcId = glGetUniformLocation(_verticalShader, "src");
	_verticalShader_gId = glGetUniformLocation(_verticalShader, "g");
	_verticalShader_xgId = glGetUniformLocation(_verticalShader, "xg");
	_verticalShader_xxgId = glGetUniformLocation(_verticalShader, "xxg");
	_verticalShader_nId = glGetUniformLocation(_verticalShader, "n");
	_verticalShader_heightId = glGetUniformLocation(_verticalShader, "height");
	_verticalShader_hsId = glGetUniformLocation(_verticalShader, "hs");

	//_horizontalShader = LoadShaders("Passthrough.vertexshader", "WobblyTexture.fragmentshader");
	_horizontalShader = LoadShaders("Passthrough.vertexshader", "HorizontalConvolution.fragmentshader");
	_horizontalShader_srcId = glGetUniformLocation(_horizontalShader, "src");
	_horizontalShader_gId = glGetUniformLocation(_horizontalShader, "g");
	_horizontalShader_xgId = glGetUniformLocation(_horizontalShader, "xg");
	_horizontalShader_xxgId = glGetUniformLocation(_horizontalShader, "xxg");
	_horizontalShader_id11Id = glGetUniformLocation(_horizontalShader, "ig11");
	_horizontalShader_id03Id = glGetUniformLocation(_horizontalShader, "ig03");
	_horizontalShader_id33Id = glGetUniformLocation(_horizontalShader, "ig33");
	_horizontalShader_id55Id = glGetUniformLocation(_horizontalShader, "ig55");
	_horizontalShader_nId = glGetUniformLocation(_horizontalShader, "n");
	_horizontalShader_widthId = glGetUniformLocation(_horizontalShader, "width");

	_matrixUpdateShader = LoadShaders("Passthrough.vertexshader", "MatrixUpdate.fragmentshader");
	_matrixUpdateShader_srcLeftA_Id = glGetUniformLocation(_matrixUpdateShader, "srcLeftA");
	_matrixUpdateShader_srcLeftB_Id = glGetUniformLocation(_matrixUpdateShader, "srcLeftB");
	_matrixUpdateShader_srcRightA_Id = glGetUniformLocation(_matrixUpdateShader, "srcRightA");
	_matrixUpdateShader_srcRightB_Id = glGetUniformLocation(_matrixUpdateShader, "srcRightB");
	_matrixUpdateShader_flowId = glGetUniformLocation(_matrixUpdateShader, "flow");
	_matrixUpdateShader_heightId = glGetUniformLocation(_matrixUpdateShader, "height");
	_matrixUpdateShader_widthId = glGetUniformLocation(_matrixUpdateShader, "width");
	_matrixUpdateShader_hsId = glGetUniformLocation(_matrixUpdateShader, "hs");
	_matrixUpdateShader_wsId = glGetUniformLocation(_matrixUpdateShader, "ws");

	_verticalGaussBlur = LoadShaders("Passthrough.vertexshader", "VerticalGaussBlur.fragmentshader");
	_verticalGaussBlur_srcA_Id = glGetUniformLocation(_verticalGaussBlur, "srcA");
	_verticalGaussBlur_srcB_Id = glGetUniformLocation(_verticalGaussBlur, "srcB");
	_verticalGaussBlur_hs_Id = glGetUniformLocation(_verticalGaussBlur, "hs");
	_verticalGaussBlur_kernel_Id = glGetUniformLocation(_verticalGaussBlur, "kernel");
	_verticalGaussBlur_m_Id = glGetUniformLocation(_verticalGaussBlur, "m");

	_horizontalGaussBlur = LoadShaders("Passthrough.vertexshader", "HorizontalGaussBlur.fragmentshader");
	_horizontalGaussBlur_srcA_Id = glGetUniformLocation(_horizontalGaussBlur, "srcA");
	_horizontalGaussBlur_srcB_Id = glGetUniformLocation(_horizontalGaussBlur, "srcB");
	_horizontalGaussBlur_ws_Id = glGetUniformLocation(_horizontalGaussBlur, "ws");
	_horizontalGaussBlur_kernel_Id = glGetUniformLocation(_horizontalGaussBlur, "kernel");
	_horizontalGaussBlur_m_Id = glGetUniformLocation(_horizontalGaussBlur, "m");

	_flowUpdateShader = LoadShaders("Passthrough.vertexshader", "FlowUpdateShader.fragmentshader");
	_flowUpdateShader_srcA_Id = glGetUniformLocation(_flowUpdateShader, "srcA");
	_flowUpdateShader_srcB_Id = glGetUniformLocation(_flowUpdateShader, "srcB");

	_copyShader = LoadShaders("Passthrough.vertexshader", "WobblyTexture.fragmentshader");
	_copyShader_srcId = glGetUniformLocation(_copyShader, "src");
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
	glBindTexture(GL_TEXTURE_2D, _textures[GetNewFrameSlot()]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _imageWidth, _imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, frame);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	ExecuteVerticalConvolutionShader(GetNewFrameSlot(), Workspace);
	ExecuteHorizontalConvolutionShader(Workspace, GetNewPolyExpSlotA(), GetNewPolyExpSlotB());
}

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

void CalculateOpticalFlow()
{
	ExecuteMatrixUpdateShader(GetCurrentPolyExpLeftA(), GetCurrentPolyExpLeftB(), GetCurrentPolyExpRightA(), GetCurrentPolyExpRightB(), Flow, UpdateMatrixA, UpdateMatrixB);
}

void ExecuteShaders()
{
	//Only for testing, later no need to set every time
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
	//-------------------------------------------------
	ExecuteVerticalConvolutionShader(GetCurrentFrameLeft(), Workspace);
	ExecuteHorizontalConvolutionShader(Workspace, GetCurrentPolyExpLeftA(), GetCurrentPolyExpLeftB());
	ExecuteVerticalConvolutionShader(GetCurrentFrameRight(), Workspace);
	ExecuteHorizontalConvolutionShader(Workspace, GetCurrentPolyExpRightA(), GetCurrentPolyExpRightB());
	ExecuteMatrixUpdateShader(GetCurrentPolyExpLeftA(), GetCurrentPolyExpLeftB(), GetCurrentPolyExpRightA(), GetCurrentPolyExpRightB(), Flow, UpdateMatrixA, UpdateMatrixB);

	for (int i = 0; i < 1; i += 1) {
		ExecuteVerticalGaussBlur(UpdateMatrixA, UpdateMatrixB, Workspace, Workspace2);
		ExecuteHorizontalGaussBlur(Workspace, Workspace2, BlurA, BlurB);
		ExecuteFlowUpdateShader(BlurA, BlurB, Flow);
		//ExecuteMatrixUpdateShader(GetCurrentPolyExpLeftA(), GetCurrentPolyExpLeftB(), GetCurrentPolyExpRightA(), GetCurrentPolyExpRightB(), Flow, UpdateMatrixA, UpdateMatrixB);
	}

	//Only for testing, later no need to set every time
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//ExecuteCopyShader(UPDATEMATRIX, UpdateMatrix);
	ExecuteCopyShader(Flow);
	//ExecuteCopyShader(UpdateMatrix);
	//-------------------------------------------------
}

void PrepareShader()
{
	glViewport(0, 0, _imageWidth, _imageHeight);

	glClear(GL_COLOR_BUFFER_BIT);
}

void FinalizeShader()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _quadVertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	glDisableVertexAttribArray(0);
}

float count = 1.f;
bool subtract = true;

void ExecuteVerticalConvolutionShader(int textureSource, int destination)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destination], 0);
	PrepareShader();

	glUseProgram(_verticalShader);
	glActiveTexture(GetTextureActiveSpace(textureSource));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSource]);
	glUniform1i(_verticalShader_srcId, GetActiveSpaceLocation(textureSource));
	float g[8] = { 0.265962, 0.212965, 0.10934, 0.035994, 0.00759733, 0.00102819, 8.92202e-05, 4.96403e-06};
	float xg[8] = { 0, 0.212965, 0.21868, 0.107982, 0.0303893, 0.00514093, 0.000535321, 3.47482e-05};
	float xxg[8] = { 0, 0.212965, 0.43736, 0.323946, 0.121557, 0.0257047, 0.00321193, 0.000243238 };
	glUniform1fv(_verticalShader_gId, 8, g);
	glUniform1fv(_verticalShader_xgId, 8, xg);
	glUniform1fv(_verticalShader_xxgId, 8, xxg);
	glUniform1i(_verticalShader_nId, 7);
	glUniform1f(_verticalShader_heightId, (float) _imageHeight);
	glUniform1f(_verticalShader_hsId, ((float) 1) / ((float) _imageHeight));

	FinalizeShader();
}

void ExecuteHorizontalConvolutionShader(int textureSource, int destinationA, int destinationB)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destinationA], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _textures[destinationB], 0);
	glDrawBuffers(2, _attachments);
	PrepareShader();

	glUseProgram(_horizontalShader);
	glActiveTexture(GetTextureActiveSpace(textureSource));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSource]);
	glUniform1i(_horizontalShader_srcId, GetActiveSpaceLocation(textureSource));
	float g[8] = { 0.265962, 0.212965, 0.10934, 0.035994, 0.00759733, 0.00102819, 8.92202e-05, 4.96403e-06 };
	float xg[8] = { 0, 0.212965, 0.21868, 0.107982, 0.0303893, 0.00514093, 0.000535321, 3.47482e-05 };
	float xxg[8] = { 0, 0.212965, 0.43736, 0.323946, 0.121557, 0.0257047, 0.00321193, 0.000243238 };
	float ig11 = 0.444449;
	float ig03 = -0.222251;
	float ig33 = 0.0987791;
	float ig55 = ig33;
	glUniform1fv(_horizontalShader_gId, 8, g);
	glUniform1fv(_horizontalShader_xgId, 8, xg);
	glUniform1fv(_horizontalShader_xxgId, 8, xxg);
	glUniform1f(_horizontalShader_id11Id, ig11);
	glUniform1f(_horizontalShader_id03Id, ig03);
	glUniform1f(_horizontalShader_id33Id, ig33);
	glUniform1f(_horizontalShader_id55Id, ig55);
	glUniform1i(_horizontalShader_nId, 7);
	glUniform1f(_horizontalShader_widthId, (float) _imageWidth);

	FinalizeShader();
}

void ExecuteMatrixUpdateShader(int textureSourceLeftA, int textureSourceLeftB, int textureSourceRightA, int textureSourceRightB, int flowSource, int destinationA, int destinationB)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destinationA], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _textures[destinationB], 0);
	glDrawBuffers(2, _attachments);
	PrepareShader();

	glUseProgram(_matrixUpdateShader);

	glActiveTexture(GetTextureActiveSpace(textureSourceLeftA));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSourceLeftA]);
	glUniform1i(_matrixUpdateShader_srcLeftA_Id, GetActiveSpaceLocation(textureSourceLeftA));

	glActiveTexture(GetTextureActiveSpace(textureSourceLeftB));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSourceLeftB]);
	glUniform1i(_matrixUpdateShader_srcLeftB_Id, GetActiveSpaceLocation(textureSourceLeftB));

	glActiveTexture(GetTextureActiveSpace(textureSourceRightA));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSourceRightA]);
	glUniform1i(_matrixUpdateShader_srcRightA_Id, GetActiveSpaceLocation(textureSourceRightA));

	glActiveTexture(GetTextureActiveSpace(textureSourceRightB));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSourceRightB]);
	glUniform1i(_matrixUpdateShader_srcRightB_Id, GetActiveSpaceLocation(textureSourceRightB));

	glActiveTexture(GetTextureActiveSpace(flowSource));
	glBindTexture(GL_TEXTURE_2D, _textures[flowSource]);
	glUniform1i(_matrixUpdateShader_flowId, GetActiveSpaceLocation(flowSource));

	glUniform1f(_matrixUpdateShader_heightId, (float) _imageHeight);
	glUniform1f(_matrixUpdateShader_widthId, (float) _imageWidth);
	glUniform1f(_matrixUpdateShader_hsId, 1.f / ((float) _imageHeight));
	glUniform1f(_matrixUpdateShader_wsId, 1.f / ((float) _imageWidth));


	FinalizeShader();
}

void ExecuteVerticalGaussBlur(int textureSourceA, int textureSourceB, int destinationA, int destinationB)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destinationA], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _textures[destinationB], 0);
	glDrawBuffers(2, _attachments);
	PrepareShader();

	glUseProgram(_verticalGaussBlur);

	glActiveTexture(GetTextureActiveSpace(textureSourceA));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSourceA]);
	glUniform1i(_verticalGaussBlur_srcA_Id, GetActiveSpaceLocation(textureSourceA));

	glActiveTexture(GetTextureActiveSpace(textureSourceB));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSourceB]);
	glUniform1i(_verticalGaussBlur_srcB_Id, GetActiveSpaceLocation(textureSourceB));

	glUniform1f(_verticalGaussBlur_hs_Id, ((float) 1) / ((float) _imageHeight));
	glUniform1fv(_verticalGaussBlur_kernel_Id, _kernelSize, _kernel);
	glUniform1i(_verticalGaussBlur_m_Id, _kernelSize);

	FinalizeShader();
}

void ExecuteHorizontalGaussBlur(int textureSourceA, int textureSourceB, int destinationA, int destinationB)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destinationA], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _textures[destinationB], 0);
	glDrawBuffers(2, _attachments);
	PrepareShader();

	glUseProgram(_horizontalGaussBlur);

	glActiveTexture(GetTextureActiveSpace(textureSourceA));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSourceA]);
	glUniform1i(_horizontalGaussBlur_srcA_Id, GetActiveSpaceLocation(textureSourceA));

	glActiveTexture(GetTextureActiveSpace(textureSourceB));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSourceB]);
	glUniform1i(_horizontalGaussBlur_srcB_Id, GetActiveSpaceLocation(textureSourceB));

	glUniform1f(_horizontalGaussBlur_ws_Id, ((float) 1) / ((float) _imageWidth));
	glUniform1fv(_horizontalGaussBlur_kernel_Id, _kernelSize, _kernel);
	glUniform1i(_horizontalGaussBlur_m_Id, _kernelSize);

	FinalizeShader();
}

void ExecuteFlowUpdateShader(int textureSourceA, int textureSourceB, int destination)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destination], 0);
	PrepareShader();

	glUseProgram(_flowUpdateShader);

	glActiveTexture(GetTextureActiveSpace(textureSourceA));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSourceA]);
	glUniform1i(_horizontalGaussBlur_srcA_Id, GetActiveSpaceLocation(textureSourceA));

	glActiveTexture(GetTextureActiveSpace(textureSourceB));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSourceB]);
	glUniform1i(_horizontalGaussBlur_srcB_Id, GetActiveSpaceLocation(textureSourceB));

	FinalizeShader();
}

void ExecuteCopyShader(int textureSource)
{
	PrepareShader();

	glUseProgram(_copyShader);
	glActiveTexture(GetTextureActiveSpace(textureSource));
	glBindTexture(GL_TEXTURE_2D, _textures[textureSource]);
	glUniform1i(_copyShader_srcId, GetActiveSpaceLocation(textureSource));

	FinalizeShader();
}

void easyTexImage2D(int width, int height, int dimension) 
{
	if (dimension == 2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, 0);
	}
	if (dimension == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
	}
}

static const GLfloat _gQuadVertexBufferData[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
};

void InitQuadVertexBuffer() {
	glGenBuffers(1, &_quadVertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _quadVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_gQuadVertexBufferData), _gQuadVertexBufferData, GL_STATIC_DRAW);
}

void InitGaussKernel()
{
	_kernel = new float[_kernelSize];

	double twoSigmaSquare = 2 * _kernelSize * _kernelSize * 0.09;
	double s = 1;
	_kernel[0] = (float) s;
	for (int i = 1; i < _kernelSize; i += 1)
	{
		float t = (float) std::exp(-i * i / twoSigmaSquare);
		_kernel[i] = t;// / std::sqrt(twoSigmaSquare * 3.1415926535897932);
		s += t * 2;
	}

	s = 1. / s;
	for (int i = 0; i < _kernelSize; i += 1)
	{
		_kernel[i] = _kernel[i] * s;
		float p = _kernel[i];
		int s = 0;
	}
}

void CleanUp()
{
	glDeleteFramebuffers(1, &_framebuffer);
	glDeleteTextures(_numTextures, _textures);
	glDeleteBuffers(1, &_quadVertexbuffer);
	glDeleteVertexArrays(1, &_vertexArrayId);
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