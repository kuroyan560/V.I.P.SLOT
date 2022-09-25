#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"

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
}

void SlotMachine::Init()
{
	//リール初期化
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Init();
}

//デバッグ用
#include"UsersInput.h"

void SlotMachine::Update()
{
	//リール更新
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Update();

	//デバッグ用
	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::RB))
	{
		for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Start();
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

void SlotMachine::Reel::Init(std::shared_ptr<TextureBuffer> ReelTex)
{
	//リールのメッシュが見つけられていない
	if (m_meshPtr == nullptr)
	{
		printf("The reel hasn't found its mesh pointer.It is nullptr.");
		assert(0);
	}

	//リールのテクスチャ指定
	if (ReelTex)m_meshPtr->material->texBuff[COLOR_TEX] = ReelTex;

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
	//タイマーリセット
	m_timer = -1;
}

void SlotMachine::Reel::Update()
{
	//回転中じゃないならスルー
	if (!m_isSpin)return;

	//タイマー起動中（ -1 でオフ）
	if (0 <= m_timer)m_timer++;

	//最高速度になるまでの時間
	const int UNTIL_MAX_SPEED_TIME = 20;
	//最高回転速度
	const float MAX_SPIN_SPEED = -0.005f;

	//回転始め
	if (m_isStartSpin)
	{
		//回転速度加速
		m_spinSpeed = KuroMath::Ease(In, Back, m_timer, UNTIL_MAX_SPEED_TIME, 0.0f, MAX_SPIN_SPEED);

		//最高速度到達
		if (UNTIL_MAX_SPEED_TIME < m_timer)
		{
			m_isStartSpin = false;	//回転始めフラグを下ろす
			m_timer = -1;	//タイマーリセット
		}
	}

	//V回転
	m_vOffset += m_spinSpeed;

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
