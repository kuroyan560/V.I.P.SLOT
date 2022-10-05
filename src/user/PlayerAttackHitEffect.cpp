#include "PlayerAttackHitEffect.h"
#include"D3D12App.h"

PlayerAttackHitEffect::PlayerAttackHitEffect()
{
	D3D12App::Instance()->GenerateTextureBuffer(
		m_texArray.data(), 
		"resource/user/img/hitEffect.png", 
		TEX_NUM, 
		Vec2<int>(TEX_NUM, 1));
}

#include"DrawFuncBillBoard.h"
void PlayerAttackHitEffect::Info::Draw(const PlayerAttackHitEffect& arg_parent, std::weak_ptr<Camera>& arg_cam)
{
	const Vec2<float>SIZE = { 6.0f,6.0f };
	DrawFuncBillBoard::Graph(*arg_cam.lock(), m_pos, SIZE, arg_parent.m_texArray[m_texIdx]);
}

void PlayerAttackHitEffect::Init()
{
	m_infoArray.clear();
}

void PlayerAttackHitEffect::Update(float arg_timeScale)
{
	for (auto& info : m_infoArray)
	{
		info.Update(arg_timeScale);
	}

	//エフェクト終了していたら削除
	m_infoArray.remove_if([](Info& info)
		{
			return info.IsEnd();
		});
}

void PlayerAttackHitEffect::Draw(std::weak_ptr<Camera> arg_cam)
{
	for (auto& info : m_infoArray)
	{
		info.Draw(*this, arg_cam);
	}
}

void PlayerAttackHitEffect::Emit(Vec3<float>arg_emitPos)
{
	const int TEX_CHANGE_SPAN = 3;
	//Vec3<float>OFFSET = { 0.0f,-0.5f,0.0f };
	Vec3<float>OFFSET = { 0.0f,0.0f,0.0f };
	m_infoArray.emplace_front(arg_emitPos + OFFSET, TEX_CHANGE_SPAN);
}
