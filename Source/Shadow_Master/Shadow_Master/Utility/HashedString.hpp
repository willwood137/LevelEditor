#pragma once
#include <string>

namespace shadow
{
	struct HashedString
	{
		std::string m_string;
		unsigned int m_hash;

		HashedString(const std::string&	StringToHash ) : m_string(StringToHash), m_hash(0)
		{
			for each (char c in m_string)
			{
				m_hash *= 31;
				m_hash += tolower(c);
			}
		}

		bool operator<( const HashedString& rhs) const
		{
			if(m_hash != rhs.m_hash)
				return m_hash < rhs.m_hash;
			else
				return _stricmp(m_string.c_str(), rhs.m_string.c_str() ) < 0;
		}
	};

	//Hashed String Definitions
	//

	//uniforms
	const HashedString HASHED_STRING_u_MV("u_MV");
	const HashedString HASHED_STRING_u_ProjectionMatrix("u_ProjectionMatrix");
	const HashedString HASHED_STRING_u_color("u_color");
	const HashedString HASHED_STRING_u_size("u_size");
	const HashedString HASHED_STRING_u_offset("u_offset");
	const HashedString HASHED_STRING_u_debugInt("u_debugInt");
	const HashedString HASHED_STRING_u_diffuse("u_diffuse");
	const HashedString HASHED_STRING_u_intersection("u_intersection");
	const HashedString HASHED_STRING_u_TopLeft("u_TopLeft");
	const HashedString HASHED_STRING_u_BottomRight("u_BottomRight");
	const HashedString HASHED_STRING_u_middleLine("u_middleLine");
	const HashedString HASHED_STRING_u_position("u_position");
	const HashedString HASHED_STRING_u_WorldTransform("u_WorldTransform");
	const HashedString HASHED_STRING_u_minGrid("u_minGrid");
	const HashedString HASHED_STRING_u_maxGrid("u_maxGrid");


	//attributes
	const HashedString HASHED_STRING_a_position("a_position");
	const HashedString HASHED_STRING_a_texCoords("a_texCoords");
	const HashedString HASHED_STRING_a_color("a_color");

	//textures

	//Fonts
	const HashedString HASHED_STRING_MainFont_EN("MainFont_EN");

	//Events 

	//Console commands
	const HashedString HASHED_STRING_help("help");


}