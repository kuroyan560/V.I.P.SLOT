#pragma once
#include<vector>
#include<memory>
#include"Vec.h"
#include"Debugger.h"
#include"HUDInterface.h"
#include"Wave.h"
class Sprite;
class TextureBuffer;
class TimeScale;
class EnemyEmitter;

class WaveMgr : public Debugger, public HUDInterface
{
	//�E�F�[�u���X�g
	std::list<Wave>m_waves;
	//���݂̃E�F�[�u
	Wave* m_nowWave = nullptr;
	//���݂̃E�F�[�u�C���f�b�N�X
	int m_nowWaveIdx;
	//���݂̃E�F�[�u�̐i�s����
	float m_time;
	//�ʎZ�m���}
	int m_sumNorma;

	struct NowAppearInfo
	{
		const Wave::AppearEnemyInfo* m_info = nullptr;
		Timer m_timer;
	};
	std::vector<NowAppearInfo>m_nowInfoArray;

	//�S�ẴE�F�[�u���N���A������
	bool m_isAllWaveClear = false;

	//�f�o�b�O�p�A�m���}�B�������؂�
	bool m_isInfinity = false;

	/*--- �m���}�\�� ---*/
	Vec2<float>m_numPos = { 1271.0f,10.0f };
	Vec2<float>m_numPosOffset = { 47.0f, -17.0f };
	float m_numScale = 0.85f;
	std::shared_ptr<Sprite>m_normaStrSprite;
	std::vector<std::shared_ptr<Sprite>>m_normaNumSpriteArray;
	std::vector<std::shared_ptr<TextureBuffer>>m_normaTexArray;
	int m_useSpriteNum = 0;

	void OnImguiItems()override;
	void OnDraw2D()override;

	//�E�F�[�u���Ƃ̏���������
	void WaveInit();

public:
	WaveMgr();
	void Init(std::list<Wave>arg_waves);
	void Update(const TimeScale& arg_timeScale, std::weak_ptr<EnemyEmitter>arg_enemyEmitter);

	//�E�F�[�u�̐i�s
	void ProceedWave();

	//���݂̃E�F�[�u���N���A������
	bool IsWaveClear(const int& arg_playersCoinNum)const
	{
		return !m_isInfinity && m_sumNorma <= arg_playersCoinNum;
	}
	//�S�ẴE�F�[�u���N���A������
	bool IsAllWaveClear()const
	{
		return m_isAllWaveClear;
	}

	//�񕜃L�b�g�̕��o��
	int GetHealKitNum()const
	{
		if (m_isAllWaveClear)return 0;
		return m_nowWave->m_healKitNum;
	}
};