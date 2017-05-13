//// Include standard headers
//#include <stdio.h>
//#include <stdlib.h>
//#include <vector>
//
////#include <png.h>
//
//// Include GLEW
//#include <GL/glew.h>
//
//// Include GLFW
//#include <glfw3.h>
//GLFWwindow* window;
//
//// Include GLM
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//using namespace glm;
//
//#include <common/shader.hpp>
//#include <common/texture.hpp>
//#include <common/controls.hpp>
//#include <common/objloader.hpp>
//#include <common/vboindexer.hpp>
//
//#include "OpenGL.h";
//
//GLuint quad_vertexbuffer;
//
//int imageWidth;
//int imageHeight;
//
//
////void* loadBMP_custom(const char *imagepath);
//
//int main(void)
//{
//	// Initialise GLFW
//	if (!glfwInit())
//	{
//		fprintf(stderr, "Failed to initialize GLFW\n");
//		getchar();
//		return -1;
//	}
//
//	glfwWindowHint(GLFW_SAMPLES, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	// Open a window and create its OpenGL context
//	window = glfwCreateWindow(1024, 768, "Tutorial 14 - Render To Texture", NULL, NULL);
//	if (window == NULL) {
//		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
//		getchar();
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//
//	// We would expect width and height to be 1024 and 768
//	int windowWidth = 1024;
//	int windowHeight = 768;
//	// But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size:
//	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
//
//	// Initialize GLEW
//	glewExperimental = true; // Needed for core profile
//	if (glewInit() != GLEW_OK) {
//		fprintf(stderr, "Failed to initialize GLEW\n");
//		getchar();
//		glfwTerminate();
//		return -1;
//	}
//
//	// Ensure we can capture the escape key being pressed below
//	//glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
//
//	// Dark blue background
//	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
//
//	// Cull triangles which normal is not towards the camera
//	glEnable(GL_CULL_FACE);
//
//	GLuint VertexArrayID;
//	glGenVertexArrays(1, &VertexArrayID);
//	glBindVertexArray(VertexArrayID);
//
//	// ---------------------------------------------
//	// Render to Texture - specific code begins here
//	// ---------------------------------------------
//
//	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
//	GLuint Framebuffer = 0;
//	glGenFramebuffers(1, &Framebuffer);
//	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
//
//	// The texture we're going to render to
//	GLuint renderedTexture;
//	glGenTextures(1, &renderedTexture);
//
//	// "Bind" the newly created texture : all future texture functions will modify this texture
//	glBindTexture(GL_TEXTURE_2D, renderedTexture);
//
//	// Give an empty image to OpenGL ( the last "0" means "empty" )
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
//
//	// Poor filtering
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	// Set "renderedTexture" as our colour attachement #0
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
//
//
//	// Set the list of draw buffers.
//	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
//
//	// Always check that our framebuffer is ok
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		return false;
//
//	InitQuadVertexBuffer();
//
//	// Create and compile our GLSL program from the shaders
//	GLuint quad_programID = LoadShaders("Passthrough.vertexshader", "TestShader.fragmentshader");
//	GLuint sourceLeftID = glGetUniformLocation(quad_programID, "sourceLeft");
//	GLuint sourceRightID = glGetUniformLocation(quad_programID, "sourceRight");
//	GLuint countID = glGetUniformLocation(quad_programID, "c");
//
//	GLuint copyProgramID = LoadShaders("Passthrough.vertexshader", "WobblyTexture.fragmentshader");
//	GLuint sourceID = glGetUniformLocation(copyProgramID, "source");
//
//	GLuint *frames = new GLuint[2];
//	glGenTextures(2, frames);
//
//	frames[0] = loadBMP_custom("Murmeln_LEFT.bmp");
//	frames[1] = loadBMP_custom("Murmeln_RIGHT.bmp");
//
//	glBindTexture(GL_TEXTURE_2D, renderedTexture);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
//
//	//glBindTexture(GL_TEXTURE_2D, frames[0]);
//	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
//	float count = 1.f;
//	bool subtract = true;
//	do {
//		// Render to the screen
//		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
//		// Render on the whole framebuffer, complete from the lower left corner to the upper right
//		glViewport(0, 0, windowWidth, windowHeight);
//
//		// Clear the screen
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// Use our shader
//		glUseProgram(quad_programID);
//		glUniform1f(countID, count);
//		if (count < 0.1f)
//		{
//			subtract = false;
//		}
//		else if (count > 0.9f)
//		{
//			subtract = true;
//		}
//		if (subtract) 
//		{
//			count -= 0.0001f;
//		}
//		else
//		{
//			count += 0.0001f;
//		}
//
//		// Bind our texture in Texture Unit 0
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, frames[0]);
//		glUniform1i(sourceLeftID, 0);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, frames[1]);
//		glUniform1i(sourceRightID, 1);
//
//		// 1rst attribute buffer : vertices
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
//		glVertexAttribPointer(
//			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//			3,                  // size
//			GL_FLOAT,           // type
//			GL_FALSE,           // normalized?
//			0,                  // stride
//			(void*)0            // array buffer offset
//		);
//
//		// Draw the triangles !
//		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
//
//		glDisableVertexAttribArray(0);
//
//
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glViewport(0, 0, windowWidth, windowHeight);
//
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		glUseProgram(copyProgramID);
//		glActiveTexture(GL_TEXTURE2);
//		glBindTexture(GL_TEXTURE_2D, renderedTexture);
//		glUniform1i(sourceID, 2);
//
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
//		glVertexAttribPointer(
//			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//			3,                  // size
//			GL_FLOAT,           // type
//			GL_FALSE,           // normalized?
//			0,                  // stride
//			(void*)0            // array buffer offset
//		);
//
//		// Draw the triangles !
//		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
//
//		glDisableVertexAttribArray(0);
//
//		// Swap buffers
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//
//	} // Check if the ESC key was pressed or the window was closed
//	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
//		glfwWindowShouldClose(window) == 0);
//
//	// Cleanup VBO and shader
//
//	glDeleteFramebuffers(1, &Framebuffer);
//	glDeleteTextures(1, &renderedTexture);
//	glDeleteBuffers(1, &quad_vertexbuffer);
//	glDeleteVertexArrays(1, &VertexArrayID);
//
//
//	// Close OpenGL window and terminate GLFW
//	glfwTerminate();
//
//	return 0;
//}
//
//static const GLfloat g_quad_vertex_buffer_data[] = {
//	-1.0f, -1.0f, 0.0f,
//	1.0f, -1.0f, 0.0f,
//	-1.0f,  1.0f, 0.0f,
//	-1.0f,  1.0f, 0.0f,
//	1.0f, -1.0f, 0.0f,
//	1.0f,  1.0f, 0.0f,
//};
//
//void InitQuadVertexBuffer() {
//	glGenBuffers(1, &quad_vertexbuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
//}