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

void SlotMachine::Update()
{
	const float REEL_SPIN_SPEED = -0.005f;

	//リール回転
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)
	{
		//リール取得
		auto& reel = m_reels[reelIdx];

		//リール回転
		reel.m_vOffset += REEL_SPIN_SPEED;

		//リールメッシュに反映
		reel.SpinAffectUV();
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
