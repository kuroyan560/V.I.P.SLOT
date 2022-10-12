#include "PatternManager.h"
#include"Pattern.h"

std::shared_ptr<Pattern>& PatternManager::GetPattern(PATTERN arg_pattern)
{
	int idx = static_cast<int>(arg_pattern);
	return m_patterns[idx];
}

PatternManager::PatternManager(CoinVault& arg_targetVault)
{
	GetPattern(PATTERN::DOUBLE) = std::make_shared<MultiplyPattern>(2.0f, &arg_targetVault);
	GetPattern(PATTERN::TRIPLE) = std::make_shared<MultiplyPattern>(3.0f, &arg_targetVault);
}

std::shared_ptr<TextureBuffer> PatternManager::GetTex(PATTERN arg_pattern)
{
	return GetPattern(arg_pattern)->GetTex();
}

void PatternManager::Invoke(PATTERN arg_pattern)
{
	GetPattern(arg_pattern)->Invoke();
}
