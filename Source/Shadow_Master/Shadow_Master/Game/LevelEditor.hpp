#pragma once

#include <string>
#include <vector>

#include "..\Math\Vector2.hpp"
#include "..\Engine\Texture.hpp"
#include "LevelTile.hpp"
#include "..\Engine\Shader.hpp"
#include "..\Engine\Font.hpp"
#include "..\Engine\EventSystem.hpp"

namespace shadow
{

	class LevelEditor : public EventRecipient<LevelEditor>
	{
	public:
		LevelEditor() : EventRecipient(nullptr) {}
//		LevelEditor( const std::string& levelFile, EventSystem* eventsytem ) {}
		LevelEditor(const Vector2i& size, const Vector2f& TileSize, EventSystem* eventsytem );



		void generateBaseTiles();
		void registerEvents();

		void loadShader(Shader& shader);
		void createTileVertexBuffers();
		void render();
		void renderGrid();
		void renderHud();

		void update(double deltaTime);
		void catchKeyEvent( NamedPropertyContainer& parameters );


	private:

		void checkInput();

		std::vector<TileInfo> m_tileInfos;
		std::vector<LevelTile> m_tiles;
		unsigned int m_currentTileInfo;
		unsigned int m_selectedTile;
		Vector2i m_size;
		Vector2f m_tileSize;
		Vector3f m_intersection;
		unsigned int m_indexVBOTerrain;
		unsigned int m_indexVBOGrid;
		TerrainMaterial m_terrainMaterial;

		Shader m_shaderTerrain;
		Shader m_shaderGrid;

		std::shared_ptr<Font> m_hudFont;

	};
}