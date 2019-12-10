#include "stdafx.h"
#include "Background.h"
#include "texture.h"



/* fixed glm library use but not using peek message and new timer yet --- TODO!!!!*/

Background::Background()
{
	// we don't use RAII currently so no action in constructor
}

Background::~Background()
{
}

void Background::CreateGLWindow(HWND hWnd, RECT rect)
{
	m_win32OpenGL.CreateGLContext(hWnd);	// may throw an exception!!!
											// MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
	m_aspectRatio = static_cast<float>(rect.right / rect.bottom);
	m_win32OpenGL.CreateShadersAndProgram("phongTexture3", m_phongTextureShader);			// with variable specular values now!
	
	m_win32OpenGL.CreateShadersAndProgram("flat32", m_flatShader);			// with variable specular values now!
																							//m_win32OpenGL.CreateShadersAndProgram("texture");

	m_win32OpenGL.SetupDisplay();
}

void Background::DestroyGLWindow()
{
	m_win32OpenGL.TearDownGLContext();
}


void Background::PrepareToDraw()
{
	ComputeProjectionMatrix();

	m_camera.ComputeDirectionVector();
	m_camera.ComputeViewMatrixUsingLookAt();

	// we compute the model matrix in the draw routine so we can animate the
	// triangle

	// send the matrixes to the shader
	//GLuint program = m_win32OpenGL.GetShaderProgram();
	
	Win32OpenGL::SendUniformMatrixToShader(m_phongTextureShader, m_projectionMatrix, "projection_matrix");
	m_camera.SetViewMatrix(m_phongTextureShader);

	Win32OpenGL::SendUniformMatrixToShader(m_flatShader, m_projectionMatrix, "projection_matrix");
	m_camera.SetViewMatrix(m_flatShader);

	Win32OpenGL::SendUniformVector3ToShader(m_phongTextureShader, m_lightPosition, "light_position_world");

	Win32OpenGL::SendUniformVector3ToShader(m_phongTextureShader, m_lightColourSpecular, "light_colour_specular");
	Win32OpenGL::SendUniformVector3ToShader(m_phongTextureShader, m_lightColourDiffuse, "light_colour_diffuse");
	Win32OpenGL::SendUniformVector3ToShader(m_phongTextureShader, m_lightColourAmbient, "light_colour_ambient");




	// this will be ignored for textures - the shader doesn't use it
	//glm::vec3 surfaceColour{ 1.0, 1.0, 0.0 };
	//Win32OpenGL::SendUniformVector3ToShader(m_phongTextureShader, surfaceColour, "surface_colour");

	// black to flat	
	//glm::vec3 surfaceColour2{ 0.0, 0.0, 0.0 };
	//Win32OpenGL::SendUniformVector3ToShader(m_flatShader, surfaceColour2, "surface_colour");





	/*if (TextureLoader::LoadBMP("Textures\\Grass1024x1024.bmp", m_textureDataIDModelGround) != 0)
	{
		Log::AppendToLogFile("Texture file not found");
		throw std::invalid_argument("Texture file not found");
	}
	//	
	if (TextureLoader::LoadBMP("Textures\\CubeWith6FacesAF.bmp", m_textureDataIDModel1) != 0)
	{
		Log::AppendToLogFile("Texture file not found");
		throw std::invalid_argument("Texture file not found");
	}
	//	
	if (TextureLoader::LoadBMP("Textures\\Granite256x256.bmp", m_textureDataIDModel2) != 0)
	{
		Log::AppendToLogFile("Texture file not found");
		throw std::invalid_argument("Texture file not found");
	}*/



	ModelLoader modelLoaderGround;
	modelLoaderGround.LoadModel("models\\desert.obj");

	m_verticesGround = modelLoaderGround.GetVertices();
	m_normalsGround = modelLoaderGround.GetNormals();

	// you will need to ensure your model conatins UV data and that the code reads and expands the textureUV values
	m_textureUVGround = modelLoaderGround.GetTextureUV();

	//GLuint vboVertices;
	glGenBuffers(1, &m_vboVerticesGround);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboVerticesGround);
	glBufferData(GL_ARRAY_BUFFER, m_verticesGround.size() * sizeof(GLfloat), &m_verticesGround[0], GL_STATIC_DRAW);

	//GLuint vboNormals;
	glGenBuffers(1, &m_vboNormalsGround);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboNormalsGround);
	glBufferData(GL_ARRAY_BUFFER, m_normalsGround.size() * sizeof(GLfloat), &m_normalsGround[0], GL_STATIC_DRAW);

	//GLuint vboTextures;
	glGenBuffers(1, &m_vboTexturesGround);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTexturesGround);
	glBufferData(GL_ARRAY_BUFFER, m_textureUVGround.size() * sizeof(GLfloat), &m_textureUVGround[0], GL_STATIC_DRAW);

	//GLuint vao;
	glGenVertexArrays(1, &m_vaoModelGround);
	glBindVertexArray(m_vaoModelGround);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboVerticesGround);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboNormalsGround);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTexturesGround);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// vertices are element 0 in VAO
	// normals element 1
	// textures element 2
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);






	ModelLoader modelLoader1;
	modelLoader1.LoadModel("models\\Moon.obj");
	m_verticesModel1 = modelLoader1.GetVertices();
	m_normalsModel1 = modelLoader1.GetNormals();
	// you will need to ensure your model conatins UV data and that the code reads and expands the textureUV values
	m_textureUVModel1 = modelLoader1.GetTextureUV();

	//GLuint vboVertices;
	glGenBuffers(1, &m_vboVerticesModel1);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboVerticesModel1);
	glBufferData(GL_ARRAY_BUFFER, m_verticesModel1.size() * sizeof(GLfloat), &m_verticesModel1[0], GL_STATIC_DRAW);

	//GLuint vboNormals;
	glGenBuffers(1, &m_vboNormalsModel1);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboNormalsModel1);
	glBufferData(GL_ARRAY_BUFFER, m_normalsModel1.size() * sizeof(GLfloat), &m_normalsModel1[0], GL_STATIC_DRAW);

	//GLuint vboTextures;
	glGenBuffers(1, &m_vboTexturesModel1);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTexturesModel1);
	glBufferData(GL_ARRAY_BUFFER, m_textureUVModel1.size() * sizeof(GLfloat), &m_textureUVModel1[0], GL_STATIC_DRAW);

	//GLuint vao;
	glGenVertexArrays(1, &m_vaoModel1);
	glBindVertexArray(m_vaoModel1);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboVerticesModel1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboNormalsModel1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTexturesModel1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// vertices are element 0 in VAO
	// normals element 1
	// textures element 2
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// ------------------- 2nd model -------------------------------------

	ModelLoader modelLoader2;
	modelLoader2.LoadModel("models\\Lowpoly_tree_sample.obj");

	m_verticesModel2 = modelLoader2.GetVertices();
	m_normalsModel2 = modelLoader2.GetNormals();

	// you will need to ensure your model conatins UV data and that the code reads and expands the textureUV values
	m_textureUVModel2 = modelLoader2.GetTextureUV();

	//GLuint vboVertices;
	glGenBuffers(1, &m_vboVerticesModel2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboVerticesModel2);
	glBufferData(GL_ARRAY_BUFFER, m_verticesModel2.size() * sizeof(GLfloat), &m_verticesModel2[0], GL_STATIC_DRAW);

	//GLuint vboNormals;
	glGenBuffers(1, &m_vboNormalsModel2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboNormalsModel2);
	glBufferData(GL_ARRAY_BUFFER, m_normalsModel2.size() * sizeof(GLfloat), &m_normalsModel2[0], GL_STATIC_DRAW);

	//GLuint vboTextures;
	glGenBuffers(1, &m_vboTexturesModel2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTexturesModel2);
	glBufferData(GL_ARRAY_BUFFER, m_textureUVModel2.size() * sizeof(GLfloat), &m_textureUVModel2[0], GL_STATIC_DRAW);

	//GLuint vao;
	glGenVertexArrays(1, &m_vaoModel2);
	glBindVertexArray(m_vaoModel2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboVerticesModel2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboNormalsModel2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTexturesModel2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// vertices are element 0 in VAO
	// normals element 1
	// textures element 2
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	////////bunny_ns

}

void Background::Draw()
{
	m_win32OpenGL.ClearGLDisplay();
	//GLuint program = m_win32OpenGL.GetShaderProgram();

	m_win32OpenGL.UseProgram(m_phongTextureShader);

	glm::vec3 surfaceSpecular{ 0,0,0};								// no specular!!!
	Win32OpenGL::SendUniformVector3ToShader(m_phongTextureShader, surfaceSpecular, "surface_specular");
	float surfacePower = 100;										// irrelevant as no mm
	Win32OpenGL::SendUniformFloatToShader(m_phongTextureShader, surfacePower, "specular_exponent");

	glBindTexture(GL_TEXTURE_2D, m_textureDataIDModelGround);
	m_modelMatrixGround = glm::mat4(1.0);
	//m_modelMatrix = translate(m_modelMatrix, vec3(0.0f, 0.0f, 0.0f));
	//m_modelMatrix = rotate_x_deg(m_modelMatrix, m_xAngle);
	//m_modelMatrix = rotate_y_deg(m_modelMatrix, m_yAngle);
	//m_modelMatrix = rotate_z_deg(m_modelMatrix, m_zAngle);
	Win32OpenGL::SendUniformMatrixToShader(m_phongTextureShader, m_modelMatrixGround, "model_matrix");

	glBindVertexArray(m_vaoModelGround);		// select first VAO
	glDrawArrays(GL_TRIANGLES, 0, m_verticesGround.size() / 3);
	glBindVertexArray(NULL);



	// cube

	glm::vec3 surfaceSpecular2{ 1,1,1 };			// full specular!!! - can try different colours
	Win32OpenGL::SendUniformVector3ToShader(m_phongTextureShader, surfaceSpecular2, "surface_specular");
	float surfacePower2 = 200;
	Win32OpenGL::SendUniformFloatToShader(m_phongTextureShader, surfacePower2, "specular_exponent");


	glBindTexture(GL_TEXTURE_2D, m_textureDataIDModel1);
	m_modelMatrixModel1 = glm::mat4(1.0);

	m_modelMatrixModel1 = glm::translate(m_modelMatrixModel1, m_positionModel1);

	m_modelMatrixModel1 = glm::rotate(m_modelMatrixModel1, (float)glm::radians(m_yAngle), glm::vec3(0, 1, 0));


	m_modelMatrixModel1 = glm::translate(m_modelMatrixModel1, glm::vec3(2.5,0,0));

	m_modelMatrixModel1 = glm::rotate(m_modelMatrixModel1, (float)glm::radians(m_xAngle), glm::vec3(1, 0, 0));

	//m_modelMatrix = rotate_z_deg(m_modelMatrix, m_zAngle);
	Win32OpenGL::SendUniformMatrixToShader(m_phongTextureShader, m_modelMatrixModel1, "model_matrix");

	glBindVertexArray(m_vaoModel1);		// select first VAO

	glDrawArrays(GL_TRIANGLES, 0, m_verticesModel1.size() / 3);
	glBindVertexArray(NULL);

	// bunny

	glm::vec3 surfaceSpecular3{ 0,0,0 };			// no specular!!!
	Win32OpenGL::SendUniformVector3ToShader(m_phongTextureShader, surfaceSpecular3, "surface_specular");
	float surfacePower3 = 100;
	Win32OpenGL::SendUniformFloatToShader(m_phongTextureShader, surfacePower3, "specular_exponent");

	glBindTexture(GL_TEXTURE_2D, m_textureDataIDModel2);
	m_modelMatrixModel2 = glm::mat4(1.0);
	m_modelMatrixModel2 = glm::translate(m_modelMatrixModel2, glm::vec3(0,-0.22,0));
	Win32OpenGL::SendUniformMatrixToShader(m_phongTextureShader, m_modelMatrixModel2, "model_matrix");

	glBindVertexArray(m_vaoModel2);		// select first VAO
	glDrawArrays(GL_TRIANGLES, 0, m_verticesModel2.size() / 3);
	glBindVertexArray(NULL);
	// unbind
	glBindTexture(GL_TEXTURE_2D, NULL);

	//
	// draw shadows
	//
	//
	// we switch shaders 

	m_win32OpenGL.UseProgram(m_flatShader);

	glm::vec4 shadowColour{ 0.0f, 0.0f, 0.0f, 0.5f };	
	Win32OpenGL::SendUniformVector4ToShader(m_flatShader, shadowColour, "surface_colour");
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	if (m_withStencilBuffer)
	{
		glStencilMask(0xFF);
		// only draw where stencil is 0
		glStencilFunc(GL_EQUAL, 0, 0xFF);
		// inc stencil buffer if was a blended write
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		glEnable(GL_STENCIL_TEST);
	}

	float deltaXModel1 = (m_positionModel1.x - m_lightPosition.x) / (m_positionModel1.y - m_lightPosition.y);
	float deltaZModel1 = (m_positionModel1.z - m_lightPosition.z) / (m_positionModel1.y - m_lightPosition.y);

	// this matrix flattens and shears the model to the ground
	// based on the slope in x and z from its centre to the sun
	// (this is an approximation - really should recalculate for each vertex!)
	glm::mat4 flatten = glm::mat4(1, 0, 0, 0, -deltaXModel1, 0, -deltaZModel1, 0, 0, 0, 1, 0, 0, 0.005, 0, 1);

	glm::mat4 m_shadowMatrix2  = flatten * m_modelMatrixModel1;
	Win32OpenGL::SendUniformMatrixToShader(m_flatShader, m_shadowMatrix2, "model_matrix");

	glBindVertexArray(m_vaoModel1);		// select first VAO
	glDrawArrays(GL_TRIANGLES, 0, m_verticesModel1.size() / 3);
	glBindVertexArray(NULL);

	float deltaXModel2 = (m_positionModel2.x - m_lightPosition.x) / (m_positionModel2.y - m_lightPosition.y);
	float deltaZModel2 = (m_positionModel2.z - m_lightPosition.z) / (m_positionModel2.y - m_lightPosition.y);

	// flatten this model
	flatten = glm::mat4(1, 0, 0, 0, -deltaXModel2, 0, -deltaZModel2, 0, 0, 0, 1, 0, 0, 0.005, 0, 1);

	glm::mat4 m_shadowMatrix3 = flatten * m_modelMatrixModel2;
	Win32OpenGL::SendUniformMatrixToShader(m_flatShader, m_shadowMatrix3, "model_matrix");

	glBindVertexArray(m_vaoModel2);		// select first VAO
	glDrawArrays(GL_TRIANGLES, 0, m_verticesModel2.size() / 3);
	glBindVertexArray(NULL);

	// finished drawing shadows
	glDisable(GL_BLEND);

	if (m_withStencilBuffer)
	{
		glDisable(GL_STENCIL_TEST);
	}
	m_win32OpenGL.FinishedDrawing();
}

void Background::Update()
{
	// we tumble the cube to see all the faces.
	m_yAngle += 1.0f;
	m_xAngle += 0.2f;
}

void Background::HandleInput(unsigned char virtualKeyCode)
{
	m_camera.HandleInput(virtualKeyCode);

	GLuint program = m_win32OpenGL.GetShaderProgram();
	m_camera.SetViewMatrix(m_phongTextureShader);
	m_camera.SetViewMatrix(m_flatShader);

	// add code for interaction here
	if (virtualKeyCode == VK_SPACE)
	{
		// no code needed here
	}
}

void Background::HandleMouseInput(int xPosRelative, int yPosRelative)
{
	//m_camera->HandleMouseInput(xPosRelative, yPosRelative);

}

void Background::Resize(HDC hdc, RECT rect)
{
	// if the display is resized then OpenGL needs to know about the apect ratio
	// to compute the correct projection matrix
	m_win32OpenGL.Reshape(hdc, rect.right, rect.bottom);
	m_aspectRatio = (float)rect.right / rect.bottom;
	ComputeProjectionMatrix();
	//GLuint program = m_win32OpenGL.GetShaderProgram();
	Win32OpenGL::SendUniformMatrixToShader(m_phongTextureShader, m_projectionMatrix, "projection_matrix");
	Win32OpenGL::SendUniformMatrixToShader(m_flatShader, m_projectionMatrix, "projection_matrix");

}

void Background::ComputeProjectionMatrix()
{
	// we will look at this later in the course
	// in Modern OpenGL we must supply a projection matrix

#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

	// input variables
	float zNear = 0.1f;		// clipping plane
	float zFar = 100.0f;	// clipping plane
	float fov = static_cast<float>(67.0f * ONE_DEG_IN_RAD); // convert 67 degrees to radians

	float range = tan(fov * 0.5f) * zNear;
	float Sx = (2.0f * zNear) / (range * m_aspectRatio + range * m_aspectRatio);
	float Sy = zNear / range;
	float Sz = -(zFar + zNear) / (zFar - zNear);
	float Pz = -(2.0f * zFar * zNear) / (zFar - zNear);
	GLfloat proj_mat[] = {
		Sx, 0.0f, 0.0f, 0.0f,
		0.0f, Sy, 0.0f, 0.0f,
		0.0f, 0.0f, Sz, -1.0f,
		0.0f, 0.0f, Pz, 0.0f
	};
	m_projectionMatrix = glm::make_mat4(proj_mat);
}

#if 0
void Example7::ComputeViewMatrix()
{
	// the view matrix allows us to implement a camera.
	// this is based on Anton's simpler version (see recommended books)  
	// - later we implement a lookAt matrix here....
	float cam_pos[] = { m_cameraX, m_cameraY, m_cameraZ }; // don't start at zero, or we will be too close
	float cam_cameraYaw = 0.0f; // y-rotation in degrees
	mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	mat4 R = rotate_y_deg(identity_mat4(), -cam_cameraYaw);
	m_viewMatrix = R * T;
}
#endif
#if 0
bool Example7::SimpleLoadTexture(string fileName, char* imageData)
{
	// we will replace this with code to read a BMP file and analyse the header in the next lab
	// for now we ASSUME a 256x256 pixel array of RGB data as unsigned bytes

	ifstream texFile;
	// important to ensure file is treated as binary data
	texFile.open(fileName, ios::binary);
	if (!texFile)
	{
		cout << "error opening texture: " << fileName << endl;
		return false;
	}

	texFile.read(imageData, 256 * 256 * 3);
	texFile.close();
	return true;
}
#endif