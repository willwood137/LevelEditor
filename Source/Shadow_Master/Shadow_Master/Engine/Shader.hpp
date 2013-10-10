#pragma once

#include <map>

#include "..\Utility\HashedString.hpp"

namespace shadow
{

	class Shader
	{
	public:

		Shader(const std::string& filepath);
		Shader() {}
		
		bool addAttribute(const HashedString& attributeName);
		bool addUniform(const HashedString& uniformName);

		bool load();

		unsigned int getAttributeID(const HashedString& attributeName) const;
		unsigned int getUniformID(const HashedString& uniformName) const;

		bool setAttribute( const HashedString& attributeName, 
			const unsigned int numValues, 
			const unsigned int glType, 
			const size_t VertexSize, 
			const size_t Offset ) const;

		bool disableAttribute( const HashedString& attributeName ) const;

		template <typename T_ObjType>
		bool setUniform( const HashedString& uniformName, const T_ObjType& data) const;

	private:
		std::map<HashedString, unsigned int> m_attributes;
		std::map<HashedString, unsigned int> m_uniforms;

		std::string m_filePath;

		unsigned int m_shaderID;
	};


// 	inline bool Shader::setUniform( const HashedString& uniformName, const float& data) const 
// 	{
// 		glUniform1f( m_uniforms[uniformName], data);
// 	}


}