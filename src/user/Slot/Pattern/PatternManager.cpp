#include "PatternManager.h"
#include"Pattern.h"

std::shared_ptr<Pattern>& PatternManager::GetPattern(PATTERN arg_pattern)
{
	int idx = static_cast<int>(arg_pattern);
	return m_patterns[idx];
}

PatternManager::PatternManager()
{
	GetPattern(PATTERN::NONE) = std::make_shared<NonePattern>();
}

std::shared_ptr<TextureBuffer> PatternManager::GetTex(PATTERN arg_pattern)
{
	return GetPattern(arg_pattern)->GetTex();
}

void PatternManager::Invoke(PATTERN arg_pattern)
{
	GetPattern(arg_pattern)->Invoke();
}
