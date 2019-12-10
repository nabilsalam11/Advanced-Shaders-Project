/*

Example9.h

*/
#pragma once
//  for M_PI constant
#define _USE_MATH_DEFINES
#include <cmath>

#include "Win32OpenGL.h"
#include "resource.h"
#include "ModelLoader.h"
#include "Log.h"
#include "camera.h"

class Background
{
	// open gl helper class
	Win32OpenGL m_win32OpenGL;

	Camera m_camera;

	// matrices
	//mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;
	//glm::mat4 m_modelMatrix;


	bool m_withStencilBuffer{ true };

	//new for textures - the id for the actual bitmap data
	//GLuint m_textureDataID;


	// model data
	vector<float> m_verticesGround;			// we can't initialise at compile time
	vector<float> m_normalsGround;
	// new for texture
	vector<float> m_textureUVGround;

	// model data
	vector<float> m_verticesModel1;			// we can't initialise at compile time
	vector<float> m_normalsModel1;
	// new for texture
	vector<float> m_textureUVModel1;

	// model data
	vector<float> m_verticesModel2;			// we can't initialise at compile time
	vector<float> m_normalsModel2;
	// new for texture
	vector<float> m_textureUVModel2;

	// ids for vao and vbo's
	GLuint m_vaoModelGround;
	GLuint m_vboVerticesGround;
	GLuint m_vboNormalsGround;
	// new for textures
	GLuint m_vboTexturesGround;


	// ids for vao and vbo's
	GLuint m_vaoModel1;
	GLuint m_vboVerticesModel1;
	GLuint m_vboNormalsModel1;
	// new for textures
	GLuint m_vboTexturesModel1;


	// ids for vao and vbo's
	GLuint m_vaoModel2;
	GLuint m_vboVerticesModel2;
	GLuint m_vboNormalsModel2;
	// new for textures
	GLuint m_vboTexturesModel2;



	//new for textures - the id for the actual bitmap data
	GLuint m_textureDataIDModelGround;
	GLuint m_textureDataIDModel1;
	GLuint m_textureDataIDModel2;

	// *** NEW **************

	GLuint m_flatShader;
	GLuint m_phongTextureShader;

	// each instance has a model matrix
	glm::mat4 m_modelMatrixGround;
	glm::mat4 m_modelMatrixModel1;
	glm::mat4 m_modelMatrixModel2;

	// ground is at (0,0,0)
	glm::vec3 m_positionModel1{ 0.0f, 2.5f, 0.0f };
	glm::vec3 m_positionModel2{ 2.0f, 2.5f, 0.0f };


	// this is same as before
	float m_xAngle{ 0.0f };
	float m_yAngle{ 0.0f };
	float m_zAngle{ 0.0f };
	float m_aspectRatio;

	// and this is from the lighting
	glm::vec3 m_lightColourSpecular{ 1.0f,1.0f,1.0f };
	glm::vec3 m_lightColourDiffuse{ 0.8f, 0.8f, 0.8f };
	glm::vec3 m_lightColourAmbient{ 0.3f, 0.3f, 0.3f };
	glm::vec3 m_lightPosition{ 10.0f, 10.0f, 10.0f };

public:
	Background();
	~Background();

	void CreateGLWindow(HWND hWnd, RECT rect);
	void DestroyGLWindow();

	void PrepareToDraw();
	void Draw();

	void Update();

	void HandleInput(unsigned char virtualKeyCode);
	void HandleMouseInput(int xPosRelative, int yPosRelative);

	void Resize(HDC hdc, RECT rect);

	void ComputeProjectionMatrix();
	//void ComputeViewMatrix();

	///bool SimpleLoadTexture(string fileName, char* imageData);
};