#pragma once

#include <string>
#include <vector>

#include "..\Math\Vector2.hpp"
#include "..\Engine\Texture.hpp"
#include "LevelTile.hpp"
#include "..\Engine\Shader.hpp"

namespace shadow
{

	class Level
	{
	public:
		Level() {}
		Level( const std::string& levelFile ) {}
		Level(const Vector2i& size, const Vector2f& TileSize );
	


		void generateRandomTiles();

		void loadShader(Shader& shader);
		void createVertexBuffer();
		void render();
		void renderGrid();

		void update(double deltaTime);


	private:
		std::vector<LevelTile> m_tiles;
		Vector2i m_size;
		Vector2f m_tileSize;
		Vector3f m_intersection;
		unsigned int m_indexVBOTerrain;
		unsigned int m_indexVBOGrid;
		std::shared_ptr<Texture> m_terrainTexture;

		Shader m_shaderTerrain;
		Shader m_shaderGrid;


	};
}