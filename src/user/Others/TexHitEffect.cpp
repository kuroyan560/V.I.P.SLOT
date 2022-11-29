#include "TexHitEffect.h"
#include"DrawFuncBillBoard.h"
#include"D3D12App.h"

TexHitEffect::Info::Info(Vec3<float>arg_emitPos, float arg_texChangeSpan, bool arg_isLoop, float arg_interval)
	:m_pos(arg_emitPos), m_loop(arg_isLoop)
{
	m_texChangeTimer.Reset(arg_texChangeSpan);

	if (!arg_isLoop)m_lifeSpan.Reset(60.0f);
	else m_lifeSpan.Reset(arg_interval);
}

void TexHitEffect::Info::Update(int arg_texNum, float arg_timeScale)
{
	if (m_loop)m_lifeSpan.UpdateTimer(arg_timeScale);

	if (m_texChangeTimer.UpdateTimer(arg_timeScale))
	{
		++m_texIdx;
		if (m_loop && arg_texNum <= m_texIdx)m_texIdx = 0;
		m_texChangeTimer.Reset();
	}
}

void TexHitEffect::Info::Draw(const TexHitEffect& arg_parent, std::weak_ptr<Camera>& arg_cam)
{
	DrawFuncBillBoard::Graph(*arg_cam.lock(), m_pos, arg_parent.m_effectSize, arg_parent.m_texArray[m_texIdx]);
}

TexHitEffect::TexHitEffect()
{
	DrawFuncBillBoard::GeneratePipeline(AlphaBlendMode_Trans);
	DrawFuncBillBoard::PrepareGraphVertBuff(5);
}

void TexHitEffect::Set(std::string arg_texPath,
	int arg_texNum,
	Vec2<int> arg_texSplitNum,
	Vec2<float>arg_effectSize,
	float arg_texChangeSpan,
	bool arg_loop,
	float arg_loopLifeSpan)
{
	m_texArray.resize(static_cast<size_t>(arg_texNum));
	D3D12App::Instance()->GenerateTextureBuffer(m_texArray.data(), arg_texPath, arg_texNum, arg_texSplitNum);

	m_effectSize = arg_effectSize;
	m_texChangeSpan = arg_texChangeSpan;
}

void TexHitEffect::Set(Vec2<float> arg_effectSize, float arg_texChangeSpan, bool arg_loop, float arg_loopLifeSpan)
{
	m_effectSize = arg_effectSize;
	m_texChangeSpan = arg_texChangeSpan;
	m_loop = arg_loop;
	m_loopLifeSpan = arg_loopLifeSpan;
}

void TexHitEffect::Set(std::vector<std::shared_ptr<TextureBuffer>>& arg_texArray, Vec2<float> arg_effectSize, float arg_texChangeSpan, bool arg_loop, float arg_loopLifeSpan)
{
	SetTexture(arg_texArray);
	Set(arg_effectSize, arg_texChangeSpan, arg_loop, arg_loopLifeSpan);
}

void TexHitEffect::SetTexture(std::vector<std::shared_ptr<TextureBuffer>>& arg_texArray)
{
	m_texArray = arg_texArray;
}

void TexHitEffect::Init()
{
	m_infoArray.clear();
}

void TexHitEffect::Update(float arg_timeScale)
{
	for (auto& info : m_infoArray)
	{
		info.Update(static_cast<int>(m_texArray.size()), arg_timeScale);
	}

	//エフェクト終了していたら削除
	m_infoArray.remove_if([this](Info& info)
		{
			return info.IsEnd(static_cast<int>(m_texArray.size()));
		});
}

void TexHitEffect::Draw(std::weak_ptr<Camera> arg_cam)
{
	for (auto& info : m_infoArray)
	{
		info.Draw(*this, arg_cam);
	}
}

void TexHitEffect::Emit(Vec3<float>arg_emitPos)
{
	m_infoArray.emplace_front(arg_emitPos, m_texChangeSpan, m_loop, m_loopLifeSpan);
}
