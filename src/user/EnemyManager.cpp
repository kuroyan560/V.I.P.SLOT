#include "EnemyManager.h"
#include"EnemyBreed.h"
#include"Importer.h"
#include"EnemyController.h"
#include"Enemy.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"Coins.h"
#include"TimeScale.h"

void EnemyManager::OnEnemyAppear(std::shared_ptr<Enemy>& arg_enemy, std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	//�V�K�G�̏�����
	arg_enemy->Init();

	//�R���C�_�[�̓o�^
	arg_collisionMgr.lock()->Register("Enemy", arg_enemy->m_colliders);
}

void EnemyManager::OnEnemyDead(std::shared_ptr<Enemy>& arg_enemy, std::weak_ptr<CollisionManager>arg_collisionMgr, bool arg_dropCoin)
{
	//�R���C�_�[�o�^����
	for (auto& col : arg_enemy->m_colliders)
	{
		arg_collisionMgr.lock()->Remove(col);
	}


	//�R�C���𗎂Ƃ�
	if (arg_dropCoin)
	{
		//���Ƃ����R�C���̕��o�p���[����
		const float DROP_COIN_EMIT_POWER_MIN = 0.4f;
		//���Ƃ����R�C���̕��o�p���[���
		const float DROP_COIN_EMIT_POWER_MAX = 1.4f;
		//���o�p���[X�����̋������[�g����
		const float DROP_COIN_EMIT_X_POWER_RATE_MIN = 0.0f;
		//���o�p���[X�����̋������[�g���
		const float DROP_COIN_EMIT_X_POWER_RATE_MAX = 1.0f;
		//���o�p���[Y�����̋������[�g����
		const float DROP_COIN_EMIT_Y_POWER_RATE_MIN = 0.8f;
		//���o�p���[Y�����̋������[�g���
		const float DROP_COIN_EMIT_Y_POWER_RATE_MAX = 1.0f;

		//���Ƃ����R�C���̎���
		const int DROP_COIN_LIFE_TIME = 400;

		//���o�p���[
		const float power = KuroFunc::GetRand(DROP_COIN_EMIT_POWER_MIN, DROP_COIN_EMIT_POWER_MAX);

		//���o�x�N�g��
		float vec_y = KuroFunc::GetRand(DROP_COIN_EMIT_Y_POWER_RATE_MIN, DROP_COIN_EMIT_Y_POWER_RATE_MAX);
		float vec_x = KuroFunc::GetRand(DROP_COIN_EMIT_X_POWER_RATE_MIN, DROP_COIN_EMIT_X_POWER_RATE_MAX);
		float vec_z = 0.0f;
		vec_x *= KuroFunc::GetRandPlusMinus();

		//���o�ړ���
		const Vec3<float>initMove = Vec3<float>(vec_x, vec_y, vec_z) * power;

		m_dropCoinObjManager.Add(
			arg_enemy->m_coinVault.GetNum(),
			arg_enemy->m_transform,
			DROP_COIN_LIFE_TIME,
			new DropCoinPerform(initMove));
	}
}

EnemyManager::EnemyManager()
{
	/*--- �G�̒�` ---*/

	//���ړ�����G���G
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>("Weak_Slide_Enemy - Body_Sphere", colPrimitiveArray));

		int weakSlideIdx = static_cast<int>(ENEMY_TYPE::WEAK_SLIDE);
		m_breeds[weakSlideIdx] = std::make_shared<EnemyBreed>(
			weakSlideIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "enemy_test.glb"),
			5,
			10,
			std::make_unique<EnemySlideMove>(0.1f),
			colliderArray
			);
	}

	/*--- ---*/

	//�G�C���X�^���X����
	for (int typeIdx = 0; typeIdx < static_cast<int>(ENEMY_TYPE::NUM); ++typeIdx)
	{
		for (int enemyCount = 0; enemyCount < ConstParameter::Enemy::INSTANCE_NUM_MAX[typeIdx]; ++enemyCount)
		{
			m_enemys[typeIdx].emplace_front(std::make_shared<Enemy>(m_breeds[typeIdx]));
		}
	}
}

void EnemyManager::Init(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	for (int typeIdx = 0; typeIdx < static_cast<int>(ENEMY_TYPE::NUM); ++typeIdx)
	{
		for (auto& enemy : m_aliveEnemyArray[typeIdx])
		{
			OnEnemyDead(enemy, arg_collisionMgr, false);
		}

		//�����G�l�~�[�z������
		m_aliveEnemyArray[typeIdx].clear();

		//���S�G�l�~�[�z��ɃG�l�~�[�z��R�s�[
		m_deadEnemyArray[typeIdx] = m_enemys[typeIdx];
	}

	//���Ƃ����R�C�����Z�b�g
	m_dropCoinObjManager.Init();
	m_dropCoinNumForPlayer = 0;
}

void EnemyManager::Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	m_dropCoinNumForPlayer = 0;

	for (auto& aliveEnemys : m_aliveEnemyArray)
	{
		for (auto& enemy : aliveEnemys)
		{
			enemy->Update(arg_timeScale);

			//����ł�����
			if (enemy->IsDead())
			{
				OnEnemyDead(enemy, arg_collisionMgr, enemy->IsKilled());
			}
		}
	}

	//���S���Ă����琶���G�l�~�[�z�񂩂�폜
	for (auto& aliveEnemys : m_aliveEnemyArray)
	{
		aliveEnemys.remove_if([](std::shared_ptr<Enemy>& e)
			{
				return e->IsDead();
			});
	}

	m_dropCoinNumForPlayer = m_dropCoinObjManager.Update(arg_timeScale.GetTimeScale());
}

void EnemyManager::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	for (auto& aliveEnemys : m_aliveEnemyArray)
	{
		for (auto& enemy : aliveEnemys)
		{
			enemy->Draw(arg_lightMgr, arg_cam);
		}
	}

	m_dropCoinObjManager.Draw(arg_lightMgr, arg_cam);
}

void EnemyManager::Appear(ENEMY_TYPE arg_type, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(arg_type);

	//�V�K�ǉ�����G�擾
	auto& newEnemy = m_deadEnemyArray[typeIdx].front();

	//�����G�l�~�[�z��ɒǉ�
	m_aliveEnemyArray[typeIdx].push_front(newEnemy);

	OnEnemyAppear(newEnemy, arg_collisionMgr);

	//�V�K�G�����S�G�l�~�[�z�񂩂�|�b�v
	m_deadEnemyArray[typeIdx].pop_front();
}

void EnemyManager::DropCoinPerform::OnUpdate(Coins& arg_coin, float arg_timeScale)
{
	using namespace ConstParameter::Environment;

	//���˕Ԃ莞�̈ړ��ʌ�����
	const float MOVE_REFLECT_RATE = -0.1f;

	//�ڒn����X���ړ��ʌ�����
	const float MOVE_X_DAMP_RATE_ON_GROUND = 0.9f;

	auto pos = arg_coin.m_transform.GetPos();
	pos += m_move * arg_timeScale;

	m_move.y += m_fallAccel;
	m_fallAccel += COIN_GRAVITY * arg_timeScale;

	//X�����̈ړ��ʂ͋�C��R�Ō���
	m_move.x = KuroMath::Lerp(m_move.x, 0.0f, 0.01f);

	//�����߂��i���j
	if (pos.y < FIELD_HEIGHT_HALF)
	{
		pos.y = FIELD_HEIGHT_HALF;
		m_fallAccel = 0.0f;
		m_move.x *= MOVE_X_DAMP_RATE_ON_GROUND;
		m_move.y *= MOVE_REFLECT_RATE;
	}

	//�����߂��i�X�e�[�W�[�j
	if (pos.x < -FIELD_WIDTH_HALF)
	{
		pos.x = -FIELD_WIDTH_HALF;
		m_move.x *= MOVE_REFLECT_RATE;
	}
	else if (FIELD_WIDTH_HALF < pos.x)
	{
		pos.x = FIELD_WIDTH_HALF;
		m_move.x *= MOVE_REFLECT_RATE;
	}
	arg_coin.m_transform.SetPos(pos);
}
