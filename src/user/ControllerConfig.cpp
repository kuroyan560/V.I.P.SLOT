#include "ControllerConfig.h"
#include"UsersInput.h"
#include<algorithm>

int ControllerConfig::GetAllocateButtonIdx(HANDLE_INPUT_TAG Tag)const
{
	//���Ɋ��蓖�Ă��Ă���{�^��������
	for (int button = 0; button < static_cast<int>(CAN_ALLOCATE_BUTTON::NUM); ++button)
	{
		if (m_allocateButton[button] == Tag)
		{
			return button;
		}
	}

	//�ǂ̃{�^���ɂ����蓖�Ă��Ă��Ȃ�
	assert(0);
	return -1;
}

void ControllerConfig::Reset()
{
	m_allocateButton[static_cast<int>(CAN_ALLOCATE_BUTTON::A)] = HANDLE_INPUT_TAG::JUMP;
	m_allocateButton[static_cast<int>(CAN_ALLOCATE_BUTTON::B)] = HANDLE_INPUT_TAG::NONE;
	m_allocateButton[static_cast<int>(CAN_ALLOCATE_BUTTON::Y)] = HANDLE_INPUT_TAG::GENERIC_ACTION;
	m_allocateButton[static_cast<int>(CAN_ALLOCATE_BUTTON::X)] = HANDLE_INPUT_TAG::ATTACK;
	m_allocateButton[static_cast<int>(CAN_ALLOCATE_BUTTON::LB)] = HANDLE_INPUT_TAG::RUSH;
	m_allocateButton[static_cast<int>(CAN_ALLOCATE_BUTTON::RB)] = HANDLE_INPUT_TAG::MARKING;
	m_allocateButton[static_cast<int>(CAN_ALLOCATE_BUTTON::LT)] = HANDLE_INPUT_TAG::GUARD_DODGE_RUN;
	m_allocateButton[static_cast<int>(CAN_ALLOCATE_BUTTON::RT)] = HANDLE_INPUT_TAG::ABILITY;
}

bool ControllerConfig::GetHandleInput(const UsersInput& Input, HANDLE_INPUT_TAG Tag)const
{
	auto button = GetAllocateButton(Tag);

	if (button == CAN_ALLOCATE_BUTTON::A)return Input.ControllerInput(m_controllerIdx, XBOX_BUTTON::A);
	else if (button == CAN_ALLOCATE_BUTTON::B)return Input.ControllerInput(m_controllerIdx, XBOX_BUTTON::B);
	else if (button == CAN_ALLOCATE_BUTTON::Y)return Input.ControllerInput(m_controllerIdx, XBOX_BUTTON::Y);
	else if (button == CAN_ALLOCATE_BUTTON::X)return Input.ControllerInput(m_controllerIdx, XBOX_BUTTON::X);
	else if (button == CAN_ALLOCATE_BUTTON::LB)return Input.ControllerInput(m_controllerIdx, XBOX_BUTTON::LB);
	else if (button == CAN_ALLOCATE_BUTTON::RB)return Input.ControllerInput(m_controllerIdx, XBOX_BUTTON::RB);
	else if (button == CAN_ALLOCATE_BUTTON::LT)return Input.ControllerInput(m_controllerIdx, XBOX_BUTTON::LT);
	else if (button == CAN_ALLOCATE_BUTTON::RT)return Input.ControllerInput(m_controllerIdx, XBOX_BUTTON::RT);

	assert(0);
	return false;
}

Vec2<float> ControllerConfig::GetMoveVec(const UsersInput& Input)const
{
	return Input.GetLeftStickVec(m_controllerIdx);
}

Vec2<float> ControllerConfig::GetCameraVec(const UsersInput& Input)const
{
	auto rStickVec = Input.GetRightStickVec(m_controllerIdx);
	if (m_camMirror.x)rStickVec.x *= -1;
	if (!m_camMirror.y)rStickVec.y *= -1;	//�㉺�͌��X�t
	return rStickVec;
}

bool ControllerConfig::GetCameraRock(const UsersInput& Input) const
{
	return Input.ControllerOnTrigger(m_controllerIdx, XBOX_BUTTON::R_STICK_PUSH);
}

Vec2<int>ControllerConfig::GetDpadTrigger(const UsersInput& Input)const
{
	Vec2<int>result = { 0,0 };
	if (Input.ControllerOnTrigger(m_controllerIdx, XBOX_BUTTON::DPAD_LEFT))result.x = -1;
	else if (Input.ControllerOnTrigger(m_controllerIdx, XBOX_BUTTON::DPAD_RIGHT))result.x = 1;
	if (Input.ControllerOnTrigger(m_controllerIdx, XBOX_BUTTON::DPAD_DOWN))result.y = -1;
	else if (Input.ControllerOnTrigger(m_controllerIdx, XBOX_BUTTON::DPAD_UP))result.y = 1;
	return result;
}


void ControllerConfig::ShakeController(UsersInput& Input, float Power, int Span)
{
	Power = std::clamp(Power, 0.0f, 1.0f);
	Input.ShakeController(m_controllerIdx, Power, Span);
}

void ControllerConfig::ReAllocateButton(HANDLE_INPUT_TAG Tag, CAN_ALLOCATE_BUTTON AllocateButton)
{
	//���蓖�Ă̓���ւ�
	m_allocateButton[GetAllocateButtonIdx(Tag)] = m_allocateButton[static_cast<int>(AllocateButton)];
	m_allocateButton[static_cast<int>(AllocateButton)] = Tag;
}

#include"imguiApp.h"
void ControllerConfig::ImguiDebug(UsersInput& Input)
{
	static std::array<std::string, static_cast<int>(HANDLE_INPUT_TAG::NUM)>TAG_NAMES =
	{
		"Attack",
		"Jump",
		"Guard / Dodge / Run",
		"Marking",
		"Rush",
		"Ability",
		"GenericAction"
	};
	static std::array<std::string, static_cast<int>(CAN_ALLOCATE_BUTTON::NUM)>ALLOCATE_BUTTON_NAME =
	{
		"A","B","Y","X","LB","RB","LT","RT"
	};

	ImGui::Begin("ControllerConfig");

	//�{�^�����蓖��
	for (int tagIdx = 0; tagIdx < static_cast<int>(HANDLE_INPUT_TAG::NUM); ++tagIdx)
	{
		//�^�O�擾
		auto tag = static_cast<HANDLE_INPUT_TAG>(tagIdx);
		//���蓖�ĂĂ���{�^���̃C���f�b�N�X�擾
		int allocateButtonIdx = GetAllocateButtonIdx(tag);

		if (ImGui::BeginCombo(TAG_NAMES[tagIdx].c_str(), ALLOCATE_BUTTON_NAME[allocateButtonIdx].c_str()))
		{
			for (int buttonIdx = 0; buttonIdx < static_cast<int>(CAN_ALLOCATE_BUTTON::NUM); ++buttonIdx)
			{
				bool isSelected = (allocateButtonIdx == buttonIdx);
				if (ImGui::Selectable(ALLOCATE_BUTTON_NAME[buttonIdx].c_str(), isSelected))
				{
					//�{�^���̊��蓖��
					ReAllocateButton(tag, static_cast<CAN_ALLOCATE_BUTTON>(buttonIdx));
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	//�J�����~���[�����O
	ImGui::Text("CameraMirror");
	ImGui::Checkbox("X", &m_camMirror.x);
	ImGui::SameLine();
	ImGui::Checkbox("Y", &m_camMirror.y);


	//���͏��\��
	static auto WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	static auto RED = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

	//�R���g���[���[�ԍ�
	ImGui::Text("ControllerIdx : %d", m_controllerIdx);
	ImGui::Separator();

	//���X�e�B�b�N���́i�ʏ�ړ��j
	ImGui::Text("Move : ");
	ImGui::SameLine();
	auto lStickVec = GetMoveVec(Input);
	ImGui::TextColored(lStickVec.IsZero() ? WHITE : RED, "{ %f , %f }", lStickVec.x, lStickVec.y);

	//�E�X�e�B�b�N���́i�J��������j
	ImGui::Text("CameraMove : ");
	ImGui::SameLine();
	auto rStickVec = GetCameraVec(Input);
	ImGui::TextColored(rStickVec.IsZero() ? WHITE : RED, "{ %f , %f }", rStickVec.x, rStickVec.y);

	//�A�r���e�B�I��
	ImGui::Text("AbilitySelect : ");
	ImGui::SameLine();
	auto dpadVec = GetDpadTrigger(Input);
	ImGui::TextColored(dpadVec.IsZero() ? WHITE : RED, "{ %d , %d }", dpadVec.x, dpadVec.y);

	//�{�^�������蓖�Ă�ꂽ����̓��͕\��
	ImGui::Separator();
	for (int tagIdx = 0; tagIdx < static_cast<int>(HANDLE_INPUT_TAG::NUM); ++tagIdx)
	{
		ImGui::Text("%s : ", TAG_NAMES[tagIdx].c_str());
		ImGui::SameLine();

		//�^�O�擾
		auto tag = static_cast<HANDLE_INPUT_TAG>(tagIdx);
		bool input = GetHandleInput(Input, tag);
		ImGui::TextColored(input ? RED : WHITE, "{ %s }", input ? "TRUE" : "FALSE");
	}

	ImGui::Separator();
	//�R���g���[���[��U��������{�^��
	static float s_power = 1.0f;
	static int s_span = 15;
	if (ImGui::Button("Shake"))
	{
		Input.ShakeController(m_controllerIdx, s_power, s_span);
	}
	ImGui::DragFloat("Power", &s_power, 0.05f, 0.0f, 1.0f);
	ImGui::DragInt("Span", &s_span, 1, 0, 120);

	ImGui::End();
}
