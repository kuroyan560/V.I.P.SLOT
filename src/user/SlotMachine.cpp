#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"
#include"ConstParameters.h"
#include"AudioApp.h"

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

		//全リール停止済
		if (REEL::RIGHT < m_lever && !m_reels[REEL::RIGHT].IsSpin())m_lever = -1;
	}
}

#include"DrawFunc3D.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> LigMgr, std::weak_ptr<Camera> Cam)
{
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *Cam.lock(), 1.0f, AlphaBlendMode_None);
}

void SlotMachine::Reel::Attach(ModelMesh* ReelMesh)
{
	m_meshPtr = ReelMesh;

	//リールメッシュ頂点のV初期化値保存
	auto& vertices = m_meshPtr->mesh->vertices;
	m_initV.resize(vertices.size());
	for (int vertIdx = 0; vertIdx < static_cast<int>(vertices.size()); ++vertIdx)
	{
		m_initV[vertIdx] = vertices[vertIdx].uv.y;
	}
}

void SlotMachine::Reel::Init(std::shared_ptr<TextureBuffer> ReelTex, int PatternNum)
{
	//リールのメッシュが見つけられていない
	if (m_meshPtr == nullptr)
	{
		printf("The reel hasn't found its mesh pointer.It is nullptr.");
		assert(0);
	}

	//リールのテクスチャ指定
	if (ReelTex)m_meshPtr->material->texBuff[COLOR_TEX] = ReelTex;

	//絵柄の数指定
	m_patternNum = PatternNum;

	//回転量リセット
	m_vOffset = 0.0f;
	//リールメッシュに反映
	SpinAffectUV();

	//回転フラグリセット
	m_isSpin = false;
	//回転速度リセット
	m_spinSpeed = 0.0f;

	//回転始めフラグリセット
	m_isStartSpin = false;
	//回転終了フラグリセット
	m_isEndSpin = false;

	//スロットの絵柄結果
	m_nowPatternIdx = 0;

	//タイマーリセット
	m_timer = -1;
}

void SlotMachine::Reel::Update()
{
	//回転中じゃないならスルー
	if (!m_isSpin)return;

	//タイマー起動中（ -1 でオフ）
	if (0 <= m_timer)m_timer++;

	//回転始め
	if (m_isStartSpin)
	{
		//回転速度加速
		m_spinSpeed = KuroMath::Ease(In, Back, m_timer,
			ConstParameter::Slot::UNTIL_MAX_SPEED_TIME, 0.0f, ConstParameter::Slot::MAX_SPIN_SPEED);

		//最高速度到達
		if (ConstParameter::Slot::UNTIL_MAX_SPEED_TIME < m_timer)
		{
			m_isStartSpin = false;	//回転始めフラグを下ろす
			m_timer = -1;				//タイマーリセット
		}
	}
	
	//回転終了
	if (m_isEndSpin)
	{
		// (0 ~ 1) を (-1 ~1) の範囲に補正してイージング計算
		float easeRate = KuroMath::Ease(Out, Elastic, m_timer, 
			ConstParameter::Slot::FINISH_SPIN_TIME, 0.0f, 1.0f) * 2.0f - 2.0f;

		//回転停止直後の振動量計算
		float shake = ConstParameter::Slot::FINISH_SPIN_SHAKE_MAX * easeRate;

		//振動しながら停止
		m_vOffset = m_vOffsetFixedStop + shake;

		//振動終了
		if (ConstParameter::Slot::FINISH_SPIN_TIME < m_timer)
		{
			m_isSpin = false;			//回転終了
			m_isEndSpin = false;	//回転終了フラグを下ろす
			m_timer = -1;				//タイマーリセット
		}
	}
	//V回転
	else
	{
		m_vOffset += m_spinSpeed;
	}

	//ループ
	if (m_vOffset < -1.0f)m_vOffset += 1.0f;

	//リールメッシュに回転を反映
	SpinAffectUV();
}

void SlotMachine::Reel::Start()
{
	//回転スタート
	m_isSpin = true;
	m_isStartSpin = true;

	//タイマースタート
	m_timer = 0;

	//回転中
	m_nowPatternIdx = -1;
}

void SlotMachine::Reel::Stop()
{
	//回転終了
	m_isEndSpin = true;

	//タイマースタート
	m_timer = 0;

	//小数第１位を丸め込む
	m_vOffsetFixedStop = roundf(m_vOffset * 10.0f) / 10.0f;

	//停止位置の絵柄インデックス記録
	const float vSpan = 1.0f / (m_patternNum - 1);
	m_nowPatternIdx = m_patternNum + static_cast<int>(m_vOffsetFixedStop / vSpan);
	printf("\n%d", m_nowPatternIdx);
}

void SlotMachine::Reel::SpinAffectUV()
{
	//頂点情報のUV回転量セット
	for (int vertIdx = 0; vertIdx < static_cast<int>(m_meshPtr->mesh->vertices.size()); ++vertIdx)
	{
		m_meshPtr->mesh->vertices[vertIdx].uv.y = m_initV[vertIdx] + m_vOffset;
	}
	m_meshPtr->mesh->Mapping();
}
