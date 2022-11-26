#pragma once
#include<memory>
#include<array>
#include"Vec.h"
#include"Timer.h"
#include<vector>
#include"Transform.h"
#include<forward_list>
class Model;
class LightManager;
class Camera;

//�G��|���Ă��炦��R�C���̃G�t�F�N�g
class EnemyKillCoinEffect
{
	//�{���f���̃C���f�b�N�X
	const int PLUS_IDX = 10;

	//���f���̐�
	static const int MODEL_NUM = 11;
	//�����{���f��
	std::array<std::shared_ptr<Model>, MODEL_NUM>m_numModels;

	struct Info
	{
		//�����J�n���̍��W�L�^�p
		Vec3<float>m_startPos;

		//Y����]��
		Angle m_angleY = Angle(-360);
		//�����J�n����Y����]�ʋL�^�p
		Angle m_startAngleY = m_angleY;

		//�R�C����
		int m_coinNum;

		//�G�t�F�N�g�̐e�g�����X�t�H�[��
		Transform m_transform;

		//�������
		struct Number
		{
			//�������f���C���f�b�N�X
			int m_numIdx;
			//���W�I�t�Z�b�g�g�����X�t�H�[��
			Transform m_transform;
		};
		std::vector<Number>m_numbers;

		//�X�e�[�^�X
		enum STATUS { APPEAR, WAIT, DISAPPEAR, NUM }m_status = APPEAR;
		//�^�C�}�[
		std::array<Timer, STATUS::NUM>m_timer;

	public:
		Info(const EnemyKillCoinEffect& arg_parent, Vec3<float>arg_pos, int arg_coinNum);
		void Update(float arg_timeScale);
		void Draw(const EnemyKillCoinEffect& arg_parent, std::weak_ptr<LightManager>&arg_lightMgr, std::weak_ptr<Camera>&arg_cam);

		bool IsEnd() { return m_status == NUM; }
	};
	std::forward_list<Info>m_infoArray;

public:
	EnemyKillCoinEffect();
	void Init();
	void Update(float arg_timeScale);
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);

	//�G�t�F�N�g�o��
	void Emit(Vec3<float>arg_playerPos, int arg_coinNum);
};

