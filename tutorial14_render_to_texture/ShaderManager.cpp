
#include <iostream>
#include <GL/glew.h>
#include <glfw3.h>
#include <common/shader.hpp>

#include "Constants.h"
#include "ShaderManager.h"

GLuint* _textures;
GLenum* _attachments;
GLuint _quadVertexbuffer;

int _imageWidthM;
int _imageHeightM;

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
GLuint _copyShader_mulId;

void ShaderInitialize(int imageWidth, int imageHeight)
{
	_imageWidthM = imageWidth;
	_imageHeightM = imageHeight;

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
	_copyShader_mulId = glGetUniformLocation(_copyShader, "mul");

	_attachments = new GLenum[2];
	_attachments[0] = GL_COLOR_ATTACHMENT0;
	_attachments[1] = GL_COLOR_ATTACHMENT1;

	_textures = new GLuint[NumTextures];
	glGenTextures(NumTextures, _textures);

	//Init _textures
	//glActiveTexture(GL_TEXTURE2);
	for (int i = NumTextures - 1; i >= 0; i -= 1)
	{
		glBindTexture(GL_TEXTURE_2D, _textures[i]);
		easyTexImage2D(_imageWidthM, _imageHeightM, GetDimension(i));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	InitQuadVertexBuffer();
	InitGaussKernel();
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

void PrepareShader()
{
	glViewport(0, 0, _imageWidthM, _imageHeightM);

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
		(void*) 0            // array buffer offset
	);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	glDisableVertexAttribArray(0);
}

void ExecuteVerticalConvolutionShader(int _texturesource, int destination)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destination], 0);
	glDrawBuffers(1, _attachments);
	PrepareShader();

	glUseProgram(_verticalShader);
	glActiveTexture(GetTextureActiveSpace(_texturesource));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesource]);
	glUniform1i(_verticalShader_srcId, GetActiveSpaceLocation(_texturesource));
	float g[8] = { 0.265962, 0.212965, 0.10934, 0.035994, 0.00759733, 0.00102819, 8.92202e-05, 4.96403e-06 };
	float xg[8] = { 0, 0.212965, 0.21868, 0.107982, 0.0303893, 0.00514093, 0.000535321, 3.47482e-05 };
	float xxg[8] = { 0, 0.212965, 0.43736, 0.323946, 0.121557, 0.0257047, 0.00321193, 0.000243238 };
	glUniform1fv(_verticalShader_gId, 8, g);
	glUniform1fv(_verticalShader_xgId, 8, xg);
	glUniform1fv(_verticalShader_xxgId, 8, xxg);
	glUniform1i(_verticalShader_nId, 7);
	glUniform1f(_verticalShader_heightId, (float) _imageHeightM);
	glUniform1f(_verticalShader_hsId, ((float) 1) / ((float) _imageHeightM));

	FinalizeShader();
}

void ExecuteHorizontalConvolutionShader(int _texturesource, int destinationA, int destinationB)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destinationA], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _textures[destinationB], 0);
	glDrawBuffers(2, _attachments);
	PrepareShader();

	glUseProgram(_horizontalShader);
	glActiveTexture(GetTextureActiveSpace(_texturesource));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesource]);
	glUniform1i(_horizontalShader_srcId, GetActiveSpaceLocation(_texturesource));
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
	glUniform1f(_horizontalShader_widthId, (float) _imageWidthM);

	FinalizeShader();
}

void ExecuteMatrixUpdateShader(int _texturesourceLeftA, int _texturesourceLeftB, int _texturesourceRightA, int _texturesourceRightB, int flowSource, int destinationA, int destinationB)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destinationA], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _textures[destinationB], 0);
	glDrawBuffers(2, _attachments);
	PrepareShader();

	glUseProgram(_matrixUpdateShader);

	glActiveTexture(GetTextureActiveSpace(_texturesourceLeftA));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesourceLeftA]);
	glUniform1i(_matrixUpdateShader_srcLeftA_Id, GetActiveSpaceLocation(_texturesourceLeftA));

	glActiveTexture(GetTextureActiveSpace(_texturesourceLeftB));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesourceLeftB]);
	glUniform1i(_matrixUpdateShader_srcLeftB_Id, GetActiveSpaceLocation(_texturesourceLeftB));

	glActiveTexture(GetTextureActiveSpace(_texturesourceRightA));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesourceRightA]);
	glUniform1i(_matrixUpdateShader_srcRightA_Id, GetActiveSpaceLocation(_texturesourceRightA));

	glActiveTexture(GetTextureActiveSpace(_texturesourceRightB));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesourceRightB]);
	glUniform1i(_matrixUpdateShader_srcRightB_Id, GetActiveSpaceLocation(_texturesourceRightB));

	glActiveTexture(GetTextureActiveSpace(flowSource));
	glBindTexture(GL_TEXTURE_2D, _textures[flowSource]);
	glUniform1i(_matrixUpdateShader_flowId, GetActiveSpaceLocation(flowSource));

	glUniform1f(_matrixUpdateShader_heightId, (float) _imageHeightM);
	glUniform1f(_matrixUpdateShader_widthId, (float) _imageWidthM);
	glUniform1f(_matrixUpdateShader_hsId, 1.f / ((float) _imageHeightM));
	glUniform1f(_matrixUpdateShader_wsId, 1.f / ((float) _imageWidthM));

	FinalizeShader();
}

void ExecuteVerticalGaussBlur(int _texturesourceA, int _texturesourceB, int destinationA, int destinationB)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destinationA], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _textures[destinationB], 0);
	glDrawBuffers(2, _attachments);
	PrepareShader();

	glUseProgram(_verticalGaussBlur);

	glActiveTexture(GetTextureActiveSpace(_texturesourceA));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesourceA]);
	glUniform1i(_verticalGaussBlur_srcA_Id, GetActiveSpaceLocation(_texturesourceA));

	glActiveTexture(GetTextureActiveSpace(_texturesourceB));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesourceB]);
	glUniform1i(_verticalGaussBlur_srcB_Id, GetActiveSpaceLocation(_texturesourceB));

	glUniform1f(_verticalGaussBlur_hs_Id, ((float) 1) / ((float) _imageHeightM));
	glUniform1fv(_verticalGaussBlur_kernel_Id, _kernelSize, _kernel);
	glUniform1i(_verticalGaussBlur_m_Id, _kernelSize);

	FinalizeShader();
}

void ExecuteHorizontalGaussBlur(int _texturesourceA, int _texturesourceB, int destinationA, int destinationB)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destinationA], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _textures[destinationB], 0);
	glDrawBuffers(2, _attachments);
	PrepareShader();

	glUseProgram(_horizontalGaussBlur);

	glActiveTexture(GetTextureActiveSpace(_texturesourceA));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesourceA]);
	glUniform1i(_horizontalGaussBlur_srcA_Id, GetActiveSpaceLocation(_texturesourceA));

	glActiveTexture(GetTextureActiveSpace(_texturesourceB));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesourceB]);
	glUniform1i(_horizontalGaussBlur_srcB_Id, GetActiveSpaceLocation(_texturesourceB));

	glUniform1f(_horizontalGaussBlur_ws_Id, ((float) 1) / ((float) _imageWidthM));
	glUniform1fv(_horizontalGaussBlur_kernel_Id, _kernelSize, _kernel);
	glUniform1i(_horizontalGaussBlur_m_Id, _kernelSize);

	FinalizeShader();
}

void ExecuteFlowUpdateShader(int _texturesourceA, int _texturesourceB, int destination)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destination], 0);
	glDrawBuffers(1, _attachments);
	PrepareShader();

	glUseProgram(_flowUpdateShader);

	glActiveTexture(GetTextureActiveSpace(_texturesourceA));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesourceA]);
	glUniform1i(_horizontalGaussBlur_srcA_Id, GetActiveSpaceLocation(_texturesourceA));

	glActiveTexture(GetTextureActiveSpace(_texturesourceB));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesourceB]);
	glUniform1i(_horizontalGaussBlur_srcB_Id, GetActiveSpaceLocation(_texturesourceB));

	FinalizeShader();
}

void ExecuteCopyShader(int _texturesource, float mul, int destination)
{
	if (destination != -1)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[destination], 0);
		glDrawBuffers(1, _attachments);
	}
	PrepareShader();

	glUseProgram(_copyShader);
	glActiveTexture(GetTextureActiveSpace(_texturesource));
	glBindTexture(GL_TEXTURE_2D, _textures[_texturesource]);
	glUniform1i(_copyShader_srcId, GetActiveSpaceLocation(_texturesource));
	glUniform1f(_copyShader_mulId, mul);

	FinalizeShader();
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

void ShaderCleanup()
{
	glDeleteTextures(NumTextures, _textures);
	glDeleteBuffers(1, &_quadVertexbuffer);
}

void SetTexture(int space, GLuint value)
{
	_textures[space] = value;
}

GLuint GetTexture(int space)
{
	return _textures[space];
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