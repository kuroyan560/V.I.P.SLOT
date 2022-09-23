#pragma once
#include"Vec.h"
#include"Color.h"
#include<array>
#include<memory>
#include"D3D12Data.h"
class Camera;

class HitParticle
{
	//�u���b�N��
	static const int s_particleNum = 10000;
	static const int s_threadDiv = 32;

	//�u���b�N�̏��\����
	struct Particle
	{
		Color m_color;
		Vec3<float>m_vel = { 0,1,0 };
		float m_life = 0;
		Vec3<float>m_pos = { 0,0,0 };
		int m_lifeSpan = 60;
	};

	//�u���b�N�̊e�̏��iGPU�j
	std::shared_ptr<RWStructuredBuffer>m_particleBuff;

	//�O���t�B�b�N�X�p�C�v���C��
	std::shared_ptr<GraphicsPipeline>m_gPipeline;

	//Indirect�@�\
	std::shared_ptr<IndirectDevice>m_indirectDev;

	//���_�o�b�t�@
	std::shared_ptr<VertexBuffer>m_vertBuff;

	//�ݒ�
	struct Config
	{
		//�J�����̃o�b�t�@�A�h���X
		//D3D12_GPU_VIRTUAL_ADDRESS camAddress;
		//�p�[�e�B�N���ő吔
		unsigned int m_maxParticleNum = s_particleNum;
	}m_config;
	std::shared_ptr<ConstantBuffer>m_configBuffer;

	//�X�V�p�p�C�v���C��
	std::shared_ptr<ComputePipeline>m_cUpdatePipeline;


	//���S�p�[�e�B�N���̃R�}���h
	std::shared_ptr<IndirectCommandBuffer>m_commandBuffer;
	
	//�ғ����p�[�e�B�N���̃R�}���h
	std::shared_ptr<IndirectCommandBuffer>m_aliveCommandBuffer;

	bool m_invalidCommandBuffer = true;

public:
	HitParticle();
	void Init(Camera& Cam);
	void Update();
	void Draw(Camera& Cam);

	void Emit(int Num, Vec3<float>Pos);
};

