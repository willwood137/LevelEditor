#include "..\stdafx.h"
#include "Billboard.hpp"
#include "Camera.hpp"
#include "..\Utility\Utility.hpp"
#include "OpenGLApp.hpp"

namespace shadow
{

	unsigned int Billboard::s_idVBO = 0;
	BillboardShaderInfo Billboard::s_shaderInfo;

	void Billboard::Initialize(BillboardShaderInfo& info)
	{
		s_shaderInfo = info;

		std::vector<BillboardVertex> positionVectors;

		positionVectors.emplace_back(BillboardVertex( Vector2f(0.f, 0.0f) ) );
		positionVectors.emplace_back(BillboardVertex( Vector2f(0.f, 1.0f) ) );
		positionVectors.emplace_back(BillboardVertex( Vector2f(1.f, 0.0f) ) );
		positionVectors.emplace_back(BillboardVertex( Vector2f(1.f, 1.0f) ) );

		s_idVBO = LoadBufferObject( GL_ARRAY_BUFFER, positionVectors.data(), sizeof( BillboardVertex ) * 4 );
	}

	void Billboard::RenderBillboardAtPoint(std::shared_ptr<Texture>& texture, Vector3f& Position, Vector2f& Size )
	{
		glUseProgram(s_shaderInfo.program);


		glBindBuffer( GL_ARRAY_BUFFER, s_idVBO );

		Vector3f camPos = OpenGLApp::Instance()->getCamera().getPosition();
		Matrix4f camMatrix = OpenGLApp::Instance()->getCamera().getMatrix();
		glUniform3f( s_shaderInfo.uniformCamPosition,  camPos.x, camPos.y, camPos.z);
		glUniform2f( s_shaderInfo.uniformSize, Size.x, Size.y );
		glUniform3f( s_shaderInfo.uniformPosition, Position.x, Position.y, Position.z);
		glUniformMatrix4fv( s_shaderInfo.uniformViewMatrix, 1, false, camMatrix.m_Matrix);

		Texture::ApplyTexture(texture);

		

		

		glVertexAttribPointer(s_shaderInfo.attributeTextureCoordinates, 2, GL_FLOAT,GL_FALSE,
			sizeof(BillboardVertex), reinterpret_cast< void* >(offsetof(BillboardVertex, texCoords) ) );
		glEnableVertexAttribArray(s_shaderInfo.attributeTextureCoordinates);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(s_shaderInfo.attributeTextureCoordinates);
	}
}