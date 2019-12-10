#include "stdafx.h"
#include "ModelLoader.h"


ModelLoader::ModelLoader()
{
}


ModelLoader::~ModelLoader()
{
}

void ModelLoader::LoadModel(string modelName)
{
	std::fstream modelfile(modelName, std::ios_base::in);

	if (modelfile.is_open())
	{
		string line;
		string token = "";
		while (getline(modelfile, line))
		{
			istringstream iss(line);

			// process the line
			token = "";

			iss >> token; // read to first whitespace

			if (token == "#")
			{
				// ignore rest of line
				// iss >> s;
			}
			else if (token == "g")
			{
				// read the model name
				iss >> m_modelName;
				// ignore rest of line
			}
			else if (token == "v")
			{
				ProcessVertexLine(iss);
			}
			else if (token == "vn")
			{
				ProcessVertexNormalLine(iss);
			}
			else if (token == "vt")
			{
				ProcessVertexTextureLine(iss);
			}
			else if (token == "f")
			{
				ProcessFaceLine(iss);
			}
			else
			{
				// ignore any line without these qualifiers
				// ignore rest of line
			}
		}
		modelfile.close();

		CreateVerticesFromIndexedVertices();
		CreateNormalsFromIndexedNormals();
		CreateTextureUVFromIndexedTextureUV();

	}
}

void ModelLoader::ProcessVertexLine(istringstream& iss)
{
	float value;
	while (iss >> value)
	{
		m_indexedVertices.push_back(value);
	}
}

void ModelLoader::ProcessVertexNormalLine(istringstream& iss)
{
	float value;
	while (iss >> value)
	{
		m_indexedNormals.push_back(value);
	}
}

void ModelLoader::ProcessVertexTextureLine(istringstream& iss)
{
	float value;
	while (iss >> value)
	{
		m_indexedTextureUV.push_back(value);	// would expect pairs!!!
	}
}


void ModelLoader::ProcessFaceLine(istringstream& iss)
{
	// need to know ascii for forward slash
	static const int forwardSlash = 0x2F;

	int numberOfFaceVertices = 0;	//we expect 3

	int value;
	while (iss >> value)
	{

		// some models have a -sign here!!!
		value = abs(value);

		numberOfFaceVertices++;

		if (numberOfFaceVertices <= 3)
		{
			// get the vertex term
			m_faceVertexIndices.push_back(value - 1);

			// f has 2 optional parts  vt and vn
			// look for f//vn and f/vt/vn as values

			int lookAhead = iss.peek();  // peek character
			if (lookAhead == forwardSlash)   // "/"
			{
				int chomp = iss.get();	// consume the character

				lookAhead = iss.peek(); // and look at the next one
				{
					if (lookAhead == forwardSlash)
					{
						// was "//" -   v // vn
						chomp = iss.get();

						// here we expect the vn part

						iss >> value;
						value = abs(value);
						m_faceNormalIndices.push_back(value - 1);


					}
					else
					{
						// was "/"   v / vt / vn
						// get vt part

						iss >> value;
						value = abs(value);
						m_faceTextureIndices.push_back(value - 1);

						// we expect another slash here
						// but some models down't have it so we check
						lookAhead = iss.peek();


						if (lookAhead == forwardSlash)
						{
							chomp = iss.get();
							// here we expect the vn part

							iss >> value;
							value = abs(value);
							m_faceNormalIndices.push_back(value - 1);
						}
					}
				}

			}
		}
		else
		{
			// we could create a second triangle here
			// with last 2 vertices plus this one?
		}

		// can't cope with > 4vertices
		if (numberOfFaceVertices > 3)
		{
			ostringstream oss;

			oss << "Model: " << m_filename << " has too many vertices in face." << endl;

			Log::AppendToLogFile(oss.str());
			// too many vertices in face
			std::cerr << "Too many vertices in face ";
			DebugBreak();
			throw std::invalid_argument("Too many vertices");
		}
		else
		{
			// all ok
		}
	}
}


void ModelLoader::CreateVerticesFromIndexedVertices()
{
	for (std::vector<unsigned int>::iterator it = m_faceVertexIndices.begin(); it != m_faceVertexIndices.end(); ++it)
	{
		//here we have a 1 base index
		// get the face number as 0 indexed
		unsigned int vertexNumber = (*it);
		// 3 floats per triangular face
		unsigned int offset = vertexNumber * 3;

		// pick up the 3 floats for the vertex
		for (int i = 0; i < 3; i++)
		{
			float value = m_indexedVertices[offset + i];
			m_vertices.push_back(value);
		}
	}
}


void ModelLoader::CreateNormalsFromIndexedNormals()
{
	// we create a list of normal triplets for each face (with duplicates)
	// this will then be the same size as the vertexTriplets vector

	m_normals.clear();

	for (std::vector<unsigned int>::iterator it = m_faceNormalIndices.begin(); it != m_faceNormalIndices.end(); ++it)
	{
		//here we have a 1 base index
		// get the face number as 0 indexed
		int vertexNormalNumber = (*it);
		// 3 floats per triangular face
		int offset = vertexNormalNumber * 3;

		// pick up the 3 floats for the vertex
		for (int i = 0; i < 3; i++)
		{
			float value = m_indexedNormals[offset + i];
			m_normals.push_back(value);
		}
	}
}
void ModelLoader::CreateTextureUVFromIndexedTextureUV()
{
	// create actual vertices here (with duplicates)
	// this is in the form that glDrawArrays can work with
	//
	// assume triangles so far
	// assert((faceVertexIndices.size() % 3) == 0);



	m_textureUV.clear();

	for (std::vector<unsigned int>::iterator it = m_faceTextureIndices.begin(); it != m_faceTextureIndices.end(); ++it)
	{
		//here we have a 1 base index
		// get the face number as 0 indexed
		unsigned int vertexTextureNumber = (*it);

		if (vertexTextureNumber >= m_faceTextureIndices.size())
		{
			std::cerr << "Bad texture vertex";
		}
		// 2 floats per texture coord
		int offset = vertexTextureNumber * 2;

		// pick up the 2 floats for the vertex
		for (int i = 0; i < 2; i++)
		{
			float value = m_indexedTextureUV[offset + i];
			m_textureUV.push_back(value);
		}
	}
}

vector<float>& ModelLoader::GetVertices()
{
	return m_vertices;
}
vector<float>& ModelLoader::GetNormals()
{
	return m_normals;
}

vector<float>& ModelLoader::GetTextureUV()
{
	return m_textureUV;
}