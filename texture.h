#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <GLEW/glew.h>
#include <iostream>
#include <fstream>

class TextureHelper
{
public:
	/*
	/* Static function and constructor to load 2D Textures on model                                                                
	*/
	static  GLuint load2DTexture(const char* filename, GLint internalFormat = GL_RGB,
		GLenum picFormat = GL_RGB, int loadChannels = SOIL_LOAD_RGB, GLboolean alpha=false)
	{
		// Step 1: Assigining Texture ID
		GLuint textureId = 0;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		// Step 2: Wrapping Texture Parameters
		// gfdhsbsrsdgfvzxghzdfhzdfhb
		// ±ßÔµ²¿·Ö°ëÍ¸Ã÷ ÊÇÒòÎªÊ¹ÓÃÏÂ´ÎÖØ¸´µÄÎÆÀí×ö²åÖµµ¼ÖÂµÄ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		// Step 3: Applying filers to texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
			GL_LINEAR_MIPMAP_LINEAR); // Applying Linear MipMap
		// Step 4: Give imageData properties
		GLubyte *imageData = NULL;
		int picWidth, picHeight;
		int channels = 0;
		imageData = SOIL_load_image(filename, &picWidth, &picHeight, &channels, loadChannels);
		if (imageData == NULL)
		{
			std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
			return 0;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, picWidth, picHeight, 
			0, picFormat, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
		// Step 5: Bind the Texture to the models
		SOIL_free_image_data(imageData);
		glBindTexture(GL_TEXTURE_2D, 0);
		return textureId;
	}
	/*
	*  framebuffer-attachable texture
	*/
	static GLuint makeAttachmentTexture(GLint level = 0, GLint internalFormat = GL_DEPTH24_STENCIL8,
		GLsizei width = 800, GLsizei height = 600,GLenum picFormat = GL_DEPTH_STENCIL, 
		GLenum picDataType = GL_UNSIGNED_INT_24_8)
	{
		GLuint textId;
		glGenTextures(1, &textId);
		glBindTexture(GL_TEXTURE_2D, textId);
		glTexImage2D(GL_TEXTURE_2D, level, internalFormat, 
			width, height, 0, picFormat, picDataType, NULL); // 2D Texture Image Properties
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textId;
	}
	/*
	* ´ multi-sampled framebuffer-attachable texture
	*/
	static GLuint makeMAAttachmentTexture(GLint samplesNum = 4, GLint internalFormat = GL_RGB,
		GLsizei width = 800, GLsizei height = 600)
	{
		GLuint textId;
		glGenTextures(1, &textId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textId); // GL binding 2D texture files
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samplesNum, internalFormat,
			width, height, GL_TRUE); // Binding Texture Images while giving height and width
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		return textId;
	}
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

	static GLuint loadDDS(const char * filename){

		
		/* try to open the file */
		std::ifstream file(filename, std::ios::in | std::ios::binary);
		if (!file){
			std::cout << "Error::loadDDs, could not open:" 
				<< filename << "for read." << std::endl;
			return 0;
		}

		/* verify the type of file */
		char filecode[4];
		file.read(filecode, 4);
		if (strncmp(filecode, "DDS ", 4) != 0) {
			std::cout << "Error::loadDDs, format is not dds :"
				<< filename  << std::endl;
			file.close();
			return 0;
		}

		/* get the surface desc */
		char header[124];
		file.read(header, 124);

		unsigned int height = *(unsigned int*)&(header[8]);
		unsigned int width = *(unsigned int*)&(header[12]);
		unsigned int linearSize = *(unsigned int*)&(header[16]);
		unsigned int mipMapCount = *(unsigned int*)&(header[24]);
		unsigned int fourCC = *(unsigned int*)&(header[80]);


		char * buffer = NULL;
		unsigned int bufsize;
		/* how big is it going to be including all mipmaps? */
		bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
		buffer = new char[bufsize];
		file.read(buffer, bufsize);
		/* close the file pointer */
		file.close();

		unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
		unsigned int format;
		switch (fourCC)
		{
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			delete[] buffer;
			return 0;
		}

		// Create one OpenGL texture
		GLuint textureID;
		glGenTextures(1, &textureID);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		unsigned int offset = 0;

		/* load the mipmaps */
		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
		{
			unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
				0, size, buffer + offset);

			offset += size;
			width /= 2;
			height /= 2;

			// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
			if (width < 1) width = 1;
			if (height < 1) height = 1;

		}

		delete[] buffer;

		return textureID;
	}
};

#endif