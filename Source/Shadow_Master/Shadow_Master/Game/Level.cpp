#include "..\stdafx.h"
// 
// #include "Level.hpp"
// #include "..\Utility\Utility.hpp"
// #include "..\Engine\OpenGLApp.hpp"
// #include "..\Math\Ray.hpp"
// 
// #include <sstream>
// 
 namespace shadow
 {
// 	
// 
// 	const float TERRAIN_TEXTURE_TILE_SIZE = 1.0f / 16.0f;
// 	const std::string SHADER_LEVEL_GRID_SOURCE_FILE_PATH = "Shaders\\TerrainGrid";
// 
// 	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 
// 	Level::Level(const Vector2i& size, const Vector2f& TileSize ): m_size(size), m_tileSize(TileSize) 
// 	{ 
// 		generateRandomTiles(); 
// 		m_terrainTexture = Texture::CreateOrGetTexture(ASSETS + "Textures\\terrain.png");
// 	}
// 
// 	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 
// 	void Level::generateRandomTiles()
// 	{
// 		for(int x = 0; x < m_size.x; ++x)
// 		{
// 			for(int y= 0; y < m_size.y; ++y)
// 			{
// 				LevelTile tile;
// 				tile.m_coordinates = Vector2i(x, y);
// 				tile.m_type = static_cast<TileType>(rand()%TILETYPE_SIZE);
// 				m_tiles.push_back(tile);
// 			}
// 		}
// 	}
// 
// 	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 
// 	void Level::loadShader(Shader& shader)
// 	{
// 		m_shaderTerrain = shader;
// 		m_shaderTerrain.addUniform(HASHED_STRING_u_MV);
// 		m_shaderTerrain.addUniform(HASHED_STRING_u_diffuse);
// 		m_shaderTerrain.addUniform(HASHED_STRING_u_intersection);
// 		m_shaderTerrain.addAttribute(HASHED_STRING_a_position);
// 		m_shaderTerrain.addAttribute(HASHED_STRING_a_texCoords);
// 		m_shaderTerrain.addAttribute(HASHED_STRING_a_color);
// 
// 		m_shaderGrid = Shader(SHADER_LEVEL_GRID_SOURCE_FILE_PATH);
// 		m_shaderGrid.addUniform(HASHED_STRING_u_MV);
// 		m_shaderGrid.addUniform(HASHED_STRING_u_color);
// 		m_shaderGrid.addUniform(HASHED_STRING_a_position);
// 		m_shaderGrid.addUniform(HASHED_STRING_u_minGrid);
// 		m_shaderGrid.addUniform(HASHED_STRING_u_maxGrid);
// 		m_shaderGrid.addAttribute(HASHED_STRING_a_position);
// 
// 	}
// 
// 	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 
// 	void Level::createVertexBuffer()
// 	{
// 		std::vector<TileVertex> verts;
// 		std::vector<LevelTile>::iterator it;
// 		for(it = m_tiles.begin(); it != m_tiles.end(); ++it)
// 		{
// 			Vector2f uvOffset(0.f, 0.f);
// 			Vector3f color(1.0f, 1.0f, 1.0f);
// 
// 			switch (it->m_type)
// 			{
// 			case TILETYPE_GRASS:
// 				uvOffset = Vector2f(TERRAIN_TEXTURE_TILE_SIZE * 0.0f, TERRAIN_TEXTURE_TILE_SIZE * 0.0f);
// 				color.x = .4f;
// 				color.z = .4f;
// 				break;
// 			case TILETYPE_RUBBLE:
// 				uvOffset = Vector2f(TERRAIN_TEXTURE_TILE_SIZE * 0.0f, TERRAIN_TEXTURE_TILE_SIZE * 0.0f);
// 				break;
// 			case TILETYPE_STONE:
// 				uvOffset = Vector2f(TERRAIN_TEXTURE_TILE_SIZE * 7.0f, TERRAIN_TEXTURE_TILE_SIZE * 0.0f);
// 				break;
// 			}
// 
// 
// 			TileVertex vert;
// 			vert.color = color;
// 
// 			vert.pos = Vector3f(it->m_coordinates.x * m_tileSize.x, 0, it->m_coordinates.y * m_tileSize.y);
// 			vert.uv = uvOffset + Vector2f(0.f, 0.f);
// 			verts.push_back(vert);
// 
// 			vert.pos = Vector3f(it->m_coordinates.x * m_tileSize.x, 0, it->m_coordinates.y * m_tileSize.y + m_tileSize.y);
// 			vert.uv = uvOffset + Vector2f(0.f, TERRAIN_TEXTURE_TILE_SIZE);
// 			verts.push_back(vert);
// 
// 			vert.pos = Vector3f(it->m_coordinates.x * m_tileSize.x + m_tileSize.x, 0, it->m_coordinates.y * m_tileSize.y + m_tileSize.y);
// 			vert.uv = uvOffset + Vector2f(TERRAIN_TEXTURE_TILE_SIZE, TERRAIN_TEXTURE_TILE_SIZE);
// 			verts.push_back(vert);
// 
// 			vert.pos = Vector3f(it->m_coordinates.x * m_tileSize.x, 0, it->m_coordinates.y * m_tileSize.y);
// 			vert.uv = uvOffset + Vector2f(0.f, 0.f);
// 			verts.push_back(vert);
// 
// 			vert.pos = Vector3f(it->m_coordinates.x * m_tileSize.x + m_tileSize.x, 0, it->m_coordinates.y * m_tileSize.y + m_tileSize.y);
// 			vert.uv = uvOffset + Vector2f(TERRAIN_TEXTURE_TILE_SIZE, TERRAIN_TEXTURE_TILE_SIZE);
// 			verts.push_back(vert);
// 
// 			vert.pos = Vector3f(it->m_coordinates.x * m_tileSize.x + m_tileSize.x, 0, it->m_coordinates.y * m_tileSize.y);
// 			vert.uv = uvOffset + Vector2f(TERRAIN_TEXTURE_TILE_SIZE, 0.f);
// 			verts.push_back(vert);
// 		}
// 
// 
// 		m_indexVBOTerrain = LoadBufferObject( GL_ARRAY_BUFFER, verts.data(), sizeof( TileVertex ) * verts.size() );
// 		
// 		//Grid System
// 		//
// 		std::vector<Vector2f> gridVerts;
// 		for( int x = -100;  x <= 100; ++x)
// 		{
// 			gridVerts.push_back(Vector2f( x * m_tileSize.x, -100.0f * m_tileSize.y ));
// 			gridVerts.push_back(Vector2f( x * m_tileSize.x, 100.0f * m_tileSize.y ));
// 		}
// 		for( int y = -100;  y <= 100; ++y)
// 		{
// 			gridVerts.push_back(Vector2f( -100.0f * m_tileSize.x, y * m_tileSize.y ));
// 			gridVerts.push_back(Vector2f( 100.0f * m_tileSize.x, y * m_tileSize.y ));
// 		}
// 
// 		m_indexVBOGrid = LoadBufferObject( GL_ARRAY_BUFFER, gridVerts.data(), sizeof( Vector2f ) * gridVerts.size() );
// 
// 	
// 	}
// 
// 	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 
// 	void Level::render()
// 	{
// 		m_shaderTerrain.load();
// 
// 		glFrontFace(GL_CCW);
// 
// 		unsigned int mvID = m_shaderTerrain.getUniformID(HASHED_STRING_u_MV);
// 		glUniformMatrix4fv( mvID, 1, false, OpenGLApp::Instance()->getCamera().getMatrix().m_Matrix );
// 		glUniform3f(m_shaderTerrain.getUniformID(HASHED_STRING_u_intersection), m_intersection.x, m_intersection.y, m_intersection.z);
// 		
// 
// 		glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT );
// 		glBindBuffer( GL_ARRAY_BUFFER, m_indexVBOTerrain );
// 
// 		glActiveTexture( GL_TEXTURE0 );
// 		glEnable( GL_TEXTURE_2D);
// 
// 		Texture::ApplyTexture( m_terrainTexture );
// 		glUniform1i(m_shaderTerrain.getUniformID(HASHED_STRING_u_diffuse), 0);
// 		
// 		m_shaderTerrain.setAttribute(HASHED_STRING_a_position, 3, GL_FLOAT, sizeof(TileVertex), offsetof(TileVertex, pos) );
// 		m_shaderTerrain.setAttribute(HASHED_STRING_a_texCoords, 2, GL_FLOAT, sizeof(TileVertex), offsetof(TileVertex, uv) );
// 		m_shaderTerrain.setAttribute(HASHED_STRING_a_color, 3, GL_FLOAT, sizeof(TileVertex), offsetof(TileVertex, color) );
// 		
// 
// 		glDrawArrays( GL_TRIANGLES, 0, m_tiles.size() * 6 );
// 
// 		m_shaderTerrain.disableAttribute(HASHED_STRING_a_position);
// 		m_shaderTerrain.disableAttribute(HASHED_STRING_a_texCoords);
// 		m_shaderTerrain.disableAttribute(HASHED_STRING_a_color);
// 		glDisable( GL_TEXTURE_2D);
// 		glPopAttrib();
// 
// 		renderGrid();
// 	}
// 
// 	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 
// 	void Level::renderGrid()
// 	{
// 
// 		//Grid system
// 		//
// 		m_shaderGrid.load();
// 
// 
// 		glBindBuffer( GL_ARRAY_BUFFER, m_indexVBOGrid );
// 		glEnable(GL_BLEND);
// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 
// 		glUniform4f( m_shaderGrid.getUniformID(HASHED_STRING_u_color), .6f, .6f, .2f, 1.0f );
// 		glUniform2f( m_shaderGrid.getUniformID(HASHED_STRING_u_minGrid), 0.f, 0.f);
// 		glUniform2f( m_shaderGrid.getUniformID(HASHED_STRING_u_maxGrid), m_tileSize.x * m_size.x, m_tileSize.y * m_size.y );
// 
// 		unsigned int mvID = m_shaderGrid.getUniformID(HASHED_STRING_u_MV);
// 		glUniformMatrix4fv( mvID, 1, false, OpenGLApp::Instance()->getCamera().getMatrix().m_Matrix );
// 
// 		m_shaderGrid.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(Vector2f), 0 );
// 		glLineWidth(4.0);
// 		glDrawArrays( GL_LINES, 0, 804);
// 		m_shaderGrid.disableAttribute(HASHED_STRING_a_position);
// 
// 		glDisable( GL_TEXTURE_2D);
// 
// 		glPopAttrib();
// 	}
// 
// 	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 
// 	void Level::update(double deltaTime)
// 	{
//   		Vector2f cursorTemp(OpenGLApp::Instance()->getCursorPositionRelativeToWindow());
// 
// 		GLint viewport[4];
// 		GLdouble modelview[16];
// 		GLdouble projection[16];
// 		GLfloat winX, winY;
// 		GLdouble posX, posY, posZ;
// 
// 		glGetDoublev( GL_PROJECTION_MATRIX, projection );
// 		glGetIntegerv( GL_VIEWPORT, viewport );
// 
// 		winX = (float)cursorTemp.x;
// 		winY = (float)viewport[3] - (float)cursorTemp.y;
// 		
// 		Matrix4f camMat = OpenGLApp::Instance()->getCamera().getMatrix();
// 		for(size_t i = 0; i < 16; ++i)
// 		{
// 			modelview[i] = (double)(camMat[i]);
// 		}
// 
// 		gluUnProject( winX, winY, 0.f, modelview, projection, viewport, &posX, &posY, &posZ);
// 
// 		Vector3f cursor((float)posX, (float)posY, (float)posZ);
// 
// 		
// 
// 		//Ray cursorRay( OpenGLApp::Instance()->getCamera().getPosition(), cursor - OpenGLApp::Instance()->getCamera().getPosition() );
// 		Vector3f origin = OpenGLApp::Instance()->getCamera().getPosition();
// 		Vector3f direction = (cursor - origin);
// 		Vector3f normal(0.f, 1.0f, 0.f);
// 		float d = -300.0f;
// 
// 		float travel = ( origin.DotProduct(normal) + d) / direction.DotProduct(normal);
// 
// 		Vector3f intersection = origin + direction * travel;
// 
// 		Vector2i intersectionTile( (int)(intersection.x / m_tileSize.x), (int)(intersection.z / m_tileSize.y) );
// 
// 		m_intersection = Vector3f((intersectionTile.x + .5f)*m_tileSize.x, 0.0f, (intersectionTile.y + .5f) *m_tileSize.y);
// 
// 
// 	}
 }