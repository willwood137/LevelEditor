#pragma once

#include <string>
#include <set>
#include <vector>

#include "Texture.hpp"
#include "..\Math\Vector4.hpp"
#include "..\Math\Vector3.hpp"
#include "..\Math\Vector2.hpp"
#include "..\Utility\HashedString.hpp"
#include "Shader.hpp"

namespace shadow
{
	class Font
	{


	public:

		struct FontCompare
		{
			bool operator()( const std::shared_ptr<Font>& lhs, const std::shared_ptr<Font>& rhs ) const;
		};
		struct GlyphVert
		{
			Vector2f pos;
			Vector2f texCoords;
			GlyphVert( ) {}
			GlyphVert( Vector2f p, Vector2f tc) : pos(p), texCoords(tc) {}
		};

		struct FontCharacterInfo
		{
			Vector2f textureMins;
			Vector2f textureMaxs;
			float ttfA;
			float ttfB;
			float ttfC;
			unsigned int ucsIndex;

			bool operator<( const FontCharacterInfo& rhs )
			{
				return ucsIndex < rhs.ucsIndex;
			}
		};

		Font( const HashedString& fontName );
		~Font();

		bool IsLoaded() const;
		std::string& GetDirectory();
		bool LoadFromFile( const HashedString& fontName );


		//
		// Static Member Functions ////////////////////////////////////////////
		//
		static float getLengthOfString( const std::string& textToDraw,
			std::shared_ptr<Font>& pFont,
			float size,
			float aspect = 1.0f);

		static bool DrawTextToScreen( const std::string& textToDraw, 
			std::shared_ptr<Font>& pFont, 
			const RGBA& color, 
			const Vector2f& topLeft, 
			float size, 
			float aspect = 1.0f);

		static bool DrawTextToWorldAlignedWorld( const std::string& textToDraw, 
			std::shared_ptr<Font>& pFont, 
			const RGBA& color, 
			const Vector3f& topLeft,
			const Vector3f& direction,
			float roll,
			float size, 
			float aspect = 1.0f);

		static bool DrawTextToWorldAlignedScreen( const std::string& textToDraw, 
			std::shared_ptr<Font>& pFont, 
			const RGBA& color, 
			const Vector3f& center,
			float size, 
			float aspect = 1.0f);

		static bool DrawTextToWorldAlignedAxis( const std::string& textToDraw, 
			std::shared_ptr<Font>& pFont, 
			const RGBA& color, 
			const Vector3f& center,
			const Vector3f& axis,
			float size, 
			float aspect = 1.0f);

		static std::shared_ptr<Font> CreateOrGetFont( const HashedString& FontName );
		static void DestroyFont( std::shared_ptr<Font>& fontToDestroy );
		static void DestroyAllFonts();
		static void SetFontDirectory( const std::string& FontDirectory);
		static void LoadShader();

	private:

		Font();

		static std::string s_FontDirectory;
		static std::set< std::shared_ptr<Font>, FontCompare > Fonts;
		static Shader s_Shader;

		std::shared_ptr<Texture> m_fontTexture;
		std::vector<FontCharacterInfo> m_characterInfos;
		int m_cellPixelHeight;
		HashedString m_name;
		unsigned int m_idVBO;
		bool m_isLoaded;
	};

	inline bool Font::FontCompare::operator()( const std::shared_ptr<Font>& lhs, const std::shared_ptr<Font>& rhs ) const
	{
		return lhs->m_name < rhs->m_name;
	}
}