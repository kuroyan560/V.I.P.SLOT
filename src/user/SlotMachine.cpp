#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"Importer.h"
#include"GameCamera.h"
#include"TimeScale.h"
#include"Player.h"
#include"KuroEngine.h"
#include"DrawFunc2D.h"

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

void SlotMachine::UpdateSlotGaugeScreen()
{
	using namespace ConstParameter::Slot;

	auto& cmdList = D3D12App::Instance()->GetCmdList();
	m_slotGaugeScreen->Clear(cmdList);

	KuroEngine::Instance()->Graphics().SetRenderTargets({ m_slotGaugeScreen });

	//スロットゲージ液晶画面画像サイズ
	const auto slotGaugeScreenSize = WinApp::Instance()->GetWinSize().Float();;
	//スロットゲージ画像サイズ
	const auto slotGaugeTexSize = m_slotGaugeTex->GetGraphSize().Float();

	//スロットゲージを描画する領域のレート
	const Vec2<float>slotGaugeDrawScreenSizeRate = { 0.95f,0.7f };
	//const Vec2<float>slotGaugeDrawScreenSizeRate = { 1.0f,1.0f };
	//スロットゲージを描画する領域
	const Vec2<float>slotGaugeDrawScreenSize = slotGaugeScreenSize * slotGaugeDrawScreenSizeRate;

	//間隔
	const float slotGaugeSpace = 30.0f;

	//スロットゲージ最大からスロットゲージの描画サイズを算出（Xは余白を考慮）
	const Vec2<float>slotGaugeDrawSize =
	{
		(slotGaugeDrawScreenSize.x - (SLOT_GAUGE_MAX - 1) * slotGaugeSpace) / SLOT_GAUGE_MAX,
		slotGaugeDrawScreenSize.y,
	};

	//描画位置一番左
	const Vec2<float>minDrawPos = (slotGaugeScreenSize - slotGaugeDrawScreenSize) / 2.0f;

	//スロットゲージ描画
	for (int i = 0; i < m_startSlotCount; ++i)
	{
		Vec2<float>drawPos =
		{
			minDrawPos.x + i * (slotGaugeDrawSize.x + slotGaugeSpace),
			minDrawPos.y
		};
		DrawFunc2D::DrawExtendGraph2D(drawPos, drawPos + slotGaugeDrawSize, m_slotGaugeTex);
	}
}

SlotMachine::SlotMachine()
{
	//スロットマシン生成
	m_slotMachineObj = std::make_shared<ModelObject>("resource/user/model/", "slotMachine.glb");
	//トランスフォーム調整
	m_slotMachineObj->m_transform.SetScale(2.0f);
	m_slotMachineObj->m_transform.SetPos({ 0.0f,3.0f,0.0f });

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

	/*--- スロットゲージ ---*/
	//スロットゲージを映す画面レンダーターゲット生成
	m_slotGaugeScreen = D3D12App::Instance()->GenerateRenderTarget(
		D3D12App::Instance()->GetBackBuffFormat(),
		Color(70, 60, 94, 255), 
		Vec2<int>(1000,200),
		L"SlotGauge - Screen");
	//スロットゲージテクスチャ生成
	m_slotGaugeTex = D3D12App::Instance()->GenerateTextureBuffer(
		Color(74, 185, 163, 255));

	//レンダーターゲットをスロットゲージを映す画面テクスチャにアタッチ
	const std::string SLOT_GAUGE_SCREEN_MATERIAL_NAME = "SlotGaugeScreen";
	for (auto& mesh : m_slotMachineObj->m_model->m_meshes)
	{
		auto& material = mesh.material;
		if (material->name.compare(SLOT_GAUGE_SCREEN_MATERIAL_NAME) != 0)continue;

		//目的のマテリアル発見、新しいテクスチャをアタッチ
		material->texBuff[0] = m_slotGaugeScreen;
		break;
	}

	//リール初期絵柄セット（デバッグ用、何も効果なし）
		//デバッグ用
	using PATTERN = ConstParameter::Slot::PATTERN;
	std::vector<PATTERN>testPatternArray;
	testPatternArray.resize(20);
	for (int i = 0; i < 20; ++i)
	{
		testPatternArray[i] = PATTERN::NONE;
	}

	//リール初期化
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].SetPattern(nullptr, testPatternArray);
}

void SlotMachine::Init()
{
	//レバー初期化
	m_lever = REEL::NONE;

	//スロット回転予約リセット
	m_startSlotCount = 0;

	//リール初期化
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Init();

	//スロットゲージ画面更新
	UpdateSlotGaugeScreen();

	//スロットゲージ自動操作タイムスケール
	m_autoOperateTimeScale = 1.0f;
}

void SlotMachine::Update(std::weak_ptr<Player>arg_player, const TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Slot;

	const float timeScale = arg_timeScale.GetTimeScale();

	//自動操作時間計測のタイムスケール
	//ゲージ量が多いほど時間が早まる
	const float autoOperateTimeScale =
		KuroMath::Lerp(1.0f,
			AUTO_OPERATE_TIME_SCALE_MAX,
			KuroMath::GetRateInRange(
				DEFAULT_TIME_SCALE_SLOT_GAUGE_NUM,
				MAX_TIME_SCALE_SLOT_GAUGE_NUM,
				m_startSlotCount)) * timeScale;

	//リール更新
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Update(timeScale);

	//停止中
	if (m_lever == REEL::NONE)
	{
		//スロット予約がある
		if (m_startSlotCount)
		{
			//予約数消費
			m_startSlotCount--;
			//スロットゲージ画面更新
			UpdateSlotGaugeScreen();
			//スロットスタート
			Lever();
			//最初のリール停止までのタイマーセット
			m_autoTimer[AUTO_OPERATE::UNTIL_FIRST_REEL].Reset(AUTO_OPERATE_TIME[AUTO_OPERATE::UNTIL_FIRST_REEL]);
		}
	}
	//稼働中
	else if(m_lever < REEL::NUM)
	{
		m_autoTimer[AUTO_OPERATE::UNTIL_FIRST_REEL].UpdateTimer(autoOperateTimeScale);
		m_autoTimer[AUTO_OPERATE::REEL_STOP_SPAN].UpdateTimer(autoOperateTimeScale);

		//リール停止
		if (m_autoTimer[AUTO_OPERATE::UNTIL_FIRST_REEL].IsTimeUpOnTrigger()
			|| m_autoTimer[AUTO_OPERATE::REEL_STOP_SPAN].IsTimeUpOnTrigger())
		{
			if (Lever())
			{
				//最後のリール停止
				m_autoTimer[AUTO_OPERATE::AFTER_STOP_ALL_REEL].Reset(
					AUTO_OPERATE_TIME[AUTO_OPERATE::AFTER_STOP_ALL_REEL]);
			}
			else
			{
				//次のリールへ
				m_autoTimer[AUTO_OPERATE::REEL_STOP_SPAN].Reset(
					AUTO_OPERATE_TIME[AUTO_OPERATE::REEL_STOP_SPAN]);
			}
		}

	}
	else
	{
		if (m_autoTimer[AUTO_OPERATE::AFTER_STOP_ALL_REEL].UpdateTimer(autoOperateTimeScale))
		{
			m_lever = REEL::NONE;
		}
	}
}

#include"BasicDraw.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	BasicDraw::Draw(*arg_lightMgr.lock(), m_slotMachineObj, *arg_cam.lock());

	//DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);
	//DrawFunc3D::DrawNonShadingModel(m_megaPhoneObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);
}

#include"imguiApp.h"
void SlotMachine::ImguiDebug()
{
	ImGui::Begin("SlotMachine");
	ImGui::Text("StartSlotCount : { %d }", m_startSlotCount);

	using namespace ConstParameter::Slot;
	const float autoOperateTimeScale =
		KuroMath::Lerp(1.0f,
			AUTO_OPERATE_TIME_SCALE_MAX,
			KuroMath::GetRateInRange(
				DEFAULT_TIME_SCALE_SLOT_GAUGE_NUM,
				MAX_TIME_SCALE_SLOT_GAUGE_NUM,
				m_startSlotCount));
	ImGui::Text("AutoOperateTimeScale : { %f }", autoOperateTimeScale);

	ImGui::End();
}

bool SlotMachine::Lever()
{
	//スロット回転開始
	if (m_lever == REEL::NONE)
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
			return true;
		}
	}
	return false;
}

void SlotMachine::Booking()
{
	//スロット予約（スロットゲージ）、上限を超えないように
	m_startSlotCount = std::min(ConstParameter::Slot::SLOT_GAUGE_MAX, m_startSlotCount + 1);


	if (m_lever == REEL::NONE)return;
	//スロットゲージ画面更新
	UpdateSlotGaugeScreen();
}

void SlotMachine::ReelSet(REEL arg_which,
	std::shared_ptr<TextureBuffer>& arg_reelTex,
	std::vector<ConstParameter::Slot::PATTERN>& arg_patternArray)
{
	m_reels[arg_which].SetPattern(arg_reelTex, arg_patternArray);
}
