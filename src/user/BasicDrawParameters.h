#pragma once
#include"Color.h"
//�g�D�[���V�F�[�_�[�̌ʂ̃p�����[�^
class ToonIndividualParameter
{
public:
	static ToonIndividualParameter& GetDefault()
	{
		static ToonIndividualParameter defaultParams =
		{
			Color(1.0f,1.0f,1.0f,1.0f),
			Color(200,142,237,255),
			Color(0.0f,0.0f,0.0f,1.0f),
		};
		return defaultParams;
	}

	//���邢�����ɏ�Z����F
	Color m_brightMulColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	//�Â������ɏ�Z����F
	Color m_darkMulColor = Color(0.3f, 0.3f, 0.3f, 1.0f);
	//�G�b�W�F
	Color m_edgeColor = Color(0.0f, 0.0f, 0.0f, 1.0f);

	ToonIndividualParameter() { *this = GetDefault(); }
	ToonIndividualParameter(
		Color arg_brightMulColor,
		Color arg_darkMulColor,
		Color arg_edgeMulColor)
		:m_brightMulColor(arg_brightMulColor), m_darkMulColor(arg_darkMulColor), m_edgeColor(arg_edgeMulColor) {}
	void ImguiDebugItem();
};