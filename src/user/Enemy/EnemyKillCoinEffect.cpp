#include "EnemyKillCoinEffect.h"
#include"Angle.h"
#include"Importer.h"

EnemyKillCoinEffect::Info::Info(const EnemyKillCoinEffect& arg_parent, Vec3<float> arg_playerPos, int arg_coinNum)
	:m_coinNum(arg_coinNum) 
{
	//�o���ʒu�I�t�Z�b�g
	const Vec3<float>APPEAR_POS_OFFSET = { 0.0f,1.0f,0.0f };
	arg_playerPos += APPEAR_POS_OFFSET;

	const std::array<int, STATUS::NUM>TIME = { 60,45,60 };
	for (int statusIdx = 0; statusIdx < STATUS::NUM; ++statusIdx)
	{
		m_timer[statusIdx].Reset(TIME[statusIdx]);
	}

	//�R�C���̌����擾
	int digit = KuroFunc::GetDigit(arg_coinNum);

	//�\��������
	int modelNum = digit + 1;
	m_numbers.resize(digit + 1);

	//�O�Ԗڂ́u�{�v
	m_numbers[0].m_numIdx = arg_parent.PLUS_IDX;
	//�e���̐����擾
	for (int i = 1; i < modelNum; ++i)
	{
		//�����擾�i�{�̃C���f�b�N�X�� -1�j
		int d = i - 1;
		//�w�茅�̐����擾
		m_numbers[i].m_numIdx = KuroFunc::GetSpecifiedDigitNum(arg_coinNum, d, true);
	}

	//����
	const float LETTER_SPACE = 1.8f;

	//���[�̃I�t�Z�b�g�ʒu�����߂�
	float left = -LETTER_SPACE * std::floor(modelNum / 2.0f);
	if (modelNum % 2 == 0)left -= LETTER_SPACE / 2.0f;	//����������

	//�g�����X�t�H�[���ݒ�
	for (int i = 0; i < modelNum; ++i)
	{
		//�e�g�����X�t�H�[���o�^
		m_numbers[i].m_transform.SetParent(&m_transform);
		//�I�t�Z�b�g�K�p
		m_numbers[i].m_transform.SetPos({ left + LETTER_SPACE * i,0.0f,0.0f });
	}

	//�������W
	m_transform.SetPos(arg_playerPos);
	m_startPos = arg_playerPos;
}

void EnemyKillCoinEffect::Info::Update(float arg_timeScale)
{
	//�����\�L��Y���㏸��
	const std::array<float, STATUS::NUM> FLOAT_HEIGHT = { 2.0f,0.0f,2.0f };
	//�C�[�W���O�ω��^�C�v
	const std::array<EASE_CHANGE_TYPE, STATUS::NUM> EASE_CHANGE_TYPE_ARRAY = { Out,In,In };
	//�C�[�W���O���
	const std::array<EASING_TYPE, STATUS::NUM> EASING_TYPE_ARRAY = { Quint,Liner,Back };
	//��]��
	const std::array<Angle, STATUS::NUM> SPIN_ANGLE_ARRAY = { Angle(360),0,Angle(360) };


	//���݂̍��W�擾
	auto pos = m_transform.GetPos();

	//�^�C�}�[�v��
	if (m_timer[m_status].UpdateTimer(arg_timeScale))
	{
		//�X�e�[�^�X�̐i�s
		if (m_status < NUM)
		{
			m_status = STATUS(m_status + 1);
			m_startPos = m_transform.GetPos();
			m_startAngleY = m_angleY;
		}
	}
	//�X�V
	else
	{
		//�o�ߎ��ԃ��[�g�擾
		float rate = m_timer[m_status].GetTimeRate();

		//�㏸
		pos.y = KuroMath::Ease(EASE_CHANGE_TYPE_ARRAY[m_status], EASING_TYPE_ARRAY[m_status], rate, m_startPos.y, m_startPos.y + FLOAT_HEIGHT[m_status]);

		//��]
		m_angleY = KuroMath::Ease(EASE_CHANGE_TYPE_ARRAY[m_status], EASING_TYPE_ARRAY[m_status], rate, m_startAngleY, m_startAngleY + SPIN_ANGLE_ARRAY[m_status]);
	}

	//�ύX��̍��W�K�p
	m_transform.SetPos(pos);
	//�ύX��̉�]�ʂ��e�I�t�Z�b�g�g�����X�t�H�[���ɓK�p
	for (auto& num : m_numbers)
	{
		num.m_transform.SetRotate(0, m_angleY, 0);
	}
}

#include"DrawFunc3D.h"
void EnemyKillCoinEffect::Info::Draw(const EnemyKillCoinEffect& arg_parent, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
	for (auto& number : m_numbers)
	{
		DrawFunc3D::DrawNonShadingModel(
			arg_parent.m_numModels[number.m_numIdx], 
			number.m_transform, 
			*arg_cam.lock(),
			1.0f, 
			nullptr,
			AlphaBlendMode_None);
	}
}

EnemyKillCoinEffect::EnemyKillCoinEffect()
{
	//���f���ǂݍ���
	std::string dir = "resource/user/model/num/";
	std::array<std::string, MODEL_NUM>modelName =
	{
		"zero","one","two","three","four","five","six","seven","eight","nine","plus"
	};
	for (int modelIdx = 0; modelIdx < MODEL_NUM; ++modelIdx)
	{
		m_numModels[modelIdx] = Importer::Instance()->LoadModel(dir, modelName[modelIdx] + ".glb");
	}
}

void EnemyKillCoinEffect::Init()
{
	m_infoArray.clear();
}

void EnemyKillCoinEffect::Update(float arg_timeScale)
{
	for (auto& info : m_infoArray)
	{
		info.Update(arg_timeScale);
	}

	//����ł�����폜
	m_infoArray.remove_if([](Info& info)
		{
			return info.IsEnd();
		});
}

void EnemyKillCoinEffect::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	for (auto& info : m_infoArray)
	{
		info.Draw(*this, arg_lightMgr, arg_cam);
	}
}

void EnemyKillCoinEffect::Emit(Vec3<float> arg_emitPos, int arg_coinNum)
{
	m_infoArray.emplace_front(*this, arg_emitPos, arg_coinNum);
}
