#include "TexHitEffect.h"
#include"DrawFuncBillBoard.h"
#include"D3D12App.h"

TexHitEffect::Info::Info(Vec3<float>arg_emitPos, float arg_texChangeSpan)
	:m_pos(arg_emitPos)
{
	m_texChangeTimer.Reset(arg_texChangeSpan);
}

void TexHitEffect::Info::Update(float arg_timeScale)
{
	if (m_texChangeTimer.UpdateTimer(arg_timeScale))
	{
		++m_texIdx;
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
	float arg_texChangeSpan)
{
	m_texArray.resize(static_cast<size_t>(arg_texNum));
	D3D12App::Instance()->GenerateTextureBuffer(m_texArray.data(), arg_texPath, arg_texNum, arg_texSplitNum);

	m_effectSize = arg_effectSize;
	m_texChangeSpan = arg_texChangeSpan;
}

void TexHitEffect::Init()
{
	m_infoArray.clear();
}

void TexHitEffect::Update(float arg_timeScale)
{
	for (auto& info : m_infoArray)
	{
		info.Update(arg_timeScale);
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
	m_infoArray.emplace_front(arg_emitPos, m_texChangeSpan);
}
