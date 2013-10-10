#include "..\stdafx.h"

#include "Shader.hpp"
#include "..\Utility\Utility.hpp"

namespace shadow
{
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	Shader::Shader( const std::string& filePath ) : m_filePath(filePath)
	{
		bool good;

		std::string vertexShaderSourceText;
		good = getFileText( m_filePath + ".vert", vertexShaderSourceText );

		if(!good)
		{
			//error!
			return;	
		}

		std::string fragmentShaderSourceText;
		good = getFileText( m_filePath + ".frag", fragmentShaderSourceText );

		if(!good)
		{
			//error!
			return;	
		}

		m_shaderID = glCreateProgram();

		GLuint idVertexShader = glCreateShader( GL_VERTEX_SHADER );

		const char* szVertexShaderSourceText = vertexShaderSourceText.c_str();
		glShaderSource( idVertexShader, 1, &szVertexShaderSourceText, NULL );
		glCompileShader( idVertexShader );

		std::string log;
		OutputDebugString( TEXT("Terrain Vertex Shader Log: \n") );
		getShaderLog( idVertexShader, log );
		OutputDebugStringA( log.c_str() );
		log.clear();

		const char* szFragmentShaderSourceText = fragmentShaderSourceText.c_str();
		GLuint idFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
		glShaderSource( idFragmentShader, 1, &szFragmentShaderSourceText, NULL );
		glCompileShader( idFragmentShader );

		OutputDebugString( TEXT("Terrain Fragment Shader Log: \n") );
		getShaderLog( idFragmentShader, log );
		OutputDebugStringA( log.c_str() );
		log.clear();

		glAttachShader( m_shaderID, idVertexShader );
		glAttachShader( m_shaderID, idFragmentShader );

		glLinkProgram( m_shaderID );

		OutputDebugString( TEXT("Terrain Shader Program Link Log: \n") );
		getProgramLog(m_shaderID, log );
		OutputDebugStringA( log.c_str() );
		log.clear();

		glValidateProgram( m_shaderID );
		OutputDebugString( TEXT("Terrain Shader Program Validation Log: \n") );
		getProgramLog( m_shaderID, log);
		OutputDebugStringA( log.c_str() );
		log.clear();

	}
	
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool Shader::addAttribute(const HashedString& attributeName)
	{
		unsigned int temp = glGetAttribLocation(m_shaderID, attributeName.m_string.c_str());
		if( temp == -1)
		{
			//error report
			// 
			// TODO
			return false;
		}

		m_attributes[attributeName] = temp;
		return true;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool Shader::addUniform( const HashedString& uniformName)
	{
		unsigned int temp = glGetUniformLocation(m_shaderID, uniformName.m_string.c_str());
		if( temp == -1)
		{
			//error report
			//
			// TODO
			return false;
		}
		m_uniforms[uniformName] = temp;
		return true;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	unsigned int Shader::getAttributeID(const HashedString& attributeName) const
	{
		//return m_attributes[attributeName];
		return m_attributes.find(attributeName)->second;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	unsigned int Shader::getUniformID(const HashedString& uniformName) const
	{
		//return m_uniforms[uniformName];
		return m_uniforms.find(uniformName)->second;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool Shader::setAttribute( const HashedString& attributeName, 
		const unsigned int numValues, 
		const unsigned int glType, 
		const size_t VertexSize, 
		const size_t Offset ) const
	{
		auto it = m_attributes.find(attributeName);
		if( it != m_attributes.end())
		{
			unsigned int attribID = it->second;
			glVertexAttribPointer(attribID, numValues, glType, GL_FALSE,
				VertexSize,	reinterpret_cast< void* >(Offset) );
			glEnableVertexAttribArray(attribID);
			return true;
		}
		else
		{
			//report error
			return false;
		}
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool Shader::disableAttribute( const HashedString& attributeName ) const
	{
		//glDisableVertexAttribArray(m_attributes[attributeName]);
		glDisableVertexAttribArray(m_attributes.find(attributeName)->second);
		return true;
	}

	bool Shader::load()
	{
		glUseProgram(m_shaderID);
		return true;
	}
}