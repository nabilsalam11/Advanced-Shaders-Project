#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Log.h"

using namespace std;

class ModelLoader
{
private:


	vector<float> m_indexedVertices;
	vector<float> m_indexedNormals;
	vector<unsigned int> m_faceVertexIndices;
	vector<unsigned int> m_faceNormalIndices;
	vector<float> m_vertices;
	vector<float> m_normals;


	vector<unsigned int> m_faceTextureIndices;
	vector<float> m_indexedTextureUV;
	vector<float> m_textureUV;



	string m_modelName;
	string m_filename;

public:
	ModelLoader();
	~ModelLoader();


	void LoadModel(string modelName);
	void ProcessVertexLine(istringstream& iss);
	void ProcessVertexNormalLine(istringstream& iss);
	void ProcessVertexTextureLine(istringstream& iss);
	void ProcessFaceLine(istringstream& iss);
	void CreateVerticesFromIndexedVertices();
	void CreateNormalsFromIndexedNormals();
	void CreateTextureUVFromIndexedTextureUV();

	vector<float>& GetVertices();
	vector<float>& GetNormals();
	vector<float>& GetTextureUV();
};
