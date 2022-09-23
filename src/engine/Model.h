#pragma once
#include<string>
#include<vector>
#include"ImportHeader.h"
#include"Skeleton.h"
#include"ModelMesh.h"
#include"Collision.h"

class Model
{
public:
	//�w�b�_�i���f�����j
	ImportHeader m_header;
	//���b�V��
	std::vector<ModelMesh>m_meshes;
	//�X�P���g���i�{�[���\���j
	std::shared_ptr<Skeleton> m_skelton;

	Model(const std::string& Dir, const std::string& FileName) :m_header(Dir, FileName) {}

	//�S���b�V�����X���[�W���O
	void AllMeshSmoothing()
	{
		for (auto& m : m_meshes)
		{
			m.Smoothing();
		}
	}

	//�S���b�V���ɂ�����e���̒��_���W�̍ŏ��l�ő�l���擾
	Vec3<ValueMinMax>GetAllMeshPosMinMax();

	void AllMeshBuildTangentAndBiNormal()
	{
		for (auto& m : m_meshes)
		{
			m.BuildTangentAndBiNormal();
		}
	}
};

