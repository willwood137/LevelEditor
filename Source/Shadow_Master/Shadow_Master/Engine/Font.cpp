#pragma once

#include "stdafx.h"

#include <string>
#include <set>
#include <sstream>
#include <vector>


#include "..\libs\pugixml.hpp"
#include "Font.hpp"
#include "..\Math\Matrix4.hpp"
#include "OpenGLApp.hpp"

namespace shadow
{

	const std::string TEXT_SHADER_SOURCE_FILE_PATH = "Shaders\\Text";

	Shader Font::s_Shader;
	std::set<std::shared_ptr<Font>, Font::FontCompare> Font::Fonts;
	std::string Font::s_FontDirectory(ASSETS + "Fonts\\");

	Font::Font( const HashedString& fontName ) : m_name( fontName ) 
	{
		LoadFromFile( fontName );
	}
	
	Font::~Font()
	{

	}

	bool Font::IsLoaded() const
	{
		// TODO
		//
		return true;
	}

	std::string& Font::GetDirectory()
	{
		return s_FontDirectory;
	}

	bool Font::LoadFromFile( const HashedString& fontName )
	{
		pugi::xml_document* doc = new pugi::xml_document();
		pugi::xml_parse_result result = doc->load_file( (s_FontDirectory + fontName.m_string + ".xml").c_str() );

		pugi::xml_node topNode = doc->first_child();

		m_cellPixelHeight = topNode.child("FontInfo").attribute("cellPixelHeight").as_int();
		

		m_characterInfos.resize(255-31);
		std::vector<GlyphVert> verts;
		verts.resize((255-31) * 4);
		if(std::string(topNode.name()).compare(std::string("FontDefinition") ) == 0 )
		{
			pugi::xml_node glyphNode = topNode.first_child();
			while(glyphNode)
			{
				std::string s(glyphNode.name());
				if( std::string(glyphNode.name()).compare(std::string("Glyph") ) == 0 )
				{
					FontCharacterInfo glyph;
					pugi::xml_attribute topAttributes = glyphNode.first_attribute();
					while(topAttributes)
					{
						if(std::string(topAttributes.name()).compare("ucsIndex") == 0)
						{
							glyph.ucsIndex = topAttributes.as_int();

							if(m_characterInfos.size() < (size_t)(glyph.ucsIndex - 31))
								m_characterInfos.resize(glyph.ucsIndex - 31);
						}
						else if(std::string(topAttributes.name()).compare("texCoordMins") == 0)
						{
							std::stringstream ss;
							std::string s = topAttributes.value();
							int firstComma = s.find(',');
							ss << s.substr(1, firstComma - 1);
							float x;
							ss >> x;
							ss.clear();
							ss << s.substr(firstComma + 1, s.length() - firstComma - 2 );
							float y;
							ss >> y;
							glyph.textureMins = Vector2f(x, y);
						}
						else if(std::string(topAttributes.name()).compare("texCoordMaxs") == 0)
						{
							std::stringstream ss;
							std::string s = topAttributes.value();
							int firstComma = s.find(',');
							ss << s.substr(1, firstComma - 1);
							float x;
							ss >> x;

							ss.clear();
							ss << s.substr(firstComma + 1, s.length() - firstComma - 2 );
							float y;
							ss >> y;
							glyph.textureMaxs = Vector2f(x, y);
						}
						else if(std::string(topAttributes.name()).compare("ttfA") == 0)
						{
							glyph.ttfA = topAttributes.as_float();
						}
						else if(std::string(topAttributes.name()).compare("ttfB") == 0)
						{
							glyph.ttfB = topAttributes.as_float();
						}
						else if(std::string(topAttributes.name()).compare("ttfC") == 0)
						{
							glyph.ttfC = topAttributes.as_float();
						}

						topAttributes = topAttributes.next_attribute();
					}
					
					
					verts[ (glyph.ucsIndex - 32) * 4 ]     = (GlyphVert(Vector2f(0.0f, 0.0f), Vector2f( glyph.textureMins.x, glyph.textureMaxs.y) ) );
					verts[ (glyph.ucsIndex - 32) * 4 + 1] = (GlyphVert(Vector2f(0.0f, 1.0f), Vector2f( glyph.textureMins.x, glyph.textureMins.y) ) );
					verts[ (glyph.ucsIndex - 32) * 4 + 2] = (GlyphVert(Vector2f(glyph.ttfB, 0.0f), Vector2f( glyph.textureMaxs.x, glyph.textureMaxs.y) ) );
					verts[ (glyph.ucsIndex - 32) * 4 + 3] = (GlyphVert(Vector2f(glyph.ttfB, 1.0f), Vector2f( glyph.textureMaxs.x, glyph.textureMins.y) ) );
					
					if(glyph.ucsIndex > 48 && glyph.ucsIndex <= 57)
					{
						glyph.ttfA = m_characterInfos[48-32].ttfA;
						glyph.ttfB = m_characterInfos[48-32].ttfB;
						glyph.ttfC = m_characterInfos[48-32].ttfC;
					}

					m_characterInfos[glyph.ucsIndex - 32] = glyph;


				}
				glyphNode = glyphNode.next_sibling();
			}
		}

	m_idVBO = LoadBufferObject(GL_ARRAY_BUFFER, verts.data(), sizeof(GlyphVert) * verts.size());
	m_fontTexture = Texture::CreateOrGetTexture(s_FontDirectory + fontName.m_string + ".png");
				
	return true;
	}


	//
	// Static Member Functions ////////////////////////////////////////////
	//
	void Font::LoadShader()
	{
		s_Shader = Shader(TEXT_SHADER_SOURCE_FILE_PATH);
		s_Shader.addUniform(HASHED_STRING_u_MV );
		s_Shader.addUniform(HASHED_STRING_u_ProjectionMatrix );
		s_Shader.addUniform(HASHED_STRING_u_offset );
		s_Shader.addUniform(HASHED_STRING_u_color );
		s_Shader.addUniform(HASHED_STRING_u_size );
		s_Shader.addUniform(HASHED_STRING_u_debugInt );
		s_Shader.addUniform(HASHED_STRING_u_diffuse );
		s_Shader.addUniform(HASHED_STRING_u_WorldTransform );
		s_Shader.addUniform(HASHED_STRING_u_position );

		s_Shader.addAttribute( HASHED_STRING_a_position );
		s_Shader.addAttribute( HASHED_STRING_a_texCoords );
	}

	float Font::getLengthOfString( const std::string& textToDraw,
		std::shared_ptr<Font>& pFont,
		float size,
		float aspect)
	{
		if(textToDraw.empty())
			return .0f;

		size_t slength = textToDraw.size();
		float x = 0;
		FontCharacterInfo* c = &pFont->m_characterInfos[textToDraw[0] - 32];
		for(size_t i = 0; i < slength; i++)
		{
			c = &(pFont->m_characterInfos[textToDraw[i] - 32]);
			x += (c->ttfA + c->ttfB + c->ttfC);
		}
		
		x -= c->ttfC;
		x *= size;
		return x;
	}

	bool Font::DrawTextToScreen( const std::string& textToDraw, 
		std::shared_ptr<Font>& pFont, 
		const RGBA& color, 
		const Vector2f& topLeft, 
		float size, 
		float aspect)
	{
		PROMISES(pFont);

		s_Shader.load();
		glFrontFace(GL_CW);
		glBindBuffer(GL_ARRAY_BUFFER, pFont->m_idVBO);
		glUniform1i(s_Shader.getUniformID(HASHED_STRING_u_debugInt), 2 );
		glUniform4f(s_Shader.getUniformID(HASHED_STRING_u_color), color.x, color.y, color.z, color.w );
		glUniform1f(s_Shader.getUniformID(HASHED_STRING_u_size), size );
		glUniform3f(s_Shader.getUniformID(HASHED_STRING_u_position), topLeft.x, topLeft.y, 0);

		glActiveTexture( GL_TEXTURE0 );
		glEnable( GL_TEXTURE_2D);
		Texture::ApplyTexture(pFont->m_fontTexture);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(s_Shader.getUniformID(HASHED_STRING_u_diffuse), 0);


		s_Shader.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, pos));
		s_Shader.setAttribute(HASHED_STRING_a_texCoords, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, texCoords));

		
		size_t slength = textToDraw.size();
		unsigned char upperBound = pFont->m_characterInfos.size() + 31;
		float x = 0;
		for(size_t i = 0; i < slength; i++)
		{
			char charToDraw = textToDraw[i];
			if(charToDraw >= 32 && charToDraw < upperBound )
			{
				FontCharacterInfo* c = &(pFont->m_characterInfos[textToDraw[i] - 32]);
				x += c->ttfA;
				glUniform1f(s_Shader.getUniformID(HASHED_STRING_u_offset), x * size);
				glDrawArrays( GL_TRIANGLE_STRIP, ( (c->ucsIndex-32) * 4), 4 );

				x += c->ttfB + c->ttfC;
			}
		}

		s_Shader.disableAttribute(HASHED_STRING_a_position );
		s_Shader.disableAttribute(HASHED_STRING_a_texCoords );
		glDisable( GL_TEXTURE_2D);

		return true;
	}

	bool Font::DrawTextToWorldAlignedWorld( const std::string& textToDraw, 
		std::shared_ptr<Font>& pFont, 
		const RGBA& color, 
		const Vector3f& topLeft,
		const Vector3f& direction,
		float roll,
		float size, 
		float aspect)
	{
		s_Shader.load();
		glDisable(GL_CULL_FACE);
		glBindBuffer(GL_ARRAY_BUFFER, pFont->m_idVBO);
		glUniform1i(s_Shader.getUniformID(HASHED_STRING_u_debugInt), 1 );
		glUniform4f(s_Shader.getUniformID(HASHED_STRING_u_color), color.x, color.y, color.z, color.w );
		glUniform1f(s_Shader.getUniformID(HASHED_STRING_u_size), size );
		glUniformMatrix4fv( s_Shader.getUniformID(HASHED_STRING_u_MV), 1, false, OpenGLApp::Instance()->getCamera().getMatrix().m_Matrix );
		glUniform3f(s_Shader.getUniformID(HASHED_STRING_u_position), topLeft.x, topLeft.y, topLeft.z);
		Matrix4f world;
		ConstructRotationMatrixFromDirectionalVector(world, direction );
		glUniformMatrix4fv( s_Shader.getUniformID(HASHED_STRING_u_WorldTransform), 1, false, world.m_Matrix );

		glActiveTexture( GL_TEXTURE0 );
		glEnable( GL_TEXTURE_2D);
		Texture::ApplyTexture(pFont->m_fontTexture);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(s_Shader.getUniformID(HASHED_STRING_u_diffuse), 0);


		s_Shader.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, pos));
		s_Shader.setAttribute(HASHED_STRING_a_texCoords, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, texCoords));
		size_t slength = textToDraw.size();
		unsigned char upperBound = pFont->m_characterInfos.size() + 31;
		float x = 0;
		for(size_t i = 0; i < slength; i++)
		{
			char charToDraw = textToDraw[i];
			if(charToDraw >= 32 && charToDraw < upperBound )
			{
				FontCharacterInfo* c = &(pFont->m_characterInfos[textToDraw[i] - 32]);
				x += c->ttfA;
				glUniform1f(s_Shader.getUniformID(HASHED_STRING_u_offset), x * size );
				glDrawArrays( GL_TRIANGLE_STRIP, ( (c->ucsIndex-32) * 4), 4 );

				x += c->ttfB + c->ttfC;
			}
		}

		s_Shader.disableAttribute(HASHED_STRING_a_position );
		s_Shader.disableAttribute(HASHED_STRING_a_texCoords );
		glDisable( GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);

		return true;
	}

	bool Font::DrawTextToWorldAlignedScreen( const std::string& textToDraw, 
		std::shared_ptr<Font>& pFont, 
		const RGBA& color, 
		const Vector3f& center,
		float size, 
		float aspect )
	{
		s_Shader.load();
		glDisable(GL_CULL_FACE);
		glBindBuffer(GL_ARRAY_BUFFER, pFont->m_idVBO);
		glUniform1i(s_Shader.getUniformID(HASHED_STRING_u_debugInt), 3 );
		glUniform4f(s_Shader.getUniformID(HASHED_STRING_u_color), color.x, color.y, color.z, color.w );
		glUniform1f(s_Shader.getUniformID(HASHED_STRING_u_size), size );
		glUniformMatrix4fv( s_Shader.getUniformID(HASHED_STRING_u_MV), 1, false, OpenGLApp::Instance()->getCamera().getMatrix().m_Matrix );
		glUniform3f(s_Shader.getUniformID(HASHED_STRING_u_position), center.x, center.y, center.z);
		
		Matrix4f world;
		glUniformMatrix4fv( s_Shader.getUniformID(HASHED_STRING_u_WorldTransform), 1, false, world.m_Matrix );

		glActiveTexture( GL_TEXTURE0 );
		glEnable( GL_TEXTURE_2D);
		Texture::ApplyTexture(pFont->m_fontTexture);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(s_Shader.getUniformID(HASHED_STRING_u_diffuse), 0);


		s_Shader.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, pos));
		s_Shader.setAttribute(HASHED_STRING_a_texCoords, 2, GL_FLOAT, sizeof(GlyphVert), offsetof(GlyphVert, texCoords));
		size_t slength = textToDraw.size();
		unsigned char upperBound = pFont->m_characterInfos.size() + 31;
		float x = 0;
		for(size_t i = 0; i < slength; i++)
		{
			char charToDraw = textToDraw[i];
			if(charToDraw >= 32 && charToDraw < upperBound )
			{
				FontCharacterInfo* c = &(pFont->m_characterInfos[textToDraw[i] - 32]);
				x += c->ttfA;
				glUniform1f(s_Shader.getUniformID(HASHED_STRING_u_offset), x * size );
				glDrawArrays( GL_TRIANGLE_STRIP, ( (c->ucsIndex-32) * 4), 4 );

				x += c->ttfB + c->ttfC;
			}
		}

		s_Shader.disableAttribute(HASHED_STRING_a_position );
		s_Shader.disableAttribute(HASHED_STRING_a_texCoords );
		glDisable( GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);

		return true;
	}

	static bool DrawTextToWorldAlignedAxis( const std::string& textToDraw, 
		std::shared_ptr<Font>& pFont,
		const RGBA& color, 
		const Vector3f& center,
		const Vector3f& axis,
		float size, 
		float aspect = 1.0f);

	std::shared_ptr<Font> Font::CreateOrGetFont( const HashedString& FontName )
	{
		std::shared_ptr<Font> result = 0;

		std::set<std::shared_ptr<Font>, Font::FontCompare >::iterator FontsIterator;
		FontsIterator = Fonts.begin();

		if(FontsIterator != Fonts.end() )
		{
			return *FontsIterator;
		}
		else
		{
			result = std::make_shared<Font>(Font( FontName ));
			Fonts.insert(result);
			return result;
		}
	}

	static void DestroyFont( std::shared_ptr<Font>& fontToDestroy );
	static void DestroyAllFonts();
	static void SetFontDirectory( const std::string& FontDirectory);


}