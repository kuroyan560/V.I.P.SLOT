#pragma once
#include"Color.h"
//トゥーンシェーダーの個別のパラメータ
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

	//明るい部分に乗算する色
	Color m_brightMulColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	//暗い部分に乗算する色
	Color m_darkMulColor = Color(0.3f, 0.3f, 0.3f, 1.0f);
	//エッジ色
	Color m_edgeColor = Color(0.0f, 0.0f, 0.0f, 1.0f);

	ToonIndividualParameter() { *this = GetDefault(); }
	ToonIndividualParameter(
		Color arg_brightMulColor,
		Color arg_darkMulColor,
		Color arg_edgeMulColor)
		:m_brightMulColor(arg_brightMulColor), m_darkMulColor(arg_darkMulColor), m_edgeColor(arg_edgeMulColor) {}
	void ImguiDebugItem();
};