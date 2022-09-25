#pragma once
class CoinVault
{
	//�����R�C��
	int m_coin = 0;
public:
	//������
	void Init(int arg_coinNum = 0) { m_coin = arg_coinNum; }

	//�R�C���ǉ�
	void Add(int arg_coinNum)
	{
		m_coin += arg_coinNum;
	}

	//�S�z�n��
	void Pass(CoinVault& arg_dest)
	{
		arg_dest.Add(this->m_coin);
		this->m_coin = 0;
	}
	//���z���w�肵�ēn���i����Ȃ���Ή��������Afalse��Ԃ��j
	bool Pass(CoinVault& arg_dest, int arg_coinNum)
	{
		if (m_coin < arg_coinNum)return false;

		arg_dest.Add(arg_coinNum);
		this->Add(-arg_coinNum);

		return true;
	}
};

