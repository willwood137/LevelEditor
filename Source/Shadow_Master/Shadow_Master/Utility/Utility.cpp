#include "..\stdafx.h"
#include <utility>
#include <sstream>
#include <fstream>
#include <memory>

#include "..\Math\Vector3.hpp"
#include "Utility.hpp"

namespace shadow
{
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


	const int CHARACTERS_PER_ROW = 16;
	const float CHARACTERS_TEXCOROD_SIZE = 1.0f / CHARACTERS_PER_ROW;

	void drawCharacter(unsigned char c)
	{
		float u = (float)(c % CHARACTERS_PER_ROW);
		float v = (float)(c / CHARACTERS_PER_ROW);

		glMatrixMode( GL_TEXTURE );
		glPushMatrix();
		glLoadIdentity();
		glTranslatef( u * CHARACTERS_TEXCOROD_SIZE, v * CHARACTERS_TEXCOROD_SIZE, 0 );

		glBegin( GL_TRIANGLE_STRIP );

		glTexCoord2f( 0, 0 );
		glVertex2f( 0.0f, 0.0f );

		glTexCoord2f( CHARACTERS_TEXCOROD_SIZE, 0 );
		glVertex2f( 1.0f, 0.0f );

		glTexCoord2f( 0, CHARACTERS_TEXCOROD_SIZE  );
		glVertex2f( 0.0f, 1.0f );

		glTexCoord2f( CHARACTERS_TEXCOROD_SIZE , CHARACTERS_TEXCOROD_SIZE  );
		glVertex2f( 1.0f, 1.0f );

		glEnd();
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


	void DrawTextLine(std::string& TextToDraw, std::shared_ptr<Texture> Font, Vector2f& Position_TopLeftOfLine, Vector2f& size, Vector3f& Color, bool DrawBack, Vector3f BackColor )
	{

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glLoadIdentity();


		Texture::ApplyTexture(Font);

		glTranslatef(Position_TopLeftOfLine.x, Position_TopLeftOfLine.y, 0.0f);
		glScalef(size.x, size.y, 1.0f );
		glTranslatef(.0f,-1.0f, 0.0f);


		glTranslatef(0.0f, 1.0f, 0.0f);	


		glColor4f(Color[0], Color[1], Color[2], 1.0f);

		glEnable(GL_BLEND);

		size_t chars = TextToDraw.size();
		for( size_t i = 0; i < chars; ++i )
		{
			char c = TextToDraw[ i ]; 
			drawCharacter( c );
			glTranslatef( .8f, 0, 0 );
		}

		glPopMatrix();

		GLenum err = glGetError();
		if( err != GL_NO_ERROR )
		{
			MessageBox( NULL, L"OpenGL Error: Render Problem in DrawTextLine", L"OpenGL Error", MB_OK | MB_ICONERROR );
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void DrawTextMultiLines(std::vector<std::string>& LinesToDraw, std::shared_ptr<Texture> Font, Vector2f& Position_TopLeftOfLine, Vector2f& size, Vector3f& Color, bool DrawBack, Vector3f BackColor )
	{
		size_t numLines = LinesToDraw.size();
		for( size_t i = 0; i < numLines; ++i)
		{
			DrawTextLine(	LinesToDraw[i],
				Font,
				Position_TopLeftOfLine + Vector2f(0, i*size.y),
				size,
				Color,
				DrawBack,
				BackColor);
		}
	}


	//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


	bool getFileText( const std::string& filePath, std::string& outText )
	{

		// Open the file
		//
		std::ifstream sourceFile( filePath.c_str() );


		if(sourceFile.good() )
		{

			// Read the full contents of the file into a string. (may throw.)
			//
			sourceFile.seekg( 0, std::ios::end );
			outText.reserve( static_cast< size_t > ( sourceFile.tellg() ));
			sourceFile.seekg( 0, std::ios::beg );

			outText.assign(( std::istreambuf_iterator<char>(sourceFile )),
				std::istreambuf_iterator<char>() );

			sourceFile.close();
			return true;
		}
		return false;
	}

	void getShaderLog( GLuint idShader, std::string& outLog)
	{
		GLint lenLog;
		glGetShaderiv( idShader, GL_INFO_LOG_LENGTH, &lenLog );

		if( lenLog > 0 )
		{
			// Report errors or warnings.
			//
			std::unique_ptr< char > szLogText( new char[ lenLog + 1 ] );

			glGetShaderInfoLog( idShader, lenLog, &lenLog, szLogText.get() );
			outLog += szLogText.get();


		}
	}

	void getProgramLog( GLuint idProgram, std::string& outLog)
	{
		assert( idProgram);


		GLint lenLog;
		glGetShaderiv( idProgram, GL_INFO_LOG_LENGTH, &lenLog );

		if( lenLog > 0 )
		{
			// Report errors or warnings.
			//
			std::unique_ptr< char > szLogText( new char[ lenLog + 1 ] );

			glGetProgramInfoLog( idProgram, lenLog, &lenLog, szLogText.get() );
			outLog += szLogText.get();
		}
	}

	unsigned int loadTexture( const char* szTexturePath )
	{
		int x, y, n;
		unsigned char *data = stbi_load( szTexturePath, &x, &y, &n, 4 );

		if( !data )
		{
			return 0;
		}

		// Create the OpenGL texture.
		//

		GLuint idTexture = 0;
		glGenTextures( 1, &idTexture );
		glBindTexture( GL_TEXTURE_2D, idTexture );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );		
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );

		GLenum err = glGetError();
		if( err != GL_NO_ERROR )
		{
			//DevConsole::Instance()->postMessage(Error, "OpenGL Error: Render Problem ->" + err );
		}

		stbi_image_free(data);

		return idTexture;
	}

	void DrawCube( 
		bool applyTexCoords, 
		bool applyNormals, 
		bool applyStandardFaceColors)
	{
		for( int axis = 0; axis < 3; ++axis )
		{
			for( int sign = -1; sign <= 1; sign += 2 )
			{
				Vector3f normal;
				normal[ axis ] = float( sign );

				if( applyStandardFaceColors )
				{
					Vector3f color( normal );
					if( color[ axis ] < 0 ) 
					{
						color[ axis ] = 1;
						color[ ( axis + 1 ) % 3 ] = 1;
					}
					glColor3fv( 
						reinterpret_cast< const float* >( 
						&( color.x )));
				}

				DrawCubeFace( normal, applyTexCoords, applyNormals );
			}
		}
	}

	void DrawCubeFace( 
		const Vector3f& faceNormal, 
		bool applyTexCoords, 
		bool applyNormals )
	{
		int normalAxis = 
			abs( faceNormal.x ) > abs( faceNormal.y )
			?
			( abs( faceNormal.x ) > abs( faceNormal.z ) ? 0 : 2 )
			:
			( abs( faceNormal.y ) > abs( faceNormal.z ) ? 1 : 2 );

		int otherAxisA = ( normalAxis + 1 ) % 3;
		int otherAxisB = ( normalAxis + 2 ) % 3;

		Vector3f vertex( 0, 0, 0 );
		vertex[ normalAxis ] = faceNormal[ normalAxis ] > 0 ? 1.0f : 0.0f;

		bool reverseWinding = faceNormal[ normalAxis ] <= 0;

		if( normalAxis == 0 )
		{
			std::swap( otherAxisA, otherAxisB );
			reverseWinding = !reverseWinding;
		}

		glBegin( GL_TRIANGLE_STRIP );
		{
			for( int i = 0; i < 4; ++i )
			{
				vertex[ otherAxisA ] = ( i & 1 ) ? 1.0f : 0.0f;

				if( reverseWinding )
				{
					vertex[ otherAxisA ] = 1.0f - vertex[ otherAxisA ];
				}

				vertex[ otherAxisB ] = ( i >> 1 ) ? 1.0f : 0.0f;

				if( applyNormals )
				{
					glNormal3fv( 
						reinterpret_cast< const float* >( 
						&( faceNormal.x )));
				}
				if( applyTexCoords )
				{
					glTexCoord2f( 
						( i & 1 ) ? 1.0f : 0.0f, 
						( i >> 1 ) ? 1.0f : 0.0f );
				}
				glVertex3fv( 
					reinterpret_cast< const float* >( 
					&( vertex.x )));
			}
		}
		glEnd();
	}




	unsigned int loadVertexBufferObject( const void* pVertices, size_t nBytes )
	{

		GLuint idVBO;
		glGenBuffers( 1, &idVBO );
		glBindBuffer( GL_ARRAY_BUFFER, idVBO );

		glBufferData( GL_ARRAY_BUFFER, nBytes, pVertices, GL_STATIC_DRAW );

		return idVBO;
	}

	unsigned int loadIndexBufferObject( const void* pVertices, size_t nBytes )
	{

		GLuint idVBO;
		glGenBuffers( 1, &idVBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, idVBO );

		glBufferData( GL_ELEMENT_ARRAY_BUFFER, nBytes, pVertices, GL_STATIC_DRAW );

		return idVBO;
	}

	float Easein( float inValue, float inStart, float inEnd, float outStart, float outEnd )
	{
		float outValue = inValue -inStart;
		if( inEnd == inStart )
			return 0.5f * ( outEnd + outStart );
		outValue /= ( inEnd - inStart);

		//outValue = 1 - (1-outValue) * (1-outValue);
		//outValue = 2* outValue * outValue - outValue*outValue*outValue;

		outValue *= ( outEnd - outStart );
		outValue += outStart;

		// if outValue is outside of out range, do not set it to the boundary values, 
		// this allows filter functions that step out of the boundaries


		return outValue;
	} 

	void TranslateToPosition( Vector3f& start, Vector3f& end, float time)
	{

		start = (end - start) * time + start;

		// bring to range 1,1
		//start.x = Easein(pos.x, start.x, end.x, start.x, end.x);
		//start.y = Easein(pos.y, start.y, end.y, start.y, end.y);
		//start.z = Easein(pos.z, start.z, end.z, start.z, end.z);

	}

	bool pointInAABB3( Vector3f& point, int* min, int* max)
	{

		if( point.x < min[0] || point.x > max[0] )
			return false;
		if( point.y < min[2] || point.y > max[2] )
			return false;
		if( point.z > -min[1] || point.z < -max[1] )
			return false;

		return true;
	}

	unsigned int LoadBufferObject( GLenum bufferType, const void* vertices, size_t bytes )
	{
		assert( bufferType == GL_ARRAY_BUFFER || bufferType == GL_ELEMENT_ARRAY_BUFFER );
		//VBO or IBO

		GLuint idVBO;

		//Bind if of our buffer
		glGenBuffers( 1, &idVBO );
		glBindBuffer( bufferType, idVBO );

		//Feed the data to it. Not going to modify it, but want to draw it.
		glBufferData( bufferType, bytes, vertices, GL_STATIC_DRAW );


		return idVBO;
	}
}