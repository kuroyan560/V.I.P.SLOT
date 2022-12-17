#include "WaveMgr.h"
#include"Sprite.h"
#include"D3D12App.h"
#include"KuroFunc.h"
#include"TimeScale.h"
#include"EnemyEmitter.h"

void WaveMgr::OnImguiItems()
{
	ImGui::Checkbox("InfinityMode", &m_isInfinity);
	ImGui::Text("NowWaveIdx : { %d / %d }", m_nowWaveIdx, static_cast<int>(m_waves.size()));
	ImGui::Text("Norma : { %d }", m_sumNorma);
}

WaveMgr::WaveMgr() : Debugger("WaveMgr")
{
	Vec2<float>NORMA_STR_POS = { 1080.0f,74.0f };
	float NORMA_STR_SCALE = 0.9f;

	auto app = D3D12App::Instance();

	/*--- ノルマ表示 ---*/
	std::string normaTexDir = "resource/user/img/ui/norma/";
	m_normaStrSprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(normaTexDir + "norma_str.png"), "Sprite - NormaStr");
	m_normaStrSprite->m_transform.SetPos(NORMA_STR_POS);
	m_normaStrSprite->m_transform.SetScale(NORMA_STR_SCALE);
	m_normaStrSprite->SendTransformBuff();

	//数字スプライトを指定した桁数分事前に用意
	static const int PREPARE_DIGIT_NUM = 5;
	m_normaNumSpriteArray.resize(PREPARE_DIGIT_NUM);
	for (int i = 0; i < static_cast<int>(m_normaNumSpriteArray.size()); ++i)
	{
		auto name = "Sprite - NormaNum - " + std::to_string(i);
		m_normaNumSpriteArray[i] = std::make_shared<Sprite>(nullptr, name.c_str());
	}

	int texNum = 11;
	m_normaTexArray.resize(texNum);
	app->GenerateTextureBuffer(m_normaTexArray.data(), normaTexDir + "norma_num.png", texNum, Vec2<int>(11, 1));
}

void WaveMgr::Init(std::list<Wave>arg_waves)
{
	//ウェーブ配列格納
	m_waves = arg_waves;
	//ウェーブインデックス初期化
	m_nowWaveIdx = 0;
	//全ウェーブクリアフラグ初期化
	m_isAllWaveClear = false;
	//通算ノルマ初期化
	m_sumNorma = 0;

	//ウェーブ進行時の初期化呼び出し
	WaveInit();
}

void WaveMgr::Update(const TimeScale& arg_timeScale, std::weak_ptr<EnemyEmitter>arg_enemyEmitter)
{
	//情報が無ければテスト用のランダム出現
	if (m_nowInfoArray.empty())
	{
		arg_enemyEmitter.lock()->TestRandEmit(arg_timeScale);
		return;
	}

	m_time += arg_timeScale.GetTimeScale();

	for (auto itr = m_nowInfoArray.begin(); itr != m_nowInfoArray.end();)
	{
		//出現タイミングでない
		if (m_time < itr->m_info->m_appearTiming)continue;

		//ループ出現でない
		if (!itr->m_info->m_loopAppear)
		{
			//出現
			arg_enemyEmitter.lock()->EmitEnemy(itr->m_info->m_type, itr->m_info->m_initPos);
			//出現済なので出現情報を配列から削除
			itr = m_nowInfoArray.erase(itr);
			continue;
		}

		//ループ出現
		if (itr->m_timer.UpdateTimer(arg_timeScale.GetTimeScale()))
		{
			//出現
			arg_enemyEmitter.lock()->EmitEnemy(itr->m_info->m_type, itr->m_info->m_initPos);
			//出現タイマーリセット
			itr->m_timer.Reset(itr->m_info->m_appearTiming);
		}
	}
}

void WaveMgr::ProceedWave()
{
	//全ウェーブクリアしたか
	if (static_cast<int>(m_waves.size()) <= m_nowWaveIdx + 1)
	{
		m_isAllWaveClear = true;
		return;
	}

	//ウェーブ進行
	m_nowWaveIdx++;

	//ウェーブ初期化
	WaveInit();
}

void WaveMgr::OnDraw2D()
{
	//ノルマ数字描画
	for (int i = 0; i < m_useSpriteNum; ++i)
	{
		m_normaNumSpriteArray[i]->Draw();
	}

	//ノルマ文字
	m_normaStrSprite->Draw();
}

void WaveMgr::WaveInit()
{
	//ウェーブ時間初期化
	m_time = 0.0f;

	//ウェーブのポインタ取得
	int offset = 0;
	auto itr = m_waves.begin();
	while (offset < m_nowWaveIdx)
	{
		itr++;
		offset++;
	}
	m_nowWave = &(*itr);

	//通算ノルマ更新
	m_sumNorma += m_nowWave->m_norma;

	/*--- 敵の出現情報配列用意 ---*/
	m_nowInfoArray.clear();
	for (const auto& info : m_nowWave->m_appearInfoList)
	{
		m_nowInfoArray.emplace_back();
		m_nowInfoArray.back().m_info = &info;
		m_nowInfoArray.back().m_timer.Reset(0.0f);
	}

	/*--- UI更新 ---*/

	//桁数を調べて、必要なスプライト数を計算
	int normaDigit = KuroFunc::GetDigit(m_sumNorma);
	//「 / 」分追加
	m_useSpriteNum = normaDigit + 1;

	//スプライト数が足りなければ必要数追加
	int spriteIdx = static_cast<int>(m_normaNumSpriteArray.size());
	while (static_cast<int>(m_normaNumSpriteArray.size()) < m_useSpriteNum)
	{
		auto name = "Sprite - NormaNum - " + std::to_string(++spriteIdx);
		m_normaNumSpriteArray.emplace_back(std::make_shared<Sprite>(nullptr, name.c_str()));
	}

	//最初のテクスチャは「 / 」
	m_normaNumSpriteArray[0]->SetTexture(m_normaTexArray[10]);
	//テクスチャのインデックス情報
	for (int i = 1; i < m_useSpriteNum; ++i)
	{
		int num = KuroFunc::GetSpecifiedDigitNum(m_sumNorma, m_useSpriteNum - i - 1, false);
		m_normaNumSpriteArray[i]->SetTexture(m_normaTexArray[num]);
	}

	//座標計算
	float offsetY = m_numPosOffset.y / static_cast<float>(m_useSpriteNum - 1);
	for (int i = 0; i < m_useSpriteNum; ++i)
	{
		Vec2<float>pos = m_numPos;
		pos.x -= m_useSpriteNum * m_numPosOffset.x;	//右揃え
		pos.x += m_numPosOffset.x * static_cast<float>(i);
		pos.y -= offsetY * static_cast<float>(m_useSpriteNum - i - 1);
		m_normaNumSpriteArray[i]->m_transform.SetPos(pos);
		m_normaNumSpriteArray[i]->m_transform.SetScale(m_numScale);
		m_normaNumSpriteArray[i]->SendTransformBuff();
	}
}
