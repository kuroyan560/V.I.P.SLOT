#include "DebugCamera.h"
#include"WinApp.h"
#include"UsersInput.h"

void DebugCamera::MoveXMVector(const XMVECTOR& MoveVector)
{
	auto pos = m_cam->GetPos();
	auto target = m_cam->GetTarget();

	Vec3<float>move(MoveVector.m128_f32[0], MoveVector.m128_f32[1], MoveVector.m128_f32[2]);
	pos += move;
	target += move;

	m_cam->SetPos(pos);
	m_cam->SetTarget(target);
}

DebugCamera::DebugCamera()
{
	m_cam = std::make_shared<Camera>("DebugCamera");
	m_dist = m_cam->GetPos().Distance(m_cam->GetTarget());

	//��ʃT�C�Y�ɑ΂��鑊�ΓI�ȃX�P�[���ɒ���
	m_scale.x = 1.0f / (float)WinApp::Instance()->GetExpandWinSize().x;
	m_scale.y = 1.0f / (float)WinApp::Instance()->GetExpandWinSize().y;
}

void DebugCamera::Init(const Vec3<float>& InitPos, const Vec3<float>& Target)
{
	m_cam->SetPos(InitPos);
	m_cam->SetTarget(Target);

	m_dist = InitPos.Distance(Target);
}

void DebugCamera::Move()
{
	bool moveDirty = false;
	float angleX = 0;
	float angleY = 0;

	//�}�E�X�̓��͂��擾
	UsersInput::MouseMove mouseMove = UsersInput::Instance()->GetMouseMove();

	//�}�E�X���N���b�N�ŃJ������]
	if (UsersInput::Instance()->MouseInput(MOUSE_BUTTON::RIGHT))
	{
		float dy = mouseMove.m_inputX * m_scale.y;
		float dx = mouseMove.m_inputY * m_scale.x;

		angleX = -dx * XM_PI;
		angleY = -dy * XM_PI;
		moveDirty = true;
	}

	//�}�E�X���N���b�N�ŃJ�������s�ړ�
	if (UsersInput::Instance()->MouseInput(MOUSE_BUTTON::CENTER))
	{
		float dx = mouseMove.m_inputX / 100.0f;
		float dy = mouseMove.m_inputY / 100.0f;

		XMVECTOR move = { -dx,+dy,0,0 };
		move = XMVector3Transform(move, m_matRot);

		MoveXMVector(move);
		moveDirty = true;
	}

	//�z�C�[�����͂ŋ�����ύX
	if (mouseMove.m_inputZ != 0)
	{
		m_dist -= mouseMove.m_inputZ / 100.0f;
		m_dist = std::max(m_dist, 1.0f);
		moveDirty = true;
	}

	if (moveDirty)
	{
		//�ǉ���]���̉�]�s��𐶐�
		XMMATRIX matRotNew = XMMatrixIdentity();
		matRotNew *= XMMatrixRotationX(-angleX);
		matRotNew *= XMMatrixRotationY(-angleY);
		// ����]�s���ݐς��Ă����ƁA�덷�ŃX�P�[�����O��������댯�������
		// �N�H�[�^�j�I�����g�p��������]�܂���
		m_matRot = matRotNew * m_matRot;

		// �����_���王�_�ւ̃x�N�g���ƁA������x�N�g��
		XMVECTOR vTargetEye = { 0.0f, 0.0f, -m_dist, 1.0f };
		XMVECTOR vUp = { 0.0f, 1.0f, 0.0f, 0.0f };

		// �x�N�g������]
		vTargetEye = XMVector3Transform(vTargetEye, m_matRot);
		vUp = XMVector3Transform(vUp, m_matRot);

		// �����_���炸�炵���ʒu�Ɏ��_���W������
		Vec3<float>target = m_cam->GetTarget();
		m_cam->SetPos({ target.x + vTargetEye.m128_f32[0], target.y + vTargetEye.m128_f32[1], target.z + vTargetEye.m128_f32[2] });
		m_cam->SetUp({ vUp.m128_f32[0], vUp.m128_f32[1], vUp.m128_f32[2] });
	}
}
