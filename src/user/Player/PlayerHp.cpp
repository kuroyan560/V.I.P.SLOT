#include "PlayerHp.h"
#include"ConstParameters.h"
#include"D3D12App.h"
#include"DrawFunc2D.h"
#include"Sprite.h"

Vec2<float> PlayerHp::CalculateHpBarSize(float arg_rate)
{
	auto hpBarSize = m_hpBar.m_sprite->GetTex()->GetGraphSize().Float();
	hpBarSize.x = arg_rate * hpBarSize.x;
	return hpBarSize;
}

PlayerHp::PlayerHp()
{
	//�������ʒu�ݒ�
	m_uiInitPos = { 20.0f,640.0f };
	m_hpStr.m_initPos = { 0.0f,-48.0f };
	m_hpBar.m_initPos = { 13.0f,13.0f };
	m_hpBarDamage.m_initPos = { 13.0f,13.0f };
	m_hpBarFrame.m_initPos = { 0.0f,0.0f };
	Vec2<float>heartSpriteInitPos = { 6.0f,4.0f };
	Vec2<float>heartSpriteFrameInitPos = { 0.0f,0.0f };

	//���C�t�֘A
	//���C�����C�t�i��ԉE�A���ɏ�����j�̈ʒu
	Vec2<float>	mainHeartInitPos = { 317.0f,-50.0f };
	//���C�����C�t�̃X�P�[��
	float mainHeartScale = 1.0f;
	//���̑��̃��C�t�̈ʒu�I�t�Z�b�g�i���C���n�[�g��j
	Vec2<float>subHeartPosOffsetWithMain = { -39.0f,13.0f };
	//�T�u���m��X���W�I�t�Z�b�g
	float subHeartXOffset = -39.0f;
	//�T�u���C�t�̃X�P�[��
	float subHeartScale = 0.8f;

	//DirectX12�@�\�擾
	auto app = D3D12App::Instance();
	//�摜�t�@�C���̃f�B���N�g��
	std::string dir = "resource/user/img/ui/hp/";

	//�X�v���C�g�����A�摜�ǂݍ���
	m_hpStr.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_str.png"), "Sprite - HpStr");
	m_hpBar.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_bar.png"), "Sprite - HpBar");
	m_hpBarDamage.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_bar_damage.png"), "Sprite - HpBarDamage");
	m_hpBarFrame.m_sprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(dir + "hp_bar_frame.png"), "Sprite - HpBarFrame");
	auto heartTex = app->GenerateTextureBuffer(dir + "hp_heart.png");
	auto heartFrameTex = app->GenerateTextureBuffer(dir + "hp_heart_frame.png");

	//�n�[�g�X�v���C�g�����i�ő僉�C�t���p�Ӂj
	m_heartArray.resize(ConstParameter::Player::MAX_LIFE);
	for (int i = 0; i < ConstParameter::Player::MAX_LIFE; ++i)
	{
		//�X�v���C�g����
		m_heartArray[i].m_heart.m_sprite = std::make_shared<Sprite>(heartTex, ("Sprite - Heart - " + std::to_string(i)).c_str());
		m_heartArray[i].m_frame.m_sprite = std::make_shared<Sprite>(heartFrameTex, ("Sprite - HeartFrame - " + std::to_string(i)).c_str());
		//�X�v���C�g�����ʒu�ݒ�
		m_heartArray[i].m_heart.m_initPos = heartSpriteInitPos;
		m_heartArray[i].m_frame.m_initPos = heartSpriteFrameInitPos;

		//�n�[�g�ʒu�ݒ�
		if (i == 0)
		{
			//���C�����C�t�̐ݒ�����ďI��
			m_heartArray[i].m_initPos = mainHeartInitPos;
			m_heartArray[i].m_initScale = mainHeartScale;
			continue;
		}

		//�T�u���C�t�̃I�t�Z�b�g�K�p
		m_heartArray[i].m_initPos = mainHeartInitPos + subHeartPosOffsetWithMain;
		//�T�u���C�t�Ԃ�X�I�t�Z�b�g�K�p
		m_heartArray[i].m_initPos.x += (i - 1) * subHeartXOffset;
		//�T�u���C�t�̃X�P�[���K�p
		m_heartArray[i].m_initScale = subHeartScale;
	}

	//�g�����X�t�H�[���̐e�q�֌W�\�z
	m_hpStr.m_sprite->m_transform.SetParent(&m_transform);
	m_hpBar.m_sprite->m_transform.SetParent(&m_transform);
	m_hpBarDamage.m_sprite->m_transform.SetParent(&m_transform);
	m_hpBarFrame.m_sprite->m_transform.SetParent(&m_transform);
	for (auto& heart : m_heartArray)
	{
		heart.m_offsetTransform.SetParent(&m_transform);
		heart.m_frame.m_sprite->m_transform.SetParent(&heart.m_offsetTransform);
		heart.m_heart.m_sprite->m_transform.SetParent(&heart.m_offsetTransform);
	}
}

void PlayerHp::Init(int arg_initMaxLife, int arg_initRemainLife)
{
	using namespace ConstParameter::Player;

	//���C�t������
	m_life = arg_initRemainLife;

	//HP������
	m_hp = MAX_HP;

	//HP�o�[�T�C�Y�X�V
	m_hpBar.m_sprite->m_mesh.SetSize(CalculateHpBarSize(1.0f));

	//UI�ʒu������
	m_transform.SetPos(m_uiInitPos);

	//�`�悷��X�v���C�g������i�ǉ��� = �`�揇�j
	//HP�o�[�ƕ����͕K�{
	m_contents.clear();
	m_contents.emplace_back(&m_hpBarFrame);
	m_contents.emplace_back(&m_hpBarDamage);
	m_contents.emplace_back(&m_hpBar);
	m_contents.emplace_back(&m_hpStr);

	//���C�t�֘A���i�n�[�g�̌��ɂ���ĕϓ��j
	
	//�ő僉�C�t�𒴂��Ă���
	if (static_cast<int>(m_heartArray.size()) < arg_initMaxLife)assert(0);

	//�K�v�Ȑ������X�v���C�g�ǉ�
	for (int i = arg_initMaxLife - 1; 0 <= i; --i)	//���C�����C�t����O���ɂ��邽�ߍ~��
	{
		//�n�[�g�g�X�v���C�g
		m_contents.emplace_back(&m_heartArray[i].m_frame);
		//�n�[�g�X�v���C�g
		m_contents.emplace_back(&m_heartArray[i].m_heart);
		//�ʒu������
		m_heartArray[i].m_offsetTransform.SetPos(m_heartArray[i].m_initPos);
		//�X�P�[��������
		m_heartArray[i].m_offsetTransform.SetScale({ m_heartArray[i].m_initScale,m_heartArray[i].m_initScale });

		//�c���C�t�Ȃ�n�[�g�`����A�N�e�B�u��Ԃ�
		m_heartArray[i].m_heart.m_active = (i < arg_initRemainLife);
	}

	//�X�v���C�g�̈ʒu������
	for (auto& content : m_contents)
	{
		content->m_sprite->m_transform.SetPos(content->m_initPos);
	}

	//�_���[�W���o������
	m_damageEffect.Init();
}

void PlayerHp::Update(const float& arg_timeScale)
{
	m_damageEffect.Update(arg_timeScale);

	if (m_damageEffect.m_active)
	{
		if (m_damageEffect.m_consumeLife)
		{

		}
		else
		{
			float sizeRate = KuroMath::Ease(In, Sine,
				m_damageEffect.m_drawChangeTimer.GetTimeRate(),
				m_damageEffect.m_startHpRate,
				m_damageEffect.m_endHpRate);
			m_hpBarDamage.m_sprite->m_mesh.SetSize(CalculateHpBarSize(sizeRate));
		}

		for (auto& content : m_contents)
		{
			content->m_sprite->m_transform.SetPos(content->m_initPos + Vec2<float>(m_damageEffect.m_shake.GetOffset().x, m_damageEffect.m_shake.GetOffset().y));
		}
	}
}


void PlayerHp::OnDraw2D()
{
	for (auto& content : m_contents)
	{
		if (!content->m_active)continue;
		content->m_sprite->Draw();
	}
}

#include"ImguiApp.h"
void PlayerHp::ImguiDebug()
{
	ImGui::Begin("PlayerHp");

	auto parentPos = m_transform.GetPos();
	if (ImGui::DragFloat2("PosUI", (float*)&parentPos))m_transform.SetPos(parentPos);

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 300), ImGuiWindowFlags_NoTitleBar);
	for (auto& content : m_contents)
	{
		auto sp = content->m_sprite;
		if (ImGui::TreeNode(sp->m_name.c_str()))
		{
			auto pos = sp->m_transform.GetPos();
			if (ImGui::DragFloat2("Pos", (float*)&pos))sp->m_transform.SetPos(pos);

			ImGui::TreePop();
		}
	}

	for (int i = 0; i < static_cast<int>(m_heartArray.size()); ++i)
	{
		if (ImGui::TreeNode(("Heart - " + std::to_string(i)).c_str()))
		{
			auto pos = m_heartArray[i].m_offsetTransform.GetPos();
			if (ImGui::DragFloat2("Pos", (float*)&pos))m_heartArray[i].m_offsetTransform.SetPos(pos);

			auto scale = m_heartArray[i].m_offsetTransform.GetScale().x;
			if (ImGui::DragFloat("Scale", &scale, 0.05f))m_heartArray[i].m_offsetTransform.SetScale({ scale,scale });

			ImGui::TreePop();
		}
	}
	ImGui::EndChild();


	ImGui::End();
}

void PlayerHp::Change(int arg_amount)
{
	//�ω��O��HP���[�g
	float beforeHpRate = static_cast<float>(m_hp) / static_cast<float>(ConstParameter::Player::MAX_HP);
	//�n�[�g���������
	bool consumeLife = false;

	//HP���㉺���E�𒴂��Ȃ��悤�ɂ���
	m_hp = std::max(0, std::min(ConstParameter::Player::MAX_HP, m_hp + arg_amount));

	//HP���Ȃ��Ȃ����烉�C�t�����HP��
	if (m_hp <= 0 && m_life)
	{
		//�n�[�g�`����A�N�e�B�u��
		m_heartArray[m_life - 1].m_heart.m_active = false;
		m_life = std::max(0, m_life - 1);
		m_hp = ConstParameter::Player::MAX_HP;
		consumeLife = true;
	}

	//�ω����HP���[�g
	float afterHpRate = static_cast<float>(m_hp) / static_cast<float>(ConstParameter::Player::MAX_HP);
	//HP�o�[�T�C�Y�ύX
	m_hpBar.m_sprite->m_mesh.SetSize(CalculateHpBarSize(afterHpRate));

	//�_���[�W
	if (arg_amount < 0)
	{
		//�_���[�W���o
		m_damageEffect.Start(beforeHpRate, afterHpRate, consumeLife);
	}
}