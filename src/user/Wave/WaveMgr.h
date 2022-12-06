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
	//ウェーブリスト
	std::list<Wave>m_waves;
	//現在のウェーブ
	Wave* m_nowWave = nullptr;
	//現在のウェーブインデックス
	int m_nowWaveIdx;
	//現在のウェーブの進行時間
	float m_time;

	struct NowAppearInfo
	{
		const Wave::AppearEnemyInfo* m_info = nullptr;
		Timer m_timer;
	};
	std::vector<NowAppearInfo>m_nowInfoArray;

	//全てのウェーブをクリアしたか
	bool m_isAllWaveClear = false;

	//デバッグ用、ノルマ達成判定を切る
	bool m_isInfinity = false;

	/*--- ノルマ表示 ---*/
	Vec2<float>m_numPos = { 1271.0f,10.0f };
	Vec2<float>m_numPosOffset = { 47.0f, -17.0f };
	float m_numScale = 0.85f;
	std::shared_ptr<Sprite>m_normaStrSprite;
	std::vector<std::shared_ptr<Sprite>>m_normaNumSpriteArray;
	std::vector<std::shared_ptr<TextureBuffer>>m_normaTexArray;
	int m_useSpriteNum = 0;

	void OnImguiItems()override;
	void OnDraw2D()override;

	//ウェーブ進行時に呼び出し
	void OnProceedWave();

public:
	WaveMgr();
	void Init(std::list<Wave>arg_waves);
	void Update(const TimeScale& arg_timeScale, std::weak_ptr<EnemyEmitter>arg_enemyEmitter);

	//現在のウェーブをクリアしたか
	bool IsWaveClear(const int& arg_playersCoinNum)const
	{
		return !m_isInfinity && m_nowWave->m_norma <= arg_playersCoinNum;
	}
	//全てのウェーブをクリアしたか
	bool IsAllWaveClear()const
	{
		return m_isAllWaveClear;
	}
};