#pragma once
#include<memory>
class Model;
class EnemyController;

class EnemyBreed
{
public:
	//種別番号（種類ごと）
	int m_typeID;
	//モデル
	std::shared_ptr<Model>m_model;
	//最大HP
	int m_maxHp;
	//所持コイン（倒したら得られるコインの数）
	int m_initCoinNum;
	//挙動制御
	std::unique_ptr<EnemyController>m_controller;
};