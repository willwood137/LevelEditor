
#include "..\Math\Vector3.hpp"
#include "..\Math\Matrix4.hpp"
#include <vector>
#include <array>
#include <string>
#include "Texture.hpp"


namespace shadow
{
	struct shaderInfo
	{
		unsigned int program;

		unsigned int attributePosition;
		unsigned int attributeTanId;
		unsigned int attributeBiTanId;
		unsigned int attributeBoneWeight;
		unsigned int attributeBoneID;

		unsigned int uniformDebugFloat;
		unsigned int uniformDebugInt;

		unsigned int uniformBoneTms;
		unsigned int uniformNormalMap;
		unsigned int uniformDiffuse;
		unsigned int uniformMat4Model;
		unsigned int uniformViewMatrix;
		unsigned int uniformCameraPosition;
		unsigned int uniformTime;
		unsigned int uniformMat4Spin;
		unsigned int uniformLight1;
		unsigned int uniformHasNormalMap;
		unsigned int uniformHasEmissiveMap;
		unsigned int uniformEmissiveMap;
		unsigned int uniformHasSpecularMap;
		unsigned int uniformSpecularMap;
	};

	struct Vertex
	{
		float vPosition[3];
		float uv[2];
		float normal[3];
		float tangent[3];
		float bitangent[3];
		float weights[4];
		float bones[4];

		bool operator<( const Vertex& o ) const 
		{
			return memcmp(this,&o,sizeof(Vertex)) < 0;
		}
	};

	struct Lump
	{
		int Offset;
		int Length;
	};

	enum eLumps
	{
		kMaterials = 0,
		kBatches,
		kNodes,
		kAnimationData,
		kCameraData,
		kMaxLumps          // A constant to store the number of lumps

	};

	struct AnimationData
	{
		unsigned int startTime;
		unsigned int endtime;
		unsigned int fps;
		unsigned int numBones;
		unsigned int largest_Animation_Frames;
	};

	struct Header
	{
		char Magic[4];
		float version;
		Lump Lumps[kMaxLumps];
	};

	struct Material
	{
		std::string m_name;
		std::shared_ptr<Texture> m_diffuseTexture;
		std::shared_ptr<Texture> m_normalMap;
		std::shared_ptr<Texture> m_emissiveMap;
		std::shared_ptr<Texture> m_specularMap;

		~Material();
	};

	struct TriangleBatch
	{
		std::shared_ptr<Material> m_material;

		unsigned int numIndices;
		unsigned int numVertices;
		std::vector<Vertex> m_verts;
		std::vector<unsigned int> m_indicies;
		unsigned int idVBO;
		unsigned int idIBO;

		~TriangleBatch();
		void Unload();

	};

	struct MaxModelNodeInfo
	{
		int parent;
		std::vector<unsigned int> childrenIDs;
	};
	

	class Model
	{
	public:
		
		class ModelNode
		{
		public:
			void AddChildNode(std::shared_ptr<ModelNode> node);
			void Render( int curFrame, shaderInfo& shader, Matrix4f& TransformIn, std::shared_ptr<Material> materialOveride );
			Matrix4f getWorldTM( int frame );
			void Unload();

			std::string m_name;
			Matrix4f m_inverseRestMatrix;
			std::shared_ptr<ModelNode> m_parent;
			std::vector< std::shared_ptr<TriangleBatch> > m_batches;
			std::vector< std::shared_ptr<ModelNode> > m_children;
			std::vector<Matrix4f> m_localTMs;
		};

		void ReadDataFromMaxFile( const char* filepath );
		void Render( Matrix4f& TransformIn, int CameraNumber, std::shared_ptr<Material> materialOveride );
		void Init( shaderInfo Shader );
		void Unload();
		void Update( double delta);
		int getNumCameras() {return m_cameras.size();}

	private:

		std::vector<std::shared_ptr<ModelNode> > m_topNodes;
		std::vector<std::shared_ptr<ModelNode> > m_bones;
		std::vector<std::shared_ptr<ModelNode> > m_cameras;

		AnimationData m_AnimData;
		int m_curFrame;
		int m_totalFrames;
		double m_curtime;

		shaderInfo m_shaderInfoSkinned;

		std::vector<float> m_boneMatriciesToShader;
		std::shared_ptr<Texture> m_defaultTexture;
	};
}