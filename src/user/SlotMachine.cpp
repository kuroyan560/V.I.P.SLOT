#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"Importer.h"
#include"GameCamera.h"

bool SlotMachine::CheckReelPattern()
{
	const auto& pattern = m_reels[REEL::LEFT].GetNowPattern();

	//絵柄が違えば何もせず終了
	if (pattern != m_reels[REEL::CENTER].GetNowPattern())return false;
	if (pattern != m_reels[REEL::RIGHT].GetNowPattern())return false;

	m_patternMgr.Invoke(pattern);
}

SlotMachine::SlotMachine() : m_patternMgr(m_coinVault)
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

	//BETコインマネージャ
	m_betCoinObjManager = std::make_unique<CoinObjectManager>(new BetCoinPerform());

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
		if (i % 2 == 0)testPatternArray[i] = PATTERN::DOUBLE;
		else testPatternArray[i] = PATTERN::TRIPLE;
	}

	//リール初期化
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Init(nullptr, testPatternArray);

	//レバー初期化
	m_lever = -1;

	//最後のリールを止めてからの時間計測用タイマーリセット
	m_slotWaitTimer.Reset(ConstParameter::Slot::SLOT_WAIT_TIME);

	//コインリセット
	m_coinVault.Set(0);

	//BETコインリセット
	m_betCoinObjManager->Init();
}

//デバッグ用
#include"UsersInput.h"

void SlotMachine::Update(CoinVault& arg_playersVault)
{
	//リール更新
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Update();

	//レバー操作
	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::RB))
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
			if (m_lever++ == REEL::RIGHT)CheckReelPattern();
		}
	}

	//全リール停止済
	if (REEL::RIGHT < m_lever)
	{
		//次のスロット回転可能に
		if (m_slotWaitTimer.UpdateTimer())
		{
			m_lever = -1;
			m_slotWaitTimer.Reset();
		}
	}

	const int MEGA_PHONE_EXPAND_TIME = 60;

	//BETコイン投げ入れ演出
	if (int betCoinNum = m_betCoinObjManager->Update())
	{
		//プレイヤーからコイン受け取り
		arg_playersVault.Pass(m_coinVault, betCoinNum);
		//メガホン拡縮
		m_megaPhoneExpandTimer.Reset(MEGA_PHONE_EXPAND_TIME);
	}

	//メガホン拡縮
	float megaPhoneScale = KuroMath::Ease(Out, Elastic, m_megaPhoneExpandTimer.GetTimeRate(), 
		ConstParameter::Slot::MEGA_PHONE_EXPAND_SCALE, 1.0f);
	m_megaPhoneObj->m_transform.SetScale(megaPhoneScale);
	m_megaPhoneExpandTimer.UpdateTimer();
}

#include"DrawFunc3D.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<GameCamera>arg_gameCam)
{
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_megaPhoneObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);

	//BETされたコインの描画
	m_betCoinObjManager->Draw(arg_lightMgr, arg_gameCam);
}

void SlotMachine::Bet(int arg_coinNum, const Transform& arg_emitTransform)
{
	//BETされたコイン情報追加
	m_betCoinObjManager->Add(arg_coinNum, arg_emitTransform, ConstParameter::Slot::UNTIL_THROW_COIN_TO_BET);
}

void SlotMachine::BetCoinPerform::OnUpdate(Coins& arg_coin)
{
	//コインの座標を算出してトランスフォームに適用
	Vec3<float>newPos = KuroMath::Lerp(arg_coin.m_initTransform.GetPos(), ConstParameter::Slot::COIN_PORT_POS, arg_coin.m_timer.GetTimeRate());
	arg_coin.m_transform.SetPos(newPos);
}