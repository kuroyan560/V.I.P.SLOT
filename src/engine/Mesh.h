#pragma once
#include<vector>
#include<memory>
#include"D3D12App.h"
#include"KuroFunc.h"

//���_�ƃC���f�b�N�X�������������b�V���e���v���[�g�N���X�i�f�[�^�R���e�i�j
template<typename VertexType>
class Mesh
{
	bool invalid = true;
public:
	//���b�V����
	std::string name;

	//���_�z��i�e���v���[�g)
	std::vector<VertexType>vertices;
	//���_�o�b�t�@
	std::shared_ptr<VertexBuffer>vertBuff;

	//�C���f�b�N�X�z��
	std::vector<unsigned int>indices;
	//�C���f�b�N�X�o�b�t�@
	std::shared_ptr<IndexBuffer>idxBuff;

	//�o�b�t�@����
	void CreateBuff()
	{
		assert(!vertices.empty());

		//���_�o�b�t�@����
		if(!vertBuff)vertBuff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(VertexType), static_cast<int>(vertices.size()), &vertices[0], name.c_str());

		//�C���f�b�N�X�o�b�t�@����
		if (!idxBuff && !indices.empty())idxBuff = D3D12App::Instance()->GenerateIndexBuffer(static_cast<int>(indices.size()), &indices[0], name.c_str());

		invalid = false;
	}
	//�}�b�s���O
	void Mapping()
	{
		assert(!invalid);
		vertBuff->Mapping(&vertices[0]);
		if (idxBuff)idxBuff->Mapping(&indices[0]);
	}
	//�o�b�t�@���Z�b�^
	void SetNameToBuff(const std::string& Name)
	{
		assert(!invalid);
		auto wideName = KuroFunc::GetWideStrFromStr(name).c_str();
		vertBuff->SetName(wideName);
		if (idxBuff)idxBuff->SetName(wideName);
	}
};

