#pragma once
#include"Color.h"
//トゥーンシェーダーの個別のパラメータ
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

	//塗りつぶし色
	Color m_fillColor = Color(1.0f, 1.0f, 1.0f, 0.0f);
	//明るい部分に乗算する色
	Color m_brightMulColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	//暗い部分に乗算する色
	Color m_darkMulColor = Color(0.3f, 0.3f, 0.3f, 1.0f);
	//リムライト部分の補正色（アルファ：補正率）
	//【補正率】 0 ：本来の色、1 : 補正色
	Color m_limBrightColor = Color(1.0f, 1.0f, 1.0f, 0.0f);
	//エッジ色
	Color m_edgeColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	
	IndividualDrawParameter() { *this = GetDefault(); }
	IndividualDrawParameter(
		Color arg_brightMulColor,
		Color arg_darkMulColor,
		Color arg_edgeMulColor)
		:m_brightMulColor(arg_brightMulColor), m_darkMulColor(arg_darkMulColor), m_edgeColor(arg_edgeMulColor) {}
	void ImguiDebugItem();
};