#include"Ability.h"
#include"D3D12App.h"

Ability::Ability(const std::string& TexName)
{
	static const std::string DIR = "resource/user/img/abilityIcon/";
	m_iconTex = D3D12App::Instance()->GenerateTextureBuffer(DIR + TexName);
}