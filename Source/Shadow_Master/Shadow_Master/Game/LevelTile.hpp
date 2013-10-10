#pragma once

#include <string>
#include <vector>

#include "..\Math\Vector2.hpp"
#include "..\Math\Vector3.hpp"
#include "..\Engine\Texture.hpp"

namespace shadow
{
	struct TerrainMaterial
	{
		std::shared_ptr<Texture> diffuse;
		unsigned int tilesPerRow;
	};
	
	struct TileRenderData
	{
		unsigned int m_tileCoord;
		unsigned int m_vboStart; // fist vertex in the tile vbo
		TileRenderData() {}
		TileRenderData( unsigned int coord ): m_tileCoord(coord) {}
	};

	struct TileInfo
	{
		unsigned int tileID;
		std::string tileName;
		std::vector<TileRenderData> variations;
	};
	struct LevelTile
	{
		Vector2i m_coordinates;
		TileInfo* m_info;
		unsigned short m_variation;
	};

	struct TileVertex
	{
		Vector3f pos;
		Vector2f uv;
	};

	void loadTileInfo( const std::string& filePath, std::vector<TileInfo>& tileInfos, TerrainMaterial& material);
}