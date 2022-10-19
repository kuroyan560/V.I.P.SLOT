#include "TexHitEffect.h"
#include"DrawFuncBillBoard.h"

void TexHitEffect::Info::Draw(const TexHitEffect& arg_parent, std::weak_ptr<Camera>& arg_cam)
{
	DrawFuncBillBoard::Graph(*arg_cam.lock(), m_pos, arg_parent.m_effectSize, arg_parent.m_texArray[m_texIdx]);
}

void TexHitEffect::Init(std::shared_ptr<TextureBuffer>* arg_texArray, int arg_texElementNum, Vec2<float>arg_effectSize)
{
	m_infoArray.clear();
	m_texArray = arg_texArray;
	m_texNum = arg_texElementNum;
	m_effectSize = arg_effectSize;
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
			return info.IsEnd(m_texNum);
		});
}

void TexHitEffect::Draw(std::weak_ptr<Camera> arg_cam)
{
	for (auto& info : m_infoArray)
	{
		info.Draw(*this, arg_cam);
	}
}

void TexHitEffect::Emit(Vec3<float>arg_emitPos, float arg_texChangeSpan)
{
	//const int TEX_CHANGE_SPAN = 3;
	m_infoArray.emplace_front(arg_emitPos, arg_texChangeSpan);
}
