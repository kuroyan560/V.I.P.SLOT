#pragma once
#include<string>
#include"KuroFunc.h"
class ImportHeader
{
public:
	const std::string dir;
	const std::string fileName;
	const std::string ext;

	ImportHeader(const std::string& Dir, const std::string& FileName)
		:dir(Dir), fileName(FileName), ext("." + KuroFunc::GetExtension(fileName)) {}

	std::string GetPath()const { return dir + fileName; }
	std::string GetModelName()const	//���f�����i�g���q�Ȃ��j
	{
		auto modelName = fileName;
		return modelName.erase(modelName.size() - ext.size());
	}
};