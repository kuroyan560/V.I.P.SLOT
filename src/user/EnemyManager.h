#pragma once
#include<vector>
#include<array>
#include<memory>
#include"Singleton.h"
#include"EnemyBreed.h"
#include"Transform.h"
#include"Collider.h"
class Enemy;
class GraphicsPipeline;
class StructuredBuffer;
class Camera;
class CubeMap;
class Player;

//�G�l�~�[���
const enum ENEMY_TYPE { SANDBAG, ENEMY_TYPE_NUM };

class EnemyManager : public Singleton<EnemyManager>
{
private:
	friend class Singleton<EnemyManager>;
	EnemyManager();
	typedef std::vector<std::shared_ptr<Enemy>> EnemyArray;

private:
	//�e��ʂ��Ƃɐ����ł���ő吔
	static const int s_maxNum = 100;
	//�e���f���̃{�[���ő吔
	static const int s_maxBoneNum = 32;
	//�`��p�C�v���C���i�C���X�^���V���O�`��j
	std::shared_ptr<GraphicsPipeline>m_pipeline;

	//�n���i�^�I�u�W�F�N�g�j
	std::array<std::unique_ptr<EnemyBreed>, ENEMY_TYPE_NUM>m_breeds;
	//�����σG�l�~�[�z��
	std::array<EnemyArray, ENEMY_TYPE_NUM>m_enemys;
	//���[���h�s��z��p�\�����o�b�t�@
	std::array<std::shared_ptr<StructuredBuffer>, ENEMY_TYPE_NUM>m_worldMatriciesBuff;
	//�{�[���s��z��p�\�����o�b�t�@
	std::array<std::shared_ptr<StructuredBuffer>, ENEMY_TYPE_NUM>m_boneMatriciesBuff;

public:
	//�����σG�l�~�[�N���A
	void Clear();
	//������
	void Init();
	//�X�V
	void Update(const Player& Player, const float& Gravity);
	//�`��
	void Draw(Camera& Cam, std::shared_ptr<CubeMap>AttachCubeMap);

	//����
	void Spawn(const ENEMY_TYPE& Type, const Transform& InitTransform);

	//���f���擾
	std::shared_ptr<Model>GetModel(const ENEMY_TYPE& Type)
	{
		return m_breeds[Type]->GetModel();
	}
};