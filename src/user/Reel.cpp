#include "Reel.h"
#include"Model.h"
#include"ConstParameters.h"

void Reel::Attach(ModelMesh* arg_reelMesh)
{
	m_meshPtr = arg_reelMesh;

	//リールメッシュ頂点のV初期化値保存
	auto& vertices = m_meshPtr->mesh->vertices;
	m_initV.resize(vertices.size());
	for (int vertIdx = 0; vertIdx < static_cast<int>(vertices.size()); ++vertIdx)
	{
		m_initV[vertIdx] = vertices[vertIdx].uv.y;
	}
}

void Reel::Init(std::shared_ptr<TextureBuffer>arg_reelTex, std::vector<PATTERN>arg_patternArray)
{
	//リールのメッシュが見つけられていない
	if (m_meshPtr == nullptr)
	{
		printf("The reel hasn't found its mesh pointer.It is nullptr.");
		assert(0);
	}

	//リールのテクスチャ指定
	if (arg_reelTex)m_meshPtr->material->texBuff[COLOR_TEX] = arg_reelTex;

	//絵柄の数指定
	m_patternArray = arg_patternArray;

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

	//リール停止時のVオフセット（中途半端な位置にならないよう補正をかけたもの）
	m_vOffsetFixedStop = 0.0f;

	//スロットの絵柄結果
	m_nowPatternIdx = 0;
}

void Reel::Update(float arg_timeScale)
{
	//回転中じゃないならスルー
	if (!m_isSpin)return;

	//タイマー更新
	m_timer.UpdateTimer(arg_timeScale);

	//回転始め
	if (m_isStartSpin)
	{
		//回転速度加速
		m_spinSpeed = KuroMath::Ease(In, Back, m_timer.GetTimeRate(), 0.0f, ConstParameter::Slot::MAX_SPIN_SPEED);

		//最高速度到達
		if (m_timer.IsTimeUp())
		{
			m_isStartSpin = false;	//回転始めフラグを下ろす
		}
	}

	//回転終了
	if (m_isEndSpin)
	{
		// (0 ~ 1) を (-1 ~1) の範囲に補正してイージング計算
		float easeRate = KuroMath::Ease(Out, Elastic, m_timer.GetTimeRate(), 0.0f, 1.0f) * 2.0f - 2.0f;

		//回転停止直後の振動量計算
		float shake = ConstParameter::Slot::FINISH_SPIN_SHAKE_MAX * easeRate;

		//振動しながら停止
		m_vOffset = m_vOffsetFixedStop + shake;

		//振動終了
		if (m_timer.IsTimeUp())
		{
			m_isSpin = false;			//回転終了
			m_isEndSpin = false;	//回転終了フラグを下ろす
		}
	}
	//V回転
	else
	{
		m_vOffset += m_spinSpeed * arg_timeScale;
	}

	//リールメッシュに回転を反映
	SpinAffectUV();
}

void Reel::Start()
{
	//回転スタート
	m_isSpin = true;
	m_isStartSpin = true;
	m_isEndSpin = false;

	//タイマースタート
	m_timer.Reset(ConstParameter::Slot::UNTIL_MAX_SPEED_TIME);

	//回転中
	m_nowPatternIdx = -1;

	//振動リセット
	m_vOffset = m_vOffsetFixedStop;
}

void Reel::Stop()
{
	//回転終了
	m_isEndSpin = true;

	//タイマースタート
	m_timer.Reset(ConstParameter::Slot::FINISH_SPIN_TIME);

	//小数第１位を丸め込んで補正（中途半端な位置で止まらないようにする）
	float roundOffset = roundf(m_vOffset * 10.0f) / 10.0f;

	//停止位置の絵柄インデックス記録
	const int patternNum = static_cast<int>(m_patternArray.size());
	const float vSpan = 1.0f / (patternNum - 1);
	m_nowPatternIdx = static_cast<int>(patternNum + roundOffset / vSpan);

	//絵柄のループ
	while (m_nowPatternIdx < 0)m_nowPatternIdx += patternNum;
	while (patternNum <= m_nowPatternIdx)m_nowPatternIdx -= patternNum;

	//停止位置のV値を記録
	m_vOffsetFixedStop = 1.0f / patternNum * m_nowPatternIdx;

	printf("ReelStop : %d\n", m_nowPatternIdx + 1);
}

void Reel::SpinAffectUV()
{
	//頂点情報のUV回転量セット
	for (int vertIdx = 0; vertIdx < static_cast<int>(m_meshPtr->mesh->vertices.size()); ++vertIdx)
	{
		m_meshPtr->mesh->vertices[vertIdx].uv.y = m_initV[vertIdx] + m_vOffset;
	}
	m_meshPtr->mesh->Mapping();
}
