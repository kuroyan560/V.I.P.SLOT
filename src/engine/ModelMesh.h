#pragma once
#include"Vec.h"
#include<memory>
#include"Mesh.h"
#include"Material.h"
#include<vector>
#include"Collision.h"

class ModelMesh
{
public:
	//���f����p���_�N���X
	class Vertex
	{
	public:
		static std::vector<InputLayoutParam>GetInputLayout()
		{
			static std::vector<InputLayoutParam>INPUT_LAYOUT =
			{
				InputLayoutParam("POSITION",DXGI_FORMAT_R32G32B32_FLOAT),
				InputLayoutParam("NORMAL",DXGI_FORMAT_R32G32B32_FLOAT),
				InputLayoutParam("TANGENT",DXGI_FORMAT_R32G32B32_FLOAT),	//�ڃx�N�g��
				InputLayoutParam("BINORMAL",DXGI_FORMAT_R32G32B32_FLOAT),	//�]�x�N�g��
				InputLayoutParam("TEXCOORD",DXGI_FORMAT_R32G32_FLOAT),
				InputLayoutParam("BONE_NO",DXGI_FORMAT_R32G32B32A32_SINT),
				InputLayoutParam("WEIGHT",DXGI_FORMAT_R32G32B32A32_FLOAT),
			};
			return INPUT_LAYOUT;
		}

		Vec3<float>pos;
		Vec3<float>normal = { 0,0,0 };
		Vec3<float>tangent = { 0,0,0 };
		Vec3<float>binormal = { 0,0,0 };
		Vec2<float>uv;
		Vec4<int>boneIdx = { -1,-1,-1,-1 };
		Vec4<float>boneWeight = { 0,0,0,0 };
	};

	std::shared_ptr<Mesh<ModelMesh::Vertex>>mesh;
	std::shared_ptr<Material> material;

	//�X���[�W���O
	void Smoothing();

	//�ڃx�N�g���Ə]�x�N�g���v�Z
	void BuildTangentAndBiNormal();

	//�����蔻��p�O�p���b�V���z��擾
	std::vector<CollisionTriangle> GetCollisionTriangles();

	//�e���̒��_���W�̍ŏ��l�ƍő�l�擾
	Vec3<ValueMinMax>GetPosMinMax()const;
};

