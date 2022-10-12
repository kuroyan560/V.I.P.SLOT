#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"Importer.h"
#include"GameCamera.h"
#include"TimeScale.h"
#include"Player.h"

bool SlotMachine::CheckReelPattern()
{
	const auto& pattern = m_reels[REEL::LEFT].GetNowPattern();

	//絵柄が違えば何もせず終了
	if (pattern != m_reels[REEL::CENTER].GetNowPattern())return false;
	if (pattern != m_reels[REEL::RIGHT].GetNowPattern())return false;

	m_patternMgr.Invoke(pattern);
	return true;
}

void SlotMachine::SlotPerform(const ConstParameter::Slot::PATTERN& arg_pattern)
{
	using namespace ConstParameter::Slot;
	using PATTERN = ConstParameter::Slot::PATTERN;
}

SlotMachine::SlotMachine()
{
	//スロットマシン生成
	m_slotMachineObj = std::make_shared<ModelObject>("resource/user/model/", "slotMachine.glb");

	//モデルからリールの情報取得
	for (auto& mesh : m_slotMachineObj->m_model->m_meshes)
	{
		//マテリアル名取得
		const auto& materialName = mesh.material->name;

		for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)
		{
			//リールのメッシュ発見
			if (REEL_MATERIAL_NAME[reelIdx].compare(materialName) == 0)
			{
				//リールのメッシュポインタをアタッチ
				m_reels[reelIdx].Attach(&mesh);
			}
		}
	}

	//コイン投入口メガホン生成
	m_megaPhoneObj = std::make_shared<ModelObject>("resource/user/model/", "megaPhone.glb");
	//スロットマシンのトランスフォームを親として登録
	m_megaPhoneObj->m_transform.SetParent(&m_slotMachineObj->m_transform);
	//メガホン位置
	m_megaPhoneObj->m_transform.SetPos(ConstParameter::Slot::MEGA_PHONE_POS);
	//トランスフォームバッファ更新のため呼び出し
	m_megaPhoneObj->GetTransformBuff();

	//サウンド読み込み
	m_spinStartSE = AudioApp::Instance()->LoadAudio("resource/user/sound/slot_start.wav");
	m_reelStopSE = AudioApp::Instance()->LoadAudio("resource/user/sound/slot_stop.wav");
}

void SlotMachine::Init()
{
	//デバッグ用
	using PATTERN = ConstParameter::Slot::PATTERN;
	std::vector<PATTERN>testPatternArray;
	testPatternArray.resize(20);
	for (int i = 0; i < 20; ++i)
	{
		testPatternArray[i] = PATTERN::NONE;
	}

	//リール初期化
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Init(nullptr, testPatternArray);

	//レバー初期化
	m_lever = -1;

	//最後のリールを止めてからの時間計測用タイマーリセット
	m_slotWaitTimer.Reset(ConstParameter::Slot::SLOT_WAIT_TIME);
}

//デバッグ用
#include"UsersInput.h"

void SlotMachine::Update(std::weak_ptr<Player>arg_player, const TimeScale& arg_timeScale)
{
	//リール更新
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Update(arg_timeScale.GetTimeScale());

	//全リール停止済
	if (REEL::RIGHT < m_lever)
	{
		//次のスロット回転可能に
		if (m_slotWaitTimer.UpdateTimer(arg_timeScale.GetTimeScale()))
		{
			m_lever = -1;
			m_slotWaitTimer.Reset();
		}
	}
}

#include"DrawFunc3D.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<GameCamera>arg_gameCam)
{
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_megaPhoneObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);
}

void SlotMachine::Lever()
{
	//スロット回転開始
	if (m_lever == -1)
	{
		for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Start();
		AudioApp::Instance()->PlayWaveDelay(m_spinStartSE);
		m_lever++;
	}
	//各リール停止
	else if (m_lever <= REEL::RIGHT && m_reels[m_lever].IsCanStop())
	{
		m_reels[m_lever].Stop();
		AudioApp::Instance()->PlayWaveDelay(m_reelStopSE);

		//絵柄を確認して全て一緒なら効果発動
		if (m_lever++ == REEL::RIGHT && CheckReelPattern())
		{
			//効果を確認して演出の処理
			SlotPerform(m_reels[REEL::LEFT].GetNowPattern());
		}
	}
}