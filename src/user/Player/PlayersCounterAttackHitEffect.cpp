#include "PlayersCounterAttackHitEffect.h"
#include"TexHitEffect.h"
#include"D3D12App.h"

PlayersCounterAttackHitEffect::PlayersCounterAttackHitEffect()
{
	//�A�j���[�V�����Ԋu
	const float ANIM_SPAN = 5.0f;

	//����
	const float LIFE_SPAN = 60.0f;

	//�X�P�[��
	const float EXPLOTION_SCALE = 4.0f;
	const float MINI_EXPLOTION_SCALE = 3.0f;

	//�����e�N�X�`���p��
	std::vector<std::shared_ptr<TextureBuffer>>explotionTexArray(COLOR_NUM);
	D3D12App::Instance()->GenerateTextureBuffer(
		explotionTexArray.data(),
		"resource/user/img/effect/causative/causative_explotion.png",
		COLOR_NUM, Vec2<int>(1, COLOR_NUM));

	//�����{�e�N�X�`���p��
	std::vector<std::shared_ptr<TextureBuffer>>numTexArray(COLOR_NUM * 11);
	D3D12App::Instance()->GenerateTextureBuffer(
		numTexArray.data(),
		"resource/user/img/effect/causative/causative_num.png",
		COLOR_NUM * 11, Vec2<int>(11, COLOR_NUM));

	//�����G�t�F�N�g�i�ʏ�j
	m_explotionEffect.Set(explotionTexArray, 
		Vec2<float>(EXPLOTION_SCALE, EXPLOTION_SCALE),
		ANIM_SPAN, 
		true, 
		LIFE_SPAN);

	//�����G�t�F�N�g�i���j
	std::vector<std::shared_ptr<TextureBuffer>>miniExplotionTexArray(COLOR_NUM);
	//�ʏ�̔����G�t�F�N�g�̃e�N�X�`���ƃC���f�b�N�X�����炷
	for (int i = 0; i < COLOR_NUM; ++i)
	{
		int selectIdx = i + 1;
		if (COLOR_NUM <= selectIdx)selectIdx = 0;
		miniExplotionTexArray[i] = explotionTexArray[selectIdx];
	}
	m_miniExplotionEffect.Set(miniExplotionTexArray,
		Vec2<float>(MINI_EXPLOTION_SCALE, MINI_EXPLOTION_SCALE),
		ANIM_SPAN,
		true,
		LIFE_SPAN);

	//�����e�N�X�`���z����Q�����z��Ɋi�[
	int loadTexIdx = 0;
	m_numTexArray.resize(COLOR_NUM);
	for (int colorIdx = 0; colorIdx < COLOR_NUM; ++colorIdx)
	{
		m_numTexArray[colorIdx].resize(11);
		for (int i = 0; i < 11; ++i)
		{
			m_numTexArray[colorIdx][i] = numTexArray[loadTexIdx++];
		}
	}
}

void PlayersCounterAttackHitEffect::Init()
{
	m_explotionEffect.Init();
	m_miniExplotionEffect.Init();
	m_numEffect.Init();
}

void PlayersCounterAttackHitEffect::Update(const float& arg_timeScale)
{
	m_explotionEffect.Update(arg_timeScale);
	m_miniExplotionEffect.Update(arg_timeScale);
	m_numEffect.Update(arg_timeScale);
}

void PlayersCounterAttackHitEffect::Draw(std::weak_ptr<Camera>arg_cam)
{
	m_explotionEffect.Draw(arg_cam);
	m_miniExplotionEffect.Draw(arg_cam);
	m_numEffect.Draw(arg_cam);
}

void PlayersCounterAttackHitEffect::Emit(Vec3<float> arg_pos, int arg_num)
{
	m_explotionEffect.Emit(arg_pos);
	m_miniExplotionEffect.Emit(arg_pos);
}
