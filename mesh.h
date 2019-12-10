#ifndef _MESH_H_
#define _MESH_H_

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>       
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"

// Vertex Structure properties
struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
	glm::vec3 tangent;
};

// Texture Structure properties
struct Texture
{
	GLuint id;
	aiTextureType type;
	std::string path;
};

// Mesh Properties 
class Mesh
{
public:
	void draw(const Shader& shader) const// Drawing the shader for mesh properties
	{
		if (VAOId == 0 
			||VBOId == 0 
			|| EBOId == 0)
		{
			return;
		}
		glBindVertexArray(this->VAOId);
		int texUnitCnt = this->bindTextures(shader);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		// Binding the textures for objects
		this->unBindTextures(texUnitCnt);
	}
	int bindTextures(const Shader& shader) const
	{
		int diffuseCnt = 0, specularCnt = 0, texUnitCnt = 0,normalCnt = 0;
		for (std::vector<Texture>::const_iterator it = this->textures.begin();
			this->textures.end() != it; ++it)
		{
			switch (it->type)
			{
				case aiTextureType_DIFFUSE:
				{
						glActiveTexture(GL_TEXTURE0 + texUnitCnt);
						glBindTexture(GL_TEXTURE_2D, it->id);
						std::stringstream samplerNameStr;
						samplerNameStr << "texture_diffuse" << diffuseCnt++;
						glUniform1i(glGetUniformLocation(shader.programId,
							samplerNameStr.str().c_str()), texUnitCnt++);
				}
				break;
				case aiTextureType_SPECULAR:
				{
					glActiveTexture(GL_TEXTURE0 + texUnitCnt);
					glBindTexture(GL_TEXTURE_2D, it->id);
					std::stringstream samplerNameStr;
					samplerNameStr << "texture_specular" << specularCnt++;
					glUniform1i(glGetUniformLocation(shader.programId,
						samplerNameStr.str().c_str()), texUnitCnt++);
				}
				break;
				case aiTextureType_HEIGHT:
				{
					glActiveTexture(GL_TEXTURE0 + texUnitCnt);
					glBindTexture(GL_TEXTURE_2D, it->id);
					std::stringstream samplerNameStr;
					samplerNameStr << "texture_normal" << normalCnt++;
					glUniform1i(glGetUniformLocation(shader.programId,
							samplerNameStr.str().c_str()), texUnitCnt++);
				}
				break;
			default:
				std::cerr << "Warning::Mesh::draw, texture type" << it->type
					<< " current not supported." << std::endl;
				break;
			}
		}
		return texUnitCnt;
	}
	void unBindTextures(const int texUnitCnt) const
	{
		for (int i = 0; i < texUnitCnt; ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	Mesh():VAOId(0), VBOId(0), EBOId(0){}
	Mesh(const std::vector<Vertex>& vertData, 
		const std::vector<Texture> & textures,
		const std::vector<GLuint>& indices):VAOId(0), VBOId(0), EBOId(0) // Giving the vertex data, textures, and indices to the Mesh
	{
		setData(vertData, textures, indices);
	}
	void setData(const std::vector<Vertex>& vertData,
		const std::vector<Texture> & textures,
		const std::vector<GLuint>& indices)
	{
		this->vertData = vertData;
		this->indices = indices;
		this->textures = textures;
		if (!vertData.empty() && !indices.empty())
		{
			this->setupMesh();
		}
	}
	void final() const
	{
		glDeleteVertexArrays(1, &this->VAOId);
		glDeleteBuffers(1, &this->VBOId);
		glDeleteBuffers(1, &this->EBOId);
	}
	~Mesh()
	{
		// Desconstructor for Mesh Object
	}
	GLuint getVAOId() const { return this->VAOId; }
	const std::vector<Vertex>& getVertices() const { return this->vertData; }
	const std::vector<GLuint>& getIndices() const { return this->indices; }
private:
	std::vector<Vertex> vertData;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	GLuint VAOId, VBOId, EBOId;
	void setupMesh()  // Constructor to setupMesh function
	{
		glGenVertexArrays(1, &this->VAOId);
		glGenBuffers(1, &this->VBOId);
		glGenBuffers(1, &this->EBOId);

		glBindVertexArray(this->VAOId);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertData.size(),
			&this->vertData[0], GL_STATIC_DRAW);
		// Giving first GL Vertex Attribute Pointer properties
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Giving second GL Vertex Attribute Pointer properties
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		// Giving third GL Vertex Attribute Pointer properties
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(5 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(2);
		// Giving fourth GL Vertex Attribute Pointer properties
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(8 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(3);
		// Bind the Buffer and Buffer Data into the Mesh object
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBOId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* this->indices.size(),
			&this->indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

#endif 