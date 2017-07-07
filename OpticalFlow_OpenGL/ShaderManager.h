#pragma once;

void ShaderInitialize(int imageWidth, int imageHeight);
void ShaderCleanup();

void ExecuteVerticalConvolutionShader(int textureSource, int destination);
void ExecuteHorizontalConvolutionShader(int textureSource, int destinationA, int destinationB);
void ExecuteMatrixUpdateShader(int textureSourceLeftA, int textureSourceLeftB, int textureSourceRightA, int textureSourceRightB, int flowSource, int destinationA, int destinationB, bool fake);
void ExecuteVerticalGaussBlur(int textureSourceA, int textureSourceB, int destinationA, int destinationB);
void ExecuteHorizontalGaussBlur(int textureSourceA, int textureSourceB, int destinationA, int destinationB);
void ExecuteFlowUpdateShader(int textureSourceA, int textureSourceB, int destination);
void ExecuteCopyShader(int textureSource, float mul, int destination);
void PrepareShader();
void FinalizeShader();
void easyTexImage2D(int width, int height, int dimension);

void ActivateTexture(int textureSource, GLuint sourceId);

void InitQuadVertexBuffer();
void InitGaussKernel();

void SetTexture(int space, GLuint value);
GLuint GetTexture(int space);