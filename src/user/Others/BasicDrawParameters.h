#pragma once
#include"Color.h"
//�g�D�[���V�F�[�_�[�̌ʂ̃p�����[�^
class IndividualDrawParameter
{
public:
	static IndividualDrawParameter& GetDefault()
	{
		static IndividualDrawParameter defaultParams =
		{
			Color(1.0f,1.0f,1.0f,1.0f),
			Color(200,142,237,255),
			Color(0.0f,0.0f,0.0f,1.0f),
		};
		return defaultParams;
	}

	//�h��Ԃ��F
	Color m_fillColor = Color(1.0f, 1.0f, 1.0f, 0.0f);
	//���邢�����ɏ�Z����F
	Color m_brightMulColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	//�Â������ɏ�Z����F
	Color m_darkMulColor = Color(0.3f, 0.3f, 0.3f, 1.0f);
	//�������C�g�����̕␳�F�i�A���t�@�F�␳���j
	//�y�␳���z 0 �F�{���̐F�A1 : �␳�F
	Color m_limBrightColor = Color(1.0f, 1.0f, 1.0f, 0.0f);
	//�G�b�W�F
	Color m_edgeColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	
	IndividualDrawParameter() { *this = GetDefault(); }
	IndividualDrawParameter(
		Color arg_brightMulColor,
		Color arg_darkMulColor,
		Color arg_edgeMulColor)
		:m_brightMulColor(arg_brightMulColor), m_darkMulColor(arg_darkMulColor), m_edgeColor(arg_edgeMulColor) {}
	void ImguiDebugItem();
};