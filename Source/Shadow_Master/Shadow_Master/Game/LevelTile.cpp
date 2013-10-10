#pragma once

#include "..\stdafx.h"

#include "LevelTile.hpp"
#include "pugixml.hpp"
#include "..\Engine\Texture.hpp"

#include <sstream>

namespace shadow
{


	void loadTileInfo( const std::string& filePath, std::vector<TileInfo>& tileInfos, TerrainMaterial& material)
	{
		pugi::xml_document* doc = new pugi::xml_document();
		pugi::xml_parse_result result = doc->load_file( (filePath).c_str() );
		pugi::xml_node topNode = doc->first_child();

		if(std::string(topNode.name()).compare(std::string("TileInfos") ) == 0 )
		{
			pugi::xml_node tileNode = topNode.first_child();
			while(tileNode)
			{
				if( std::strcmp(tileNode.name(), "Texture") == 0)
				{
					material.diffuse = Texture::CreateOrGetTexture(ASSETS + "Textures\\" + tileNode.attribute("diffuse").as_string() );
					material.tilesPerRow = tileNode.attribute("tilesPerRow").as_uint();
				}
				else if( std::strcmp(tileNode.name(), "Tile") == 0)
				{
					TileInfo info;
					info.tileID = tileInfos.size();
					info.tileName = tileNode.attribute("type").as_string();
					std::stringstream ss(tileNode.attribute("variations").as_string() );
					while(ss.good())
					{
						unsigned int temp;
						ss >> temp;
						info.variations.push_back(TileRenderData(temp));
					}
					tileInfos.push_back(info);
				}
				tileNode = tileNode.next_sibling();
			}
		}
	}

}