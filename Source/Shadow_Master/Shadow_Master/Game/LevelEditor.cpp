#include "..\stdafx.h"

#include "LevelEditor.hpp"
#include "..\Utility\Utility.hpp"
#include "..\Engine\OpenGLApp.hpp"
#include "..\Math\Ray.hpp"

#include <sstream>

namespace shadow
{


	const float TERRAIN_TEXTURE_TILE_SIZE = 1.0f / 16.0f;
	const std::string SHADER_LevelEditor_GRID_SOURCE_FILE_PATH = "Shaders\\TerrainGrid";
	const int MAX_GRID_LINES = 25;

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	LevelEditor::LevelEditor(const Vector2i& size, const Vector2f& TileSize, EventSystem* eventsytem )
		: m_size(size), 
		m_tileSize(TileSize),
		EventRecipient(eventsytem)
	{ 

		m_currentTileInfo = 0;
		m_selectedTile = -1;
		loadTileInfo("Game\\TileInfos.xml", m_tileInfos, m_terrainMaterial);
		createTileVertexBuffers();
		m_hudFont = Font::CreateOrGetFont(HASHED_STRING_MainFont_EN);
		//generateBaseTiles();
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void LevelEditor::registerEvents()
	{
		p_eventSystem->RegisterObjectForEvent(this, &LevelEditor::catchKeyEvent, "KeyDown");
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void LevelEditor::generateBaseTiles()
	{
		m_currentTileInfo = 0;
		size_t numTileTypes =  m_tileInfos.size();
 		for(int x = 0; x < m_size.x; ++x)
 		{
 			for(int y= 0; y < m_size.y; ++y)
 			{
 				LevelTile tile;
 				tile.m_coordinates = Vector2i(x, y);
 				tile.m_info = &(m_tileInfos[rand()%numTileTypes]);
				tile.m_variation = (rand()%tile.m_info->variations.size());
 				m_tiles.push_back(tile);
 			}
 		}
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void LevelEditor::loadShader(Shader& shader)
	{
		m_shaderTerrain = shader;
		m_shaderTerrain.addUniform(HASHED_STRING_u_MV);
		m_shaderTerrain.addUniform(HASHED_STRING_u_diffuse);
		m_shaderTerrain.addUniform(HASHED_STRING_u_intersection);
		m_shaderTerrain.addUniform(HASHED_STRING_u_position);
		m_shaderTerrain.addAttribute(HASHED_STRING_a_position);
		m_shaderTerrain.addAttribute(HASHED_STRING_a_texCoords);

		m_shaderGrid = Shader(SHADER_LevelEditor_GRID_SOURCE_FILE_PATH);
		m_shaderGrid.addUniform(HASHED_STRING_u_MV);
		m_shaderGrid.addUniform(HASHED_STRING_u_color);
		m_shaderGrid.addUniform(HASHED_STRING_u_position);
		m_shaderGrid.addUniform(HASHED_STRING_u_minGrid);
		m_shaderGrid.addUniform(HASHED_STRING_u_maxGrid);
		m_shaderGrid.addAttribute(HASHED_STRING_a_position);

	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void LevelEditor::createTileVertexBuffers()
	{
		std::vector<TileVertex> verts;
		std::vector<TileInfo>::iterator it;
		for(it = m_tileInfos.begin(); it != m_tileInfos.end(); ++it)
		{
			size_t numVariations = it->variations.size();
			for(size_t i = 0; i < numVariations; ++i)
			{
				float col = static_cast<float>( (it->variations[i].m_tileCoord - 1) % 16 );
				float row = floor(static_cast<float>(it->variations[i].m_tileCoord) / 16);
				Vector2f uvOffset(col * TERRAIN_TEXTURE_TILE_SIZE, row * TERRAIN_TEXTURE_TILE_SIZE);

				it->variations[i].m_vboStart = verts.size();

				TileVertex vert;
				vert.pos = Vector3f(0, 0, 0);
				vert.uv = uvOffset + Vector2f(0.f, TERRAIN_TEXTURE_TILE_SIZE);
				verts.push_back(vert);

				vert.pos = Vector3f(0, 0, m_tileSize.y);
				vert.uv = uvOffset + Vector2f(0.f, 0.f);
				verts.push_back(vert);

				vert.pos = Vector3f(m_tileSize.x, 0, 0);
				vert.uv = uvOffset + Vector2f(TERRAIN_TEXTURE_TILE_SIZE, TERRAIN_TEXTURE_TILE_SIZE);
				verts.push_back(vert);

				vert.pos = Vector3f(m_tileSize.x, 0, m_tileSize.y);
				vert.uv = uvOffset + Vector2f(TERRAIN_TEXTURE_TILE_SIZE, 0);
				verts.push_back(vert);
			}
		}


		m_indexVBOTerrain = LoadBufferObject( GL_ARRAY_BUFFER, verts.data(), sizeof( TileVertex ) * verts.size() );

		//Grid System
		//
		std::vector<Vector2f> gridVerts;
		for( int x = -MAX_GRID_LINES;  x <= MAX_GRID_LINES; ++x)
		{
			gridVerts.push_back(Vector2f( x * m_tileSize.x, -100.0f * m_tileSize.y ));
			gridVerts.push_back(Vector2f( x * m_tileSize.x, 100.0f * m_tileSize.y ));
		}
		for( int y = -MAX_GRID_LINES;  y <= MAX_GRID_LINES; ++y)
		{
			gridVerts.push_back(Vector2f( -100.0f * m_tileSize.x, y * m_tileSize.y ));
			gridVerts.push_back(Vector2f( 100.0f * m_tileSize.x, y * m_tileSize.y ));
		}

		m_indexVBOGrid = LoadBufferObject( GL_ARRAY_BUFFER, gridVerts.data(), sizeof( Vector2f ) * gridVerts.size() );


	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void LevelEditor::render()
	{
		m_shaderTerrain.load();

		//glFrontFace(GL_CCW);
		glDisable(GL_CULL_FACE);

		unsigned int mvID = m_shaderTerrain.getUniformID(HASHED_STRING_u_MV);
		glUniformMatrix4fv( mvID, 1, false, OpenGLApp::Instance()->getCamera().getMatrix().m_Matrix );
		glUniform3f(m_shaderTerrain.getUniformID(HASHED_STRING_u_intersection), m_intersection.x, m_intersection.y, m_intersection.z);


		glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT );
		glBindBuffer( GL_ARRAY_BUFFER, m_indexVBOTerrain );

		glActiveTexture( GL_TEXTURE0 );
		glEnable( GL_TEXTURE_2D);

		Texture::ApplyTexture( m_terrainMaterial.diffuse );
		glUniform1i(m_shaderTerrain.getUniformID(HASHED_STRING_u_diffuse), 0);

		m_shaderTerrain.setAttribute(HASHED_STRING_a_position, 3, GL_FLOAT, sizeof(TileVertex), offsetof(TileVertex, pos) );
		m_shaderTerrain.setAttribute(HASHED_STRING_a_texCoords, 2, GL_FLOAT, sizeof(TileVertex), offsetof(TileVertex, uv) );
		
		size_t numTiles = m_tiles.size();
		for( size_t i = 0; i < numTiles; ++i)
		{
			glUniform2f(m_shaderTerrain.getUniformID(HASHED_STRING_u_position), m_tiles[i].m_coordinates.x * m_tileSize.x, m_tiles[i].m_coordinates.y * m_tileSize.y);
			glDrawArrays( GL_TRIANGLE_STRIP, m_tiles[i].m_info->variations[m_tiles[i].m_variation].m_vboStart, 4);
		}

		m_shaderTerrain.disableAttribute(HASHED_STRING_a_position);
		m_shaderTerrain.disableAttribute(HASHED_STRING_a_texCoords);
		glDisable( GL_TEXTURE_2D);
		glPopAttrib();

		renderGrid();
		renderHud();
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void LevelEditor::renderGrid()
	{

		//Grid system
		//
		m_shaderGrid.load();


		glBindBuffer( GL_ARRAY_BUFFER, m_indexVBOGrid );
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUniform4f( m_shaderGrid.getUniformID(HASHED_STRING_u_color), .6f, .6f, .2f, 1.0f );
		glUniform2f( m_shaderGrid.getUniformID(HASHED_STRING_u_minGrid), 0.f, 0.f);
		glUniform2f( m_shaderGrid.getUniformID(HASHED_STRING_u_maxGrid), m_tileSize.x * m_size.x, m_tileSize.y * m_size.y );

		unsigned int mvID = m_shaderGrid.getUniformID(HASHED_STRING_u_MV);
		glUniformMatrix4fv( mvID, 1, false, OpenGLApp::Instance()->getCamera().getMatrix().m_Matrix );

		m_shaderGrid.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(Vector2f), 0 );
		glLineWidth(4.0);
		glDrawArrays( GL_LINES, 0, 804);
		m_shaderGrid.disableAttribute(HASHED_STRING_a_position);

		glDisable( GL_TEXTURE_2D);

		glPopAttrib();
	}
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void LevelEditor::renderHud()
	{
		Font::DrawTextToScreen(m_tileInfos[m_currentTileInfo].tileName, m_hudFont, RGBA(0.2f, 0.0f, 1.0f, 1.0f), Vector2f(-.9f, .9f), .1f);

		if( m_selectedTile != -1)
		{
			Font::DrawTextToScreen(m_tiles[m_selectedTile].m_info->tileName, m_hudFont, RGBA(1.0f, 0.0f, 0.0f, 1.0f), Vector2f(-.9f, .8f), .066f);
		}
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void LevelEditor::update(double deltaTime)
	{
		Vector2f cursorTemp(OpenGLApp::Instance()->getCursorPositionRelativeToWindow());

		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLfloat winX, winY;
		GLdouble posX, posY, posZ;

		glGetDoublev( GL_PROJECTION_MATRIX, projection );
		glGetIntegerv( GL_VIEWPORT, viewport );

		winX = (float)cursorTemp.x;
		winY = (float)viewport[3] - (float)cursorTemp.y;

		Matrix4f camMat = OpenGLApp::Instance()->getCamera().getMatrix();
		for(size_t i = 0; i < 16; ++i)
		{
			modelview[i] = (double)(camMat[i]);
		}

		gluUnProject( winX, winY, 0.f, modelview, projection, viewport, &posX, &posY, &posZ);

		Vector3f cursor((float)posX, (float)posY, (float)posZ);



		//Ray cursorRay( OpenGLApp::Instance()->getCamera().getPosition(), cursor - OpenGLApp::Instance()->getCamera().getPosition() );
		Vector3f origin = OpenGLApp::Instance()->getCamera().getPosition();
		Vector3f direction = (cursor - origin);
		Vector3f normal(0.f, 1.0f, 0.f);
		float d = -300.0f;

		float travel = ( origin.DotProduct(normal) + d) / direction.DotProduct(normal);

		Vector3f intersection = origin + direction * travel;

		Vector2i intersectionTile( (int)(intersection.x / m_tileSize.x), (int)(intersection.z / m_tileSize.y) );

		m_intersection = Vector3f((intersectionTile.x + .5f)*m_tileSize.x, 0.0f, (intersectionTile.y + .5f) *m_tileSize.y);
		
		Vector2i coords(m_intersection.x / m_tileSize.x, m_intersection.z / m_tileSize.y);
		size_t numTiles = m_tiles.size();
		for( size_t i = 0; i < numTiles; ++i)
		{
			if( m_tiles[i].m_coordinates == coords )
			{
				m_selectedTile = i;
				break;
			}
		}

		checkInput();
	}

	void LevelEditor::checkInput()
	{
		bool mouse1 = isKeyDown(VK_LBUTTON);
		if( mouse1 )
		{
			if( m_selectedTile != -1)
			{
				m_tiles[m_selectedTile].m_info = &m_tileInfos[m_currentTileInfo];
				m_tiles[m_selectedTile].m_variation = rand()%m_tileInfos[m_currentTileInfo].variations.size();
			}
		}
	}

	void LevelEditor::catchKeyEvent( NamedPropertyContainer& parameters )
	{
		unsigned int key; 
		parameters.getNamedData("Key", key);

		switch( key)
		{
		case VK_OEM_4:
			m_currentTileInfo = clamp<unsigned int>(m_currentTileInfo - 1, (unsigned int)0, (unsigned int)m_tileInfos.size() - 1 );
			break;
		case VK_OEM_6:
			m_currentTileInfo = clamp<unsigned int>(m_currentTileInfo + 1, 0, m_tileInfos.size() - 1 );
			break;
		}
	}
}