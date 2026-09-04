#pragma once
#include ".\Library\GameObject.h"
#include "global.h"

enum class EnemyState
{
	Patrol,
	Chase,
	Attack,
	Search,
};

class Enemy : public GameObject
{
private:
	int hImage_;//‰æ‘œID
	Point pos_;//ˆÊ’u
	DIR dir_;//ˆÚ“®•ûŒü

	EnemyState state_ = EnemyState::Patrol;

	float dir_timer = 3.0f;
	float prog_timer = 0.5f;

public:
	Enemy();
	~Enemy();
	void Update() override;
	void Draw() override;
};


