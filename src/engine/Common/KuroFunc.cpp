#include"KuroFunc.h"
#include<windows.h>
#include <cassert>

#include<fstream>
#include<iostream>

bool operator!=(const Matrix& lhs, const Matrix& rhs)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (lhs.r[i].m128_f32[j] != rhs.r[i].m128_f32[j])return true;
		}
	}
	return false;
}

bool operator==(const Matrix& lhs, const Matrix& rhs)
{
	return !(lhs != rhs);
}

bool KuroFunc::LoadData(FILE* Fp, void* Data, const size_t& Size, const int& ElementNum)
{
	return 1 <= fread(Data, Size, ElementNum, Fp);
}

bool KuroFunc::SaveData(FILE* Fp, const void* Data, const size_t& Size, const int& ElementNum)
{
	return 1 <= fwrite(Data, Size, ElementNum, Fp);
}

std::wstring KuroFunc::GetWideStrFromStr(const std::string& Str)
{
	auto num1 = MultiByteToWideChar(
		CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		Str.c_str(), -1, nullptr, 0);

	std::wstring wstr;
	wstr.resize(num1);

	auto num2 = MultiByteToWideChar(
		CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		Str.c_str(), -1, &wstr[0], num1);

	assert(num1 == num2);

	return wstr;
}

std::wstring KuroFunc::GetWideStrFromStr(const char* Str)
{
	return GetWideStrFromStr(std::string(Str));
}

std::string KuroFunc::GetExtension(const std::string& Path)
{
	int idx = static_cast<int>(Path.rfind('.'));
	return Path.substr(idx + 1, Path.length() - idx - 1);
}

void KuroFunc::GetDivideStr(const std::string& Str, std::string* Dir, std::string* FileName)
{
	int idx = static_cast<int>(Str.rfind('/'));
	std::string fileName = Str.substr(idx + 1, Str.length() - idx - 1);
	if (FileName != nullptr)
	{
		*FileName = fileName;
	}
	if (Dir != nullptr)
	{
		*Dir = Str;
		for (int i = 0; i < fileName.size(); ++i)Dir->pop_back();
	}
}


Vec2<float> KuroFunc::ConvertWorldToScreen(Vec3<float> WorldPos, const Matrix& ViewMat, const Matrix& ProjMat, const Vec2<float>& WinSize)
{
	XMVECTOR pos = XMLoadFloat3((XMFLOAT3*)&WorldPos);
	//�r���[�s��K��
	pos = XMVector3Transform(pos, ViewMat);
	//�v���W�F�N�V�����s��K��
	pos = XMVector3TransformCoord(pos, ProjMat);

	//�r���[�|�[�g�s��
	Matrix viewPortMat = XMMatrixIdentity();
	viewPortMat.r[0].m128_f32[0] = WinSize.x / 2.0f;
	viewPortMat.r[1].m128_f32[1] = -WinSize.y / 2.0f;
	viewPortMat.r[3].m128_f32[0] = WinSize.x / 2.0f;
	viewPortMat.r[3].m128_f32[1] = WinSize.y / 2.0f;
	//�r���[�|�[�g�s��K��
	pos = XMVector3Transform(pos, viewPortMat);

	Vec2<float> result;
	result.x = pos.m128_f32[0];
	result.y = pos.m128_f32[1];
	//result.z = pos.m128_f32[2];

	return result;
}

Vec3<float> KuroFunc::ConvertScreenToWorld(Vec2<float> ScreenPos, float Z, const Matrix& ViewMat, const Matrix& ProjMat, const Vec2<int>& WinSize)
{
	Matrix invView, invPrj, invViewPort;
	invView = XMMatrixInverse(nullptr, ViewMat);
	invPrj = XMMatrixInverse(nullptr, ProjMat);

	invViewPort = XMMatrixIdentity();
	invViewPort.r[0].m128_f32[0] = (float)WinSize.x / 2.0f;
	invViewPort.r[1].m128_f32[1] = (float)-WinSize.y / 2.0f;
	invViewPort.r[3].m128_f32[0] = (float)WinSize.x / 2.0f;
	invViewPort.r[3].m128_f32[1] = (float)WinSize.y / 2.0f;
	invViewPort = XMMatrixInverse(nullptr, invViewPort);

	XMVECTOR pos;
	auto screenPosXMFLOAT3 = XMFLOAT3(ScreenPos.x, ScreenPos.y, Z);
	pos = XMLoadFloat3(&screenPosXMFLOAT3);
	/*
		�����Ǝ���05_03.�ˉe�ϊ��@�������e�ϊ����Q��
		z(0.0f ~ 1.0f)
		0.0f �� proj ��near��
		1.0f �� far�ʂ̈ʒu
	*/
	pos = XMVector3Transform(pos, invViewPort);
	pos = XMVector3TransformCoord(pos, invPrj);
	//Coord��w���Z���s���Ă����
	/*
		w���Z�����邱�Ƃɂ���ĉ����̂��̂����ڂ܂��Ă����A
		��ʓ��Ɏ��܂�悤�ɂ��Ă����
		Coord���Ȃ��ƃX�N���[�����猩�؂ꂽ�A�݂����ɂȂ�
	*/
	pos = XMVector3Transform(pos, invView);

	Vec3<float> result;
	result.x = pos.m128_f32[0];
	result.y = pos.m128_f32[1];
	result.z = pos.m128_f32[2];
	//result.x = base.r[0].m128_f32[0];
	//result.y = base.r[0].m128_f32[1];

	return result;
}

bool KuroFunc::InScreen(Vec2<float> ScreenPos, Vec2<float> WinSize)
{
	if (ScreenPos.x < 0)return false;
	if (WinSize.x < ScreenPos.x)return false;
	if (ScreenPos.y < 0)return false;
	if (WinSize.y < ScreenPos.y)return false;
	return true;
}

bool KuroFunc::InScreen(Vec3<float> WorldPos, const Matrix& ViewMat, const Matrix& ProjMat, Vec2<float> WinSize)
{
	Vec2<float> screenPos = KuroFunc::ConvertWorldToScreen(WorldPos, ViewMat, ProjMat, WinSize);
	return InScreen(screenPos, WinSize);
}

bool KuroFunc::ExistFile(const std::string FilePass)
{
	std::ifstream ifs(FilePass);
	return ifs.is_open();
}

float KuroFunc::GetYFromXOnBezier(float x, const Vec2<float>& a, const Vec2<float>& b, uint8_t n)
{
	if (a.x == a.y && b.x == b.y)
	{
		return x;		//�v�Z�s�v
	}

	float t = x;
	const float k0 = 1 + 3 * a.x - 3 * b.x;	//t^3�̌W��
	const float k1 = 3 * b.x - 6 * a.x;	//t^2�̌W��
	const float k2 = 3 * a.x;	//t�̌W��

	constexpr float epsilon = 0.0005f;	//�덷���e�͈�

	//t���ߎ��ŋ��߂�
	for (int i = 0; i < n; i++)
	{
		//f(t)�����߂�
		auto ft = k0 * t * t * t + k1 * t * t + k2 * t - x;
		//�������ʂ��O�ɋ߂��i�덷�͈͓̔��j�Ȃ�ł��؂�
		if (ft <= epsilon && ft >= -epsilon)
		{
			break;
		}
		t -= ft / 2;	//����
	}

	//���߂���t�͊��ɋ��߂Ă���̂�y���v�Z
	auto r = 1 - t;
	return t * t * t + 3 * t * t * r * b.y + 3 * t * r * r * a.y;
}

//�K������߂�
int GetFactorial(int num)
{
	int result = 1;
	for (int i = num; 0 < i; --i)
	{
		result *= i;
	}
	return result;
}

//n�̒�����k��I�ԂƂ��̑g�ݍ��킹�̐�
int GetSelectPatternNum(int n, int k)
{
	if (k == 0)return 1;

	//���q
	int upper = GetFactorial(n);
	//����
	int a = GetFactorial(k);
	int b = GetFactorial(n - k);
	int lower = a * b;

	return upper / lower;
}

float KuroFunc::GetBezierFromControlPoint(float t, int ControlPointNum, float* ControlPointArray)
{
	double result = 0;
	for (int i = 0; i < ControlPointNum; ++i)
	{
		int coefficient = GetSelectPatternNum(ControlPointNum, i);	//�񍀌W��
		double a = pow(t, i);	// t �� i ��
		double b = pow((1 - t), (ControlPointNum - i));	// (1-t) �� n-i ��
		result += coefficient * a * b * ControlPointArray[i];		//����_�̏d�ݕt�������ĉ��Z(��)
	}
	return static_cast<float>(result);
}

int KuroFunc::GetRand(int Min, int Max)
{
	int result = (Max - Min + 1) * rand() / RAND_MAX + Min;
	return result;
}

int KuroFunc::GetRand(int Max)
{
	return GetRand(0, Max);
}

float KuroFunc::GetRand(float Min, float Max)
{
	double result = (Max - Min) * ((double)rand() / RAND_MAX) + Min;
	return static_cast<float>(result);
}

Vec2<float> KuroFunc::GetRand(Vec2<float> Min, Vec2<float> Max)
{
	return Vec2<float>(GetRand(Min.x, Max.x), GetRand(Min.y, Max.y));
}

Vec3<float> KuroFunc::GetRand(Vec3<float> Min, Vec3<float> Max)
{
	return Vec3<float>(GetRand(Min.x, Max.x), GetRand(Min.y, Max.y), GetRand(Min.z, Max.z));
}

float KuroFunc::GetRand(float Max)
{
	return GetRand(0.0f, Max);
}

Vec2<float> KuroFunc::GetRand(Vec2<float> Max)
{
	return Vec2<float>(GetRand(Max.x), GetRand(Max.y));
}

Vec3<float> KuroFunc::GetRand(Vec3<float> Max)
{
	return Vec3<float>(GetRand(Max.x), GetRand(Max.y), GetRand(Max.z));
}

int KuroFunc::GetRandPlusMinus()
{
	int r = GetRand(0, 1);
	if (r)return 1;
	else return -1;
}

int KuroFunc::GetRandFromCenter(int CenterNum, int Range)
{
	return CenterNum + GetRand(0, Range) * GetRandPlusMinus();
}


int KuroFunc::GetSpecifiedDigitNum(int From, int Digit)
{
	int result = 0;
	for (int i = Digit; 0 <= i; i--)
	{
		result = From % 10;				//6-i���ڂ̐������L�^
		From /= 10;					//�������炷
	}
	return result;
}

int KuroFunc::GetDigit(int Num)
{
	if (Num == 0)return 1;

	int result = 0;
	while (Num != 0)
	{
		Num /= 10;
		result++;
	}
	return result;
}

int KuroFunc::GetNumSign(int Num)
{
	if (Num == 0)return 0;
	if (Num < 0)return -1;
	return 1;
}

int KuroFunc::GetNumSign(float Num)
{
	if (Num == 0.0f)return 0;
	if (Num < 0.0f)return -1;
	return 1;
}

void KuroFunc::LoadData(FILE* Fp, std::string DataName, void* Data, size_t Size, int ElementNum)
{
	if (fread(Data, Size, ElementNum, Fp) < 1)
	{
		printf("�@%s�̓ǂݍ��݂Ɏ��s\n", DataName.c_str());
		assert(0);
	}
}

void KuroFunc::SaveData(FILE* Fp, std::string DataName, const void* Data, size_t Size, int ElementNum)
{
	if (fwrite(Data, Size, ElementNum, Fp) < 1)
	{
		printf("�@%s�̏������݂Ɏ��s\n", DataName.c_str());
		assert(0);
	}
}

