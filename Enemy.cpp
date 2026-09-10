#include "Enemy.h"
#include "time.h"
#include "Stage.h"
#include "Player.h"

namespace
{
	const int ENEMY_SIZE = 48; //敵のサイズ 32*32
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;
}

Enemy::Enemy()
	: GameObject() 
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	dir_ = INIT_ENEMY_DIR;

	search_timer = 5.0f;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	switch (state_)
	{
	case EnemyState::Patrol:
	{
		Point playerPos = FindGameObject<Player>()->GetPlayerPos();

		int centerX = pos_.x + ENEMY_DRAW_SIZE / 2;
		int centerY = pos_.y + ENEMY_DRAW_SIZE / 2;

		int dx = (playerPos.x + CHA_SIZE / 2) - centerX;
		int dy = (playerPos.y + CHA_SIZE / 2) - centerY;

		const int VIEW_LENGTH = CHA_SIZE * 5;
		const int VIEW_WIDTH = CHA_SIZE * 5;

		bool inView = false;

		switch (dir_)
		{
		case UP:
			inView = dy < 0 && -dy <= VIEW_LENGTH && abs(dx) <= VIEW_WIDTH / 2;
			break;

		case DOWN:
			inView = dy > 0 && dy <= VIEW_LENGTH && abs(dx) <= VIEW_WIDTH / 2;
			break;

		case LEFT:
			inView = dx < 0 && -dx <= VIEW_LENGTH && abs(dy) <= VIEW_WIDTH / 2;
			break;

		case RIGHT:
			inView = dx > 0 && dx <= VIEW_LENGTH && abs(dy) <= VIEW_WIDTH / 2;
			break;
		}

		if (inView)
		{
			state_ = EnemyState::Chase;
			break;
		}

		prog_timer -= Time::DeltaTime();

		if (prog_timer < 0.0f)
		{
			Point newPos = pos_;

			switch (dir_)
			{
			case UP:
				newPos.y -= ENEMY_DRAW_SIZE;

				if (newPos.y <= 1)
				{
					dir_ = RIGHT;
				}
				break;

			case RIGHT:
				newPos.x += ENEMY_DRAW_SIZE;

				if (newPos.x >= (STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE)
				{
					dir_ = DOWN;
				}
				break;

			case DOWN:
				newPos.y += ENEMY_DRAW_SIZE;

				if (newPos.y >= (STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE)
				{
					dir_ = LEFT;
				}
				break;

			case LEFT:
				newPos.x -= ENEMY_DRAW_SIZE;

				if (newPos.x <= 1)
				{
					dir_ = UP;
				}
				break;
			}

			// ステージ外に出ない場合だけ移動
			if (!(newPos.x < 1 ||
				newPos.x >(STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE ||
				newPos.y < 1 ||
				newPos.y >(STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE))
			{
				pos_ = newPos;
			}

			prog_timer = 0.5f + prog_timer;
		}

		break;
	}
	case EnemyState::Chase:
	{
		Point playerPos = FindGameObject<Player>()->GetPlayerPos();

		int centerX = pos_.x + ENEMY_DRAW_SIZE / 2;
		int centerY = pos_.y + ENEMY_DRAW_SIZE / 2;

		int dx = (playerPos.x + CHA_SIZE / 2) - centerX;
		int dy = (playerPos.y + CHA_SIZE / 2) - centerY;

		const int VIEW_LENGTH = CHA_SIZE * 5;
		const int VIEW_WIDTH = CHA_SIZE * 5;

		bool inView = false;

		switch (dir_)
		{
		case UP:
			inView = dy < 0 && -dy <= VIEW_LENGTH && abs(dx) <= VIEW_WIDTH / 2;
			break;

		case DOWN:
			inView = dy > 0 && dy <= VIEW_LENGTH && abs(dx) <= VIEW_WIDTH / 2;
			break;

		case LEFT:
			inView = dx < 0 && -dx <= VIEW_LENGTH && abs(dy) <= VIEW_WIDTH / 2;
			break;

		case RIGHT:
			inView = dx > 0 && dx <= VIEW_LENGTH && abs(dy) <= VIEW_WIDTH / 2;
			break;
		}

		const int ATTACK_DISTANCE = CHA_SIZE * 2;

		// プレイヤーを発見して2マス以内ならAttackへ
		if (inView &&
			abs(dx) <= ATTACK_DISTANCE && abs(dy) <= ATTACK_DISTANCE)
		{
			state_ = EnemyState::Attack;
			attack_timer = 0.5f;
			attackMoveTimer = 0.0f;
			break;
		}

		// プレイヤーを見失ったらSearchへ
		if (!inView)
		{
			state_ = EnemyState::Search;
			break;
		}

		prog_timer -= Time::DeltaTime();


		if (prog_timer < 0.0f)
		{
			if (abs(dx) > abs(dy))
			{
				if (dx > 0)
				{
					dir_ = RIGHT;
					pos_.x += ENEMY_DRAW_SIZE;
				}
				else
				{
					dir_ = LEFT;
					pos_.x -= ENEMY_DRAW_SIZE;
				}
			}
			else
			{
				if (dy > 0)
				{
					dir_ = DOWN;
					pos_.y += ENEMY_DRAW_SIZE;
				}
				else
				{
					dir_ = UP;
					pos_.y -= ENEMY_DRAW_SIZE;
				}
			}

			prog_timer = 0.2f + prog_timer;
		}
		break;
	}
	case EnemyState::Attack:
	{
		Point playerPos = FindGameObject<Player>()->GetPlayerPos();

		float dt = Time::DeltaTime();
		attack_timer -= dt;
		hit_timer -= dt;
		attackMoveTimer -= dt;

		if (attack_timer <= 0.0f)
		{
			isHit = true;
			hit_timer = 0.5f;
			// 攻撃
			attack_timer = 1.0f;
		}

		if (hit_timer <= 0.0f)
		{
			isHit = false;
		}

		int centerX = pos_.x + ENEMY_DRAW_SIZE / 2;
		int centerY = pos_.y + ENEMY_DRAW_SIZE / 2;

		int dx = (playerPos.x + CHA_SIZE / 2) - centerX;
		int dy = (playerPos.y + CHA_SIZE / 2) - centerY;

		const int VIEW_LENGTH = CHA_SIZE * 5;
		const int VIEW_WIDTH = CHA_SIZE * 5;

		bool inView = false;

		switch (dir_)
		{
		case UP:
			inView = dy < 0 && -dy <= VIEW_LENGTH && abs(dx) <= VIEW_WIDTH / 2;
			break;

		case DOWN:
			inView = dy > 0 && dy <= VIEW_LENGTH && abs(dx) <= VIEW_WIDTH / 2;
			break;

		case LEFT:
			inView = dx < 0 && -dx <= VIEW_LENGTH && abs(dy) <= VIEW_WIDTH / 2;
			break;

		case RIGHT:
			inView = dx > 0 && dx <= VIEW_LENGTH && abs(dy) <= VIEW_WIDTH / 2;
			break;
		}

		if (!inView)
		{
			state_ = EnemyState::Search;
			isHit = false;
			break;
		}

		const int ATTACK_DISTANCE = CHA_SIZE * 2;

		// 攻撃距離から逃げられたらSearch
		if (abs(dx) > ATTACK_DISTANCE || abs(dy) > ATTACK_DISTANCE)
		{
			state_ = EnemyState::Search;
			isHit = false;
			break;
		}

		// 0.2秒ごとに近づく
		if (attackMoveTimer <= 0.0f)
		{
			if (abs(dx) > abs(dy))
			{
				if (dx > 0)
				{
					dir_ = RIGHT;

					if (abs(dx) > CHA_SIZE)
					{
						pos_.x += ENEMY_DRAW_SIZE;
					}
				}
				else
				{
					dir_ = LEFT;

					if (abs(dx) > CHA_SIZE)
					{
						pos_.x -= ENEMY_DRAW_SIZE;
					}
				}
			}
			else
			{
				if (dy > 0)
				{
					dir_ = DOWN;

					if (abs(dy) > CHA_SIZE)
					{
						pos_.y += ENEMY_DRAW_SIZE;
					}
				}
				else
				{
					dir_ = UP;

					if (abs(dy) > CHA_SIZE)
					{
						pos_.y -= ENEMY_DRAW_SIZE;
					}
				}
			}

			attackMoveTimer = 0.2f;
		}

		break;
	}
	case EnemyState::Search:
		//GetRand(数値)
//3秒に1回向きをランダムに変える
		float dt = Time::DeltaTime();

		search_timer -= dt;

		if (search_timer <= 0.0f)
		{
			state_ = EnemyState::Patrol;
			search_timer = 5.0f;
			dir_ = (DIR)(GetRand(3));
			break;
		}

		Point playerPos = FindGameObject<Player>()->GetPlayerPos();

		int centerX = pos_.x + ENEMY_DRAW_SIZE / 2;
		int centerY = pos_.y + ENEMY_DRAW_SIZE / 2;

		int dx = (playerPos.x + CHA_SIZE / 2) - centerX;
		int dy = (playerPos.y + CHA_SIZE / 2) - centerY;

		const int VIEW_LENGTH = CHA_SIZE * 5;
		const int VIEW_WIDTH = CHA_SIZE * 5;

		bool inView = false;

		switch (dir_)
		{
		case UP:
			inView = dy < 0 && -dy <= VIEW_LENGTH && abs(dx) <= VIEW_WIDTH / 2;
			break;

		case DOWN:
			inView = dy > 0 && dy <= VIEW_LENGTH && abs(dx) <= VIEW_WIDTH / 2;
			break;

		case LEFT:
			inView = dx < 0 && -dx <= VIEW_LENGTH && abs(dy) <= VIEW_WIDTH / 2;
			break;

		case RIGHT:
			inView = dx > 0 && dx <= VIEW_LENGTH && abs(dy) <= VIEW_WIDTH / 2;
			break;
		}

		const int ATTACK_DISTANCE = CHA_SIZE * 2;
		// プレイヤーを再発見して2マス以内ならAttackへ
		if (inView && abs(dx) <= ATTACK_DISTANCE && abs(dy) <= ATTACK_DISTANCE)
		{
			state_ = EnemyState::Attack;
			attack_timer = 0.5f;
			attackMoveTimer = 0.0f;
			break;
		}

		// プレイヤーを再発見し遠かったらChaseへ
		if (inView)
		{
			state_ = EnemyState::Chase;
			break;
		}


		dir_timer -= dt;
		prog_timer -= dt;

		// 3秒に1回向きをランダムに変える
		if (dir_timer < 0.0f)
		{
			dir_ = (DIR)(GetRand(3));
			dir_timer = 3.0f + dir_timer;
		}

		Point newPos = pos_;

		if (prog_timer < 0.0f)
		{
			switch (dir_)
			{
			case UP:
				newPos.y -= ENEMY_DRAW_SIZE;
				break;

			case DOWN:
				newPos.y += ENEMY_DRAW_SIZE;
				break;

			case LEFT:
				newPos.x -= ENEMY_DRAW_SIZE;
				break;

			case RIGHT:
				newPos.x += ENEMY_DRAW_SIZE;
				break;

			default:
				break;
			}

			int mapValue = FindGameObject<Stage>()->GetMap(
				newPos.x / CHA_SIZE,
				newPos.y / CHA_SIZE);

			// ステージ外に出ない場合だけ移動
			if (!(newPos.x < 1 ||
				newPos.x >(STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE ||
				newPos.y < 1 ||
				newPos.y >(STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE))
			{
				pos_ = newPos;
			}

			prog_timer = 0.5f + prog_timer;
		}

		break;
		}
	}

void Enemy::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	if (isHit)
	{
		DrawString(pos_.x, pos_.y - 20, "Hit!", GetColor(255, 0, 0));
	}

	Rect iRect[4] = {
		{  nowFrame * ENEMY_SIZE, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE}
	};
	DrawBox(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0), FALSE,2);
	DrawRectExtendGraph(pos_.x, pos_.y,pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		               iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);
	if (animTimer < 0) {
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}
	animTimer = animTimer - Time::DeltaTime();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);

	int centerX = pos_.x + ENEMY_DRAW_SIZE / 2;
	int centerY = pos_.y + ENEMY_DRAW_SIZE / 2;

	const int VIEW_LENGTH = CHA_SIZE * 4;  // 4マス
	const int VIEW_WIDTH = CHA_SIZE * 4;  // 横幅も4マス

	switch (dir_)
	{
	case UP:
		DrawTriangle(
			centerX, centerY,
			centerX - VIEW_WIDTH / 2, centerY - VIEW_LENGTH,
			centerX + VIEW_WIDTH / 2, centerY - VIEW_LENGTH,
			GetColor(255, 0, 0), TRUE);
		break;

	case DOWN:
		DrawTriangle(
			centerX, centerY,
			centerX - VIEW_WIDTH / 2, centerY + VIEW_LENGTH,
			centerX + VIEW_WIDTH / 2, centerY + VIEW_LENGTH,
			GetColor(255, 0, 0), TRUE);
		break;

	case LEFT:
		DrawTriangle(
			centerX, centerY,
			centerX - VIEW_LENGTH, centerY - VIEW_WIDTH / 2,
			centerX - VIEW_LENGTH, centerY + VIEW_WIDTH / 2,
			GetColor(255, 0, 0), TRUE);
		break;

	case RIGHT:
		DrawTriangle(
			centerX, centerY,
			centerX + VIEW_LENGTH, centerY - VIEW_WIDTH / 2,
			centerX + VIEW_LENGTH, centerY + VIEW_WIDTH / 2,
			GetColor(255, 0, 0), TRUE);
		break;
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
