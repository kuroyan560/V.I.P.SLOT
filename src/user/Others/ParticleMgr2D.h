#pragma once
#include"Singleton.h"
#include<memory>
#include"Vec.h"
#include"Color.h"
#include<vector>
#include<string>
#include"KuroMath.h"
#include<map>
#include"D3D12Data.h"

class VertexBuffer;
class ComputePipeline;
class GraphicsPipeline;
class ConstantBuffer;
class TextureBuffer;

struct ParticleInitializer
{
	Color m_mulColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vec2<float>m_pos = { 0.0f,0.0f };
	Vec2<float>m_vec = { 0.0f,0.0f };
	float m_speed = 0.0f;
	float m_scale = 64.0f;
	float m_radian = 0.0f;
	float m_lifeSpan = 60.0f;
	unsigned int m_texIdx = 0;
};

class ParticleMgr2D : public Singleton<ParticleMgr2D>
{
	friend class Singleton<ParticleMgr2D>;
	ParticleMgr2D();

	//�X���b�h��
	static const int THREAD_PER_NUM = 10;
	//�Z�b�g�ł���e�N�X�`���̍ő吔
	static const int TEX_NUM_MAX = 10;

	struct Particle
	{
		ParticleInitializer m_initializer;	//���������
		Color m_mulColor;	//��Z�F
		Vec2<float>m_pos;	//���W
		float m_speed;	//�X�s�[�h
		float m_scale;	//�X�P�[��
		float m_radian;	//��]�p�x
		float m_lifeTimer = 0;	//��������Ă���̃^�C�}�[
		unsigned int m_isAlive = 0;		// �����t���O
		unsigned int m_texIdx;	//�e�N�X�`���C���f�b�N�X

		// ��������
		void Generate(const ParticleInitializer& arg_initializer)
		{
			m_initializer = arg_initializer;
			m_mulColor = arg_initializer.m_mulColor;
			m_pos = arg_initializer.m_pos;
			m_speed = arg_initializer.m_speed;
			m_scale = arg_initializer.m_scale;
			m_radian = arg_initializer.m_radian;
			m_lifeTimer = 0.0f;
			m_isAlive = 1;
			m_texIdx = arg_initializer.m_texIdx;
		}
	};

	struct DefineParticleInfo
	{
		//�p�[�e�B�N���C���X�^���X
		std::vector<Particle>m_particles;
		//�p�[�e�B�N���̏��i�[�o�b�t�@�A�`��ɗ��p
		std::shared_ptr<VertexBuffer>m_vertBuff;
		//�p�[�e�B�N���̍X�V�p�C�v���C��
		std::shared_ptr<ComputePipeline>m_cPipeline;
		//�e�N�X�`���o�b�t�@�z��
		std::vector<std::shared_ptr<TextureBuffer>>m_textures;
		//�f�t�H���g�̏��������
		ParticleInitializer* m_defaultInitializer = nullptr;
		//���ɕ��o����p�[�e�B�N���̃C���f�b�N�X
		int m_nextEmitParticleIdx = 0;
		//�`�掞�̃u�����h���[�h
		AlphaBlendMode m_blendMode;

		~DefineParticleInfo()
		{
			if (m_defaultInitializer != nullptr)
			{
				delete m_defaultInitializer;
				m_defaultInitializer = nullptr;
			}
		}
	};
	std::vector<DefineParticleInfo>m_particleInfoArray;

	//�S�p�[�e�B�N�����ʂ̒萔�o�b�t�@
	struct CommonConstData
	{
		float m_timeScale = 1.0f;
	}m_commonConstData;
	std::shared_ptr<ConstantBuffer>m_commonConstBuff;

	//�S�p�[�e�B�N���̍X�V���s���R���s���[�g�p�C�v���C��
	std::shared_ptr<ComputePipeline>m_commonComputePipeline;

	//�`��p�O���t�B�b�N�p�C�v���C��
	std::map<AlphaBlendMode, std::shared_ptr<GraphicsPipeline>>m_graphicsPipeline;

public:
	void Init();
	void Update(float arg_timeScale);
	void Draw();

	/// <summary>
	/// �p�[�e�B�N������
	/// </summary>
	/// <param name="arg_maxInstanceNum">�ő吔</param>
	/// <param name="arg_computeShaderPath">�p�[�e�B�N���̍X�V���s���R���s���[�g�V�F�[�_�[�̃t�@�C���p�X</param>
	/// <param name="arg_texArray">�e�N�X�`���z��擪�|�C���^</param>
	/// <param name="arg_texArraySize">�e�N�X�`���z��T�C�Y</param>
	/// <param name="arg_defaultInitializer">�f�t�H���g�̏��������</param>
	/// <param name="arg_blendMode">�`�掞�̃u�����h���[�h</param>
	/// <returns>�p�[�e�B�N��ID�F�p�[�e�B�N�����̃C���f�b�N�X(Emit���ɕK�v)</returns>
	int Prepare(int arg_maxInstanceNum,
		std::string arg_computeShaderPath,
		std::shared_ptr<TextureBuffer>* arg_texArray,
		size_t arg_texArraySize,
		ParticleInitializer* arg_defaultInitializer = nullptr,
		AlphaBlendMode arg_blendMode = AlphaBlendMode_Trans);

	/// <summary>
	/// �p�[�e�B�N������
	/// </summary>
	/// <param name="arg_particleID">�p�[�e�B�N��ID</param>
	/// <param name="arg_initializer">���������</param>
	/// <param name="arg_texIdx">�Q�Ƃ���e�N�X�`���̃C���f�b�N�X</param>
	void Emit(int arg_particleID, ParticleInitializer arg_initializer);

	/// <summary>
	/// �ݒ肳��Ă���f�t�H���g�̏��������̃Q�b�^
	/// </summary>
	/// <param name="arg_particleIdx">�p�[�e�B�N��ID</param>
	/// <returns>�ݒ肳��Ă���f�t�H���g�̏��������</returns>
	const ParticleInitializer& GetDefaultInitializer(int arg_particleIdx)const
	{
		assert(m_particleInfoArray[arg_particleIdx].m_defaultInitializer != nullptr);
		return *m_particleInfoArray[arg_particleIdx].m_defaultInitializer;
	}

	/// <summary>
	/// �w�肵����ʂ̃p�[�e�B�N�������ׂď���
	/// </summary>
	/// <param name="arg_particleID">�p�[�e�B�N��ID</param>
	void EraseParticles(int arg_particleID);

	/// <summary>
	/// �S�Ẵp�[�e�B�N��������
	/// </summary>
	void EraseAllParticles()
	{
		for (int i = 0; i < static_cast<int>(m_particleInfoArray.size()); ++i)EraseParticles(i);
	}
};