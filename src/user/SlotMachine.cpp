#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"Importer.h"
#include"GameCamera.h"

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

	//コインモデル読み込み
	m_coinModel = Importer::Instance()->LoadModel("resource/user/model/", "coin.glb");

	//サウンド読み込み
	m_spinStartSE = AudioApp::Instance()->LoadAudio("resource/user/sound/slot_start.wav");
	m_reelStopSE = AudioApp::Instance()->LoadAudio("resource/user/sound/slot_stop.wav");
}

void SlotMachine::Init()
{
	//リール初期化
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Init();

	//レバー初期化
	m_lever = -1;

	//最後のリールを止めてからの時間計測用タイマーリセット
	m_slotWaitTimer = 0;

	//コインリセット
	m_coinVault.Init(0);
}

//デバッグ用
#include"UsersInput.h"

void SlotMachine::Update()
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
			m_lever++;
		}
	}

	//全リール停止済
	if (REEL::RIGHT < m_lever)
	{
		m_slotWaitTimer++;

		//次のスロット回転可能に
		if (ConstParameter::Slot::SLOT_WAIT_TIME < m_slotWaitTimer)
		{
			m_lever = -1;
			m_slotWaitTimer = 0;
		}
	}

	for (auto& coin : m_betCoinArray)
	{
		if (ConstParameter::Slot::UNTIL_THROW_COIN_TO_BET < coin.m_timer)
		{
			//相手からコイン受け取り
			assert(coin.m_otherVault); //一応nullチェック
			coin.m_otherVault->Pass(m_coinVault, coin.m_coinNum);
			//BETに成功
			coin.m_bet = true;
		}
		else
		{
			//コインの座標を算出してトランスフォームに適用
			Vec3<float>newPos = KuroMath::Lerp(	coin.m_emitTransform.GetPos(), ConstParameter::Slot::COIN_PORT_POS, 
				coin.m_timer, ConstParameter::Slot::UNTIL_THROW_COIN_TO_BET);
			coin.m_transform.SetPos(newPos);
		}

		//時間計測
		coin.m_timer++;
	}

	//投入口に到達したら削除
	m_betCoinArray.remove_if([](BetCoin& c)
		{
			return c.m_bet;
		});
}

#include"DrawFunc3D.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<GameCamera>arg_gameCam)
{
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_megaPhoneObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);

	//BETされたコインの描画
	for (auto& coin : m_betCoinArray)
	{
		DrawFunc3D::DrawNonShadingModel(m_coinModel, coin.m_transform, *arg_gameCam.lock()->GetFrontCam(), 1.0f, nullptr, AlphaBlendMode_None);
	}
}

void SlotMachine::Bet(CoinVault& arg_otherVault, int arg_coinNum, const Transform& arg_emitTransform)
{
	//BETされたコイン情報追加
	m_betCoinArray.emplace_front(&arg_otherVault, arg_coinNum, arg_emitTransform);
}