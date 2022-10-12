#pragma once
#include<memory>
#include<array>
#include"ConstParameters.h"
class Pattern;
class TextureBuffer;
class CoinVault;

//�G���̒�`
class PatternManager
{
private:
	using PATTERN = ConstParameter::Slot::PATTERN;

	//��`�����G���z��
	std::array<std::shared_ptr<Pattern>, static_cast<int>(PATTERN::NUM)>m_patterns;

	//�񋓎q���G���擾
	std::shared_ptr<Pattern>& GetPattern(PATTERN arg_pattern);

public:
	PatternManager(CoinVault& arg_targetVault);
	//�G���̃e�N�X�`���擾
	std::shared_ptr<TextureBuffer>GetTex(PATTERN arg_pattern);
	//�G�����w�肵�Č��ʂ𔭓�������
	void Invoke(PATTERN arg_pattern);
};