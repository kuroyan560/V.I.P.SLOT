#pragma once
#include"Singleton.h"
#include<string>
#include<fbxsdk.h>
#include<vector>
#include<memory>
#include<map>
#include<forward_list>
#include"Animation.h"
#include"ModelMesh.h"
#include"Skeleton.h"
class Model;

#include<GLTFSDK/GLTF.h>
#include"StreamReader.h"

class Importer : public Singleton<Importer>
{
	friend class Singleton<Importer>;

	//���f����p���_
	using Vertex = ModelMesh::Vertex;

	//�G���[���b�Z�[�W�\��
	void ErrorMessage(const std::string& FuncName, const bool& Fail, const std::string& Comment);
	//HSM���[�h�ŗp����f�[�^�̓ǂݎ��֐��i���������true��Ԃ�)
	bool LoadData(FILE* Fp, void* Data, const size_t& Size, const int& ElementNum);
	//HSM�Z�[�u�ŗp����f�[�^�̏������݊֐��i���������true��Ԃ�)
	bool SaveData(FILE* Fp, const void* Data, const size_t& Size, const int& ElementNum);


#pragma region FBX�֘A
	struct LoadFbxNode
	{
		std::string m_name;
		DirectX::XMVECTOR m_scaling = { 1,1,1,0 };
		DirectX::XMVECTOR m_rotation = { 0,0,0,0 };
		DirectX::XMVECTOR m_translation = { 0,0,0,1 };
		Matrix m_transform;
		FbxNode* m_fbxNode = nullptr;
		LoadFbxNode* m_parent = nullptr;
		FbxNodeAttribute* m_attribute = nullptr;
	};
	struct FbxBoneAffect	//�{�[���ƒ��_�̊֘A���L�^���邽�߂̃N���X
	{
		signed short m_index;
		float m_weight;
	};
	//�{�[�������_�ɗ^����e���Ɋւ�����e�[�u��
	//< ���_�C���f�b�N�X�A���(�ϒ��A�S�܂�)>
	//�T�ȏ�ǂݍ��܂�Ă����ꍇ�A���_�ǂݍ��ݎ��ɖ��������
	using BoneTable = std::map<int, std::forward_list<FbxBoneAffect>>;

	FbxManager* m_fbxManager = nullptr;
	FbxIOSettings* m_ioSettings = nullptr;
	FbxImporter* m_fbxImporter = nullptr;

	//FBX�֘A�f�o�C�X�̍폜����
	void FbxDeviceDestroy();
	//Fbx�m�[�h���ċA�I�T��
	void ParseNodeRecursive(std::vector<LoadFbxNode>& LoadFbxNodeArray, FbxNode* FbxNode, LoadFbxNode* Parent = nullptr);
	//FbxMesh����ǂݍ���
	void LoadFbxMesh(const std::string& Dir, const Skeleton& Skel, ModelMesh& ModelMesh, FbxMesh* FbxMesh);
	//�{�[�������_�ɗ^����e���Ɋւ��ď��擾
	void LoadBoneAffectTable(const Skeleton& Skel, FbxMesh* FbxMesh, BoneTable& BoneTable);
	//���_���ǂݍ���
	void LoadFbxVertex(ModelMesh& ModelMesh, FbxMesh* FbxMesh, BoneTable& BoneTable);
	std::string GetFileName(std::string FbxRelativeFileName);
	void LoadFbxMaterial(const std::string& Dir, ModelMesh& ModelMesh, FbxMesh* FbxMesh);
	//�A�j���[�V�������C���[�̒ǐ�
	//void LoadAnimCurve(FbxAnimCurve* FbxAnimCurve, Animation& Animation);
	void LoadBoneAnim(const LoadFbxNode& BoneNode, Skeleton::ModelAnimation& ModelAnimation, FbxAnimLayer* FbxAnimLayer);

#pragma endregion

#pragma region glTF�֘A
	void LoadGLTFPrimitive(ModelMesh& ModelMesh, const Microsoft::glTF::MeshPrimitive& GLTFPrimitive, const Microsoft::glTF::Skin* GLTFSkin, const Microsoft::glTF::GLTFResourceReader& Reader, const Microsoft::glTF::Document& Doc);
	void PrintDocumentInfo(const Microsoft::glTF::Document& document);
	void PrintResourceInfo(const Microsoft::glTF::Document& document, const Microsoft::glTF::GLTFResourceReader& resourceReader);
	void LoadGLTFMaterial(const MATERIAL_TEX_TYPE& Type, std::weak_ptr<Material>AttachMaterial, const Microsoft::glTF::Image& Img, const std::string& Dir, const Microsoft::glTF::GLTFResourceReader& Reader, const Microsoft::glTF::Document& Doc);
#pragma endregion

	//�C���|�[�g�������f��
	std::map<std::string, std::shared_ptr<Model>>m_models;
	void RegisterImportModel(const std::string& Dir, const std::string& FileName, const std::shared_ptr<Model>& Model)
	{
		m_models[Dir + FileName] = Model;
	}
	std::shared_ptr<Model> CheckAlreadyExsit(const std::string& Dir, const std::string& FileName);

	//HSM�̓G���W����p�̑��탂�f���Ƃ̒�����i���̃t�H�[�}�b�g�ǂݍ��݂͎��ԃR�X�g�������H�j
	/*
	std::shared_ptr<Model>LoadHSMModel(const std::string& Dir, const std::string& FileName, const std::string& HSMPath);
	void SaveHSMModel(const std::string& FileNameTail, std::shared_ptr<Model>Model, const FILETIME& LastWriteTime);
	*/

	//HSM�����݂���ꍇ��HSM����ǂݍ��߂�悤�ɂ��邩(HSM�̃t�H�[�}�b�g���ς�����Ƃ��̓t���O���I�t�ɂ���)
	const bool m_canLoadHSM = true;

	Importer();

	//�� �t�@�C�����͊g���q��
	std::shared_ptr<Model> LoadFBXModel(const std::string& Dir, const std::string& FileName, const std::string& Ext);
	std::shared_ptr<Model> LoadGLTFModel(const std::string& Dir, const std::string& FileName, const std::string& Ext);
public:
	~Importer() { FbxDeviceDestroy(); }

	std::shared_ptr<Model>LoadModel(const std::string& Dir, const std::string& FileName);
};

