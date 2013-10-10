#pragma once

#include "..\Math\Vector3.hpp"
#include "..\Math\Vector2.hpp"
#include "..\Math\Matrix4.hpp"
#include "Texture.hpp"
#include <vector>


namespace shadow
{
	struct BillboardShaderInfo
	{
		unsigned int program;
		
		unsigned int uniformCamPosition;
		unsigned int uniformSize;
		unsigned int uniformPosition;
		unsigned int uniformViewMatrix;

		unsigned int attributePosition;
		unsigned int attributeTextureCoordinates;
	};

	struct BillboardVertex
	{
		Vector2f texCoords;

		BillboardVertex( Vector2f tex) : texCoords(tex) {}
	};

	class Billboard
	{
	public:

		static void Initialize(BillboardShaderInfo& info);
		static void RenderBillboardAtPoint(std::shared_ptr<Texture>& texture, Vector3f& Position, Vector2f& Size );
	private:
		static BillboardShaderInfo s_shaderInfo;
		static unsigned int s_idVBO;

	};
}