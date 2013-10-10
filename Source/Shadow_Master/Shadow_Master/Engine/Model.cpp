#include "..\stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Billboard.hpp"
#include "Model.hpp"
#include "..\Utility\Utility.hpp"

namespace shadow
{
	unsigned int programID;

	void Model::ModelNode::AddChildNode(std::shared_ptr<ModelNode> node)
	{
		m_children.push_back(node);
	}
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	// readStringFromFile(std::string& outString, std::ifstream& in, unsigned int offset)
	//
	// Reads a string from a file starting at offset.  format is assumed that first is a unsigned int giving
	// the length (number of chars) of the string followed by a string, all in binary format.
	//
	// return length of string;
	//
	unsigned int readStringFromFile(std::string& outString, std::ifstream& in, unsigned int offset)
	{
		in.seekg(offset, std::ios::beg);
		unsigned int stringLength;

		in.read(reinterpret_cast<char*>(&stringLength), sizeof(unsigned int) );
		if( stringLength > 0)
		{
			outString.resize(stringLength);
			in.read(reinterpret_cast<char*>(&outString[0]), sizeof(char)*stringLength );
		}

		return stringLength;
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void readMaterialsFromMaxFile(const Lump &materialLump, std::vector<std::shared_ptr<Material> >& materials, std::ifstream& in)
	{
		unsigned int offset = materialLump.Offset;
		unsigned int offsetEnd = offset + materialLump.Length;

		while(offset < offsetEnd)
		{
			Material newMaterial;
			offset += readStringFromFile(newMaterial.m_name, in, offset) + sizeof(unsigned int);

			// read diffuse texture
			//
			std::string diffuse;
			offset += readStringFromFile(diffuse, in, offset) + sizeof(unsigned int);

			if(diffuse.length() > 0)
				newMaterial.m_diffuseTexture = Texture::CreateOrGetTexture("Textures\\" + diffuse);
			else
				newMaterial.m_diffuseTexture = nullptr;

			std::string normal;
			offset += readStringFromFile(normal, in, offset) + sizeof(unsigned int);

			if(normal.length() > 0)
				newMaterial.m_normalMap = Texture::CreateOrGetTexture("Textures\\" + normal);
			else
				newMaterial.m_normalMap = nullptr;



	


			materials.push_back(std::make_shared<Material>(newMaterial) );
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void readCameraDataFromMaxFile(const Lump &cameraLump, 
		std::vector<std::shared_ptr<Model::ModelNode> >& nodes, 
		std::vector<std::shared_ptr<Model::ModelNode> >& Cameras, 
		std::ifstream& in)
	{

		in.seekg(cameraLump.Offset, std::ios::beg);

		unsigned int numCameras;
		in.read((char*)&numCameras , sizeof(unsigned int));
		
		if( numCameras > 0)
		{
			std::vector<unsigned int> cameraData;
			cameraData.resize(numCameras);
			Cameras.resize(numCameras);
			in.read((char*)&cameraData[0], sizeof(unsigned int) * numCameras );
			
			for(unsigned int i = 0; i < numCameras; ++i)
			{
				Cameras[i] = nodes[ cameraData[i] ];
			}
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void readTriangleBatchesFromMaxFile(const Lump &TriangleBatchLump, 
		std::vector<std::shared_ptr<TriangleBatch> >& batches, 
		const std::vector<std::shared_ptr<Material> >& materials, 
		std::ifstream& in)
	{
		unsigned int offset = TriangleBatchLump.Offset;
		unsigned int offsetEnd = offset + TriangleBatchLump.Length;


		in.seekg(offset, std::ios::beg);
		while(offset < offsetEnd)
		{
			std::shared_ptr<TriangleBatch> tBatch = std::make_shared<TriangleBatch>( TriangleBatch() );
			int matId;
			in.read((char*)&matId , sizeof(int));
			offset += sizeof(int);

			if( matId >= 0)
				tBatch->m_material = materials[matId];
			else
				tBatch->m_material = nullptr;

			in.read((char*)&tBatch->numIndices, sizeof(unsigned int));
			offset += sizeof(unsigned int);

			in.read((char*)&tBatch->numVertices, sizeof(unsigned int));
			offset += sizeof(unsigned int);

			//in.seekg(offset, std::ios::beg);
			tBatch->m_verts.resize(tBatch->numVertices);
			in.read((char*)&tBatch->m_verts[0], sizeof(Vertex) * tBatch->numVertices );
			offset += sizeof(Vertex) * tBatch->numVertices;
			
		
			for(unsigned int n = 0; n < tBatch->numVertices; ++n)
			{
				if(tBatch->m_verts[n].bones[0] == 20)
					 bool x = false;
			}

			tBatch->m_indicies.resize(tBatch->numIndices);
			//in.seekg(offset, std::ios::beg);
			in.read((char*)&tBatch->m_indicies[0], sizeof(unsigned int) * tBatch->numIndices);
			offset += sizeof(unsigned int) * tBatch->numIndices;

			tBatch->idVBO = LoadBufferObject( GL_ARRAY_BUFFER, tBatch->m_verts.data(), sizeof( Vertex ) * tBatch->numVertices );
			tBatch->idIBO = LoadBufferObject( GL_ELEMENT_ARRAY_BUFFER, tBatch->m_indicies.data(), sizeof( unsigned int ) * tBatch->numIndices );

			batches.push_back( tBatch );
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void readModelNodesFromMaxFile(const Lump &NodeLump,
		std::vector<std::shared_ptr<Model::ModelNode> >& nodes, 
		std::vector<std::shared_ptr<Model::ModelNode> >& topNodes,
		std::vector<std::shared_ptr<Model::ModelNode> >& Bones,
		std::vector< std::shared_ptr< TriangleBatch> >& batches, 
		std::ifstream& in)
	{
		std::vector<MaxModelNodeInfo> MaxInfos;


		//--!--approximation--!--
		MaxInfos.reserve( NodeLump.Length / sizeof(MaxModelNodeInfo) );

		unsigned int offset = NodeLump.Offset;
		unsigned int offsetEnd = offset + NodeLump.Length;


		in.seekg(offset, std::ios::beg);

		while( offset  < offsetEnd )
		{
			Model::ModelNode node;
			MaxModelNodeInfo nodeInfo;
			unsigned int numFrames;
			unsigned int numChildren;
			unsigned int numBatches;
			int BoneID;
			std::vector<unsigned int> batchIDs;

			offset += readStringFromFile(node.m_name, in, offset) + sizeof(unsigned int);

			in.read((char*)&nodeInfo.parent, sizeof(int));
			offset += sizeof(int);

			in.read((char*)&numFrames, sizeof(unsigned int));
			offset += sizeof(unsigned int);

			in.read((char*)&numChildren, sizeof(unsigned int));
			offset += sizeof(unsigned int);

			in.read((char*)&numBatches, sizeof(unsigned int));
			offset += sizeof(unsigned int);

			in.read((char*)&BoneID, sizeof(unsigned int));
			offset += sizeof(unsigned int);

			node.m_localTMs.resize( numFrames );
			in.read((char*)&node.m_localTMs[0], sizeof(float) * 16 * numFrames);
			offset += sizeof(float) * 16 * numFrames;


			in.read((char*)&node.m_inverseRestMatrix, sizeof(float) * 16);
			offset += sizeof(float) * 16;

			if(numChildren > 0)
			{
				nodeInfo.childrenIDs.resize(numChildren);
				in.read((char*)&nodeInfo.childrenIDs[0], sizeof(unsigned int) * numChildren);
				offset += sizeof(unsigned int) * numChildren;
			}

			if(numBatches > 0)
			{
				batchIDs.resize(numBatches);
				in.read((char*)&batchIDs[0], sizeof(unsigned int) * numBatches);
				offset += sizeof(unsigned int) * numBatches;

				node.m_batches.reserve(numBatches);
				for(unsigned int i = 0; i < numBatches; ++i)
				{
					node.m_batches.push_back( batches[batchIDs[i] ] );
				}

			}

			MaxInfos.push_back(nodeInfo);
			std::shared_ptr<Model::ModelNode> nodePtr = std::make_shared<Model::ModelNode>(node);
			nodes.push_back( nodePtr );

			if(BoneID >= 0 && BoneID < (int)Bones.size())
				Bones[BoneID] = nodePtr;

		}

		//make Node Tree
		size_t numNodes = MaxInfos.size();
		for(size_t i = 0; i < numNodes; ++i)
		{
			if(MaxInfos[i].parent == -1)
			{
				//this is a toplevelnode
				topNodes.push_back( nodes[i] );
				nodes[i]->m_parent = nullptr;
			}
			else
			{
				nodes[i]->m_parent = nodes[MaxInfos[i].parent];
			}

			//add its children
			size_t numChildren = MaxInfos[i].childrenIDs.size();
			for( size_t c = 0; c < numChildren; ++c)
			{
				nodes[i]->AddChildNode(nodes[ MaxInfos[i].childrenIDs[c] ] );
			}
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void Model::ReadDataFromMaxFile( const char* filepath )
	{
		Header ModelHeader;
		std::vector<std::shared_ptr<TriangleBatch> > batches;
		std::vector<std::shared_ptr<Material> > materials;
		std::vector<std::shared_ptr<Model::ModelNode> > nodes;


		m_defaultTexture = Texture::CreateOrGetTexture("Textures\\rgb.png");

		std::ifstream in( filepath, std::ios::binary );
		if (in.is_open())
		{
			//read Header File and lumps
			//
			in.read((char*)&ModelHeader, sizeof(Header));

			//verify Header file
			//
			// TODO

			//read Materials
			//
			readMaterialsFromMaxFile(ModelHeader.Lumps[kMaterials], materials, in );

			//read TriangleBatches
			//
			readTriangleBatchesFromMaxFile(ModelHeader.Lumps[kBatches], batches, materials, in );

			// read Animation Data
			//
			in.seekg(ModelHeader.Lumps[kAnimationData].Offset, std::ios::beg);
			in.read((char*)&m_AnimData, sizeof(AnimationData));

			//resize bone vector5
			//
			m_bones.resize( m_AnimData.numBones );


			// read Node Data
			//
			readModelNodesFromMaxFile(ModelHeader.Lumps[kNodes], nodes, m_topNodes, m_bones, batches, in);

			// read Camera Data
			//
			readCameraDataFromMaxFile(ModelHeader.Lumps[kCameraData], nodes, m_cameras, in);
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void Model::Unload()
	{
		size_t numTopNodes = m_topNodes.size();
		for(size_t i = 0; i < numTopNodes; ++i)
		{
			m_topNodes[i]->Unload();
		}

		m_topNodes.clear();
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void Model::ModelNode::Unload()
	{
		size_t numBatches = m_batches.size();
		for(size_t i = 0; i < numBatches; ++i)
		{
			m_batches[i]->Unload();
		}

		size_t numChildren = m_children.size();
		for(size_t i = 0; i < numChildren; ++i)
		{
			m_children[i]->Unload();
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	Material::~Material()
	{
		Texture::DestroyTexture(m_diffuseTexture);
		Texture::DestroyTexture(m_emissiveMap);
		Texture::DestroyTexture(m_normalMap);
		Texture::DestroyTexture(m_specularMap);
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	TriangleBatch::~TriangleBatch()
	{
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void TriangleBatch::Unload()
	{
		glDeleteBuffers(1, &idIBO);
		glDeleteBuffers(1, &idVBO);
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void Model::Render( Matrix4f& TransformIn, int CameraNumber, std::shared_ptr<Material> materialOveride )
	{
		unsigned int curFrame = m_curFrame % m_AnimData.largest_Animation_Frames;

		if(m_AnimData.numBones > 0)
		{

 			std::vector<float> boneTransforms;
 			boneTransforms.resize(m_AnimData.numBones * 16);


			for(size_t i = 0; i < m_AnimData.numBones; ++i)
			{	

				Matrix4f m = m_bones[i]->getWorldTM(curFrame);
				Matrix4f m2 = m_bones[i]->m_inverseRestMatrix;
				m = m2 * m;
				
				//Billboard::RenderBillboardAtPoint(m_defaultTexture, Vector3f(m[12], m[13], m[14]), Vector2f(.80f, .80f) );

				for( size_t n = 0; n < 16; ++n)
				{
					boneTransforms[i*16 + n] = m[n];
				}
			}
			glUseProgram(m_shaderInfoSkinned.program);
			glUniformMatrix4fv( m_shaderInfoSkinned.uniformBoneTms, m_AnimData.numBones, false, &boneTransforms[0] );
		}

		if(CameraNumber >= 0 && CameraNumber < (int)m_cameras.size() )
		{
			glRotatef(90, 1, 0, 0);
			glUniformMatrix4fv( m_shaderInfoSkinned.uniformViewMatrix, 1, false, m_cameras[CameraNumber]->getWorldTM(curFrame).Invert().m_Matrix);
		}

		
		size_t numTopNodes = m_topNodes.size();
		for(size_t i = 0; i < numTopNodes; ++i)
		{
			m_topNodes[i]->Render( curFrame, m_shaderInfoSkinned, TransformIn, materialOveride );
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void Model::Update(double delta)
	{
		m_curtime += delta;
		m_curFrame = (int)(m_curtime * m_AnimData.fps);
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	Matrix4f Model::ModelNode::getWorldTM( int frame )
	{
		size_t localFrame = frame;
		if(localFrame >= m_localTMs.size() )
			localFrame = m_localTMs.size() - 1;

		if( m_parent ==  nullptr)
			return m_localTMs[localFrame];
		
		return m_localTMs[localFrame] * m_parent->getWorldTM(frame);

	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void Model::ModelNode::Render( int curFrame, shaderInfo& shader, Matrix4f& TransformIn, std::shared_ptr<Material> materialOveride )
	{
		glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT );
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

		unsigned int numFrames = m_localTMs.size();
		Matrix4f TransformOut = m_localTMs[curFrame % numFrames] * TransformIn;

		glUniformMatrix4fv( shader.uniformMat4Model, 1, false, TransformOut.m_Matrix );

		size_t numBatches = m_batches.size();
		for( size_t i = 0; i < numBatches; ++i)
		{
			std::shared_ptr<TriangleBatch> batch = m_batches[i];

			glFrontFace( GL_CCW );

			glBindBuffer( GL_ARRAY_BUFFER, batch->idVBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, batch->idIBO );

			glDisable( GL_BLEND );

			std::shared_ptr<Material> mat;
			if( materialOveride != nullptr)
				mat = materialOveride;
			else
				mat = batch->m_material;

			if(mat != nullptr)
			{

				glActiveTexture( GL_TEXTURE0 );
				glEnable( GL_TEXTURE_2D);

				if( Texture::ApplyTexture( mat->m_diffuseTexture ) )
				{

					glUniform1i(shader.uniformDiffuse, 0);

					glEnableClientState( GL_TEXTURE_COORD_ARRAY );
					glTexCoordPointer( 3, GL_FLOAT, sizeof( Vertex ),
						reinterpret_cast< const GLvoid* >( offsetof( Vertex, uv )));
				}

				if(mat->m_normalMap != nullptr)
				{
					glActiveTexture( GL_TEXTURE1 );
					Texture::ApplyTexture( mat->m_normalMap );

					// Set the u_diffuse sampler uniform to refer to texture unit 0
					glUniform1i(shader.uniformNormalMap, 1);
					glUniform1i(shader.uniformHasNormalMap, 1);
				}
				else
				{
					glUniform1i(shader.uniformHasNormalMap, 0);
				}

				if(mat->m_emissiveMap != nullptr)
				{
					glActiveTexture( GL_TEXTURE2 );
					Texture::ApplyTexture( mat->m_emissiveMap );

					// Set the u_diffuse sampler uniform to refer to texture unit 0
					glUniform1i(shader.uniformEmissiveMap, 2);
					glUniform1i(shader.uniformHasEmissiveMap, 2);
				}
				else
				{
					glUniform1i(shader.uniformHasEmissiveMap, 0);
				}

				if(mat->m_specularMap != nullptr)
				{
					glActiveTexture( GL_TEXTURE3 );
					Texture::ApplyTexture( mat->m_specularMap );

					// Set the u_diffuse sampler uniform to refer to texture unit 0
					glUniform1i(shader.uniformSpecularMap, 3);
					glUniform1i(shader.uniformHasSpecularMap, 3);
				}
				else
				{
					glUniform1i(shader.uniformHasSpecularMap, 0);
				}

			}

			glEnableClientState( GL_VERTEX_ARRAY );
			glVertexPointer( 3, GL_FLOAT, sizeof( Vertex ), 
				reinterpret_cast< const GLvoid* >( offsetof( Vertex, vPosition )));

			glEnableClientState( GL_NORMAL_ARRAY );
			glNormalPointer( GL_FLOAT, sizeof( Vertex ),
				reinterpret_cast< const GLvoid* >( offsetof( Vertex, normal )));


			glVertexAttribPointer(shader.attributeTanId, 3, GL_FLOAT,GL_FALSE,
				sizeof(Vertex),	reinterpret_cast< void* >(offsetof(Vertex, tangent) ) );
			glEnableVertexAttribArray(shader.attributeTanId);

			glVertexAttribPointer(shader.attributeBiTanId, 3, GL_FLOAT,GL_FALSE,
				sizeof(Vertex),	reinterpret_cast< void* >(offsetof(Vertex, bitangent) ) );
			glEnableVertexAttribArray(shader.attributeBiTanId);

			glVertexAttribPointer(shader.attributeBoneID, 4, GL_FLOAT, GL_FALSE,
				sizeof(Vertex),	reinterpret_cast< void* >(offsetof(Vertex, bones) ) );
			glEnableVertexAttribArray(shader.attributeBoneID);

			glVertexAttribPointer(shader.attributeBoneWeight, 4, GL_FLOAT, GL_FALSE,
				sizeof(Vertex),	reinterpret_cast< void* >(offsetof(Vertex, weights) ) );
			glEnableVertexAttribArray(shader.attributeBoneWeight);



			glDrawElements( GL_TRIANGLES, batch->m_indicies.size(), GL_UNSIGNED_INT, 0 ); 


			glDisableVertexAttribArray(shader.attributeTanId);
			glDisableVertexAttribArray(shader.attributeBiTanId);
			glDisableVertexAttribArray(shader.attributeBoneID);
			glDisableVertexAttribArray(shader.attributeBoneWeight);

			glActiveTexture( GL_TEXTURE2 );
			glClientActiveTexture( GL_TEXTURE2 );
			glDisable( GL_TEXTURE_2D);
			glActiveTexture( GL_TEXTURE1 );
			glClientActiveTexture( GL_TEXTURE1 );
			glDisable( GL_TEXTURE_2D);
			glActiveTexture( GL_TEXTURE0 );
			glClientActiveTexture( GL_TEXTURE0 );
			glDisable( GL_TEXTURE_2D);

		}
		//Clean up
		//
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
		glPopAttrib();

		size_t numChildren =  m_children.size();
		for(size_t i = 0; i < numChildren; ++i)
		{
			m_children[i]->Render(curFrame, shader, TransformOut, materialOveride);
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------

	void Model::Init( shaderInfo SkinnedShader )
	{
		m_shaderInfoSkinned = SkinnedShader;
		m_curtime = 0.0;
		m_boneMatriciesToShader.resize( m_AnimData.numBones * 16 );
	}
}
