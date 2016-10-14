#include "common.h"

//Human------------------------------------------------
Human::TypeInfo Human::getTypeInfo(Type t) {
	TypeInfo i = {"雑魚", 0, 0, 10, 1, 1};
	switch (t) 	{
	case Member::IPPAN:
		strncpy_s(i.name, "平隊員", 15);
		i.rx = 0;
		i.ry = 0;
		i.maxLife = 100;
		i.power = 10;
		i.speed = 10;
		break;
	case Member::GANKO:
		strncpy_s(i.name, "頑固爺", 15);
		i.rx = 0;
		i.ry = 1;
		i.maxLife = 200;
		i.power = 10;
		i.speed = 5;
		break;
	case Member::GORIKI:
		strncpy_s(i.name, "力自慢", 15);
		i.rx = 0;
		i.ry = 2;
		i.maxLife = 150;
		i.power = 20;
		i.speed = 5;
		break;
	case Member::IDATEN:
		strncpy_s(i.name, "韋駄天", 15);
		i.rx = 0;
		i.ry = 3;
		i.maxLife = 75;
		i.power = 5;
		i.speed = 15;
		break;
	case Member::CITIZEN:
		strncpy_s(i.name, "避難民", 15);
		i.rx = 1;
		i.ry = 0;
		i.maxLife = 50;
		i.power = 0;
		i.speed = 3;
		break;
	case Member::INCENDIARY:
		strncpy_s(i.name, "放火魔", 15);
		i.rx = 1;
		i.ry = 1;
		i.maxLife = 150;
		i.power = 0;
		i.speed = 3;
		break;
	case Member::THIEF:
		strncpy_s(i.name, "火事泥棒", 15);
		i.rx = 1;
		i.ry = 2;
		i.maxLife = 75;
		i.power = 0;
		i.speed = 3;
		break;
	}
	if (getInput(IN_ACCEL) > 0) i.speed *= ACCEL_RATE;

	return i;
}

Human::Human(int X, int Y, Type T) {
	//コンストラクタ
	x = X;
	y = Y;
	type = T;
	life = getTypeInfo(type).maxLife;
	interval = maxInterval;
}

void Human::move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	if (life > 0) {
		//移動メソッド
		if (interval <= 0) interval -= getTypeInfo(type).speed;
	}
}

void Human::beFired(int damage) {
	//炎ダメージを受けるメソッド
	life -= damage;
	if (life <= 0)	MikanSound->Play(S_HIMEI1, false);
}

void Human::draw(int camX, int camY) {
	if (life > 0) {	//生きている者のみ描画
		drawChara(x - camX, y - camY);
	}
}

void Human::drawChara(int X, int Y) {
	//キャラクター自身と体力バーを描画するメソッド
	int s = CHIP;
	TypeInfo i = getTypeInfo(type);
	MikanDraw->DrawTexture(0, X * s, (Y + SPACE) * s, i.rx * s,i.ry * s, s, s);
}

void Human::drawLife(int camX, int camY) {
	//体力バーの描画
	int s = CHIP;
	int f = 2; //Frameの太さ
	int X = (x - camX) * s;
	int Y = (y - camY + SPACE) * s;
	TypeInfo i = getTypeInfo(type);
	int ratio = 100 * life / i.maxLife;	//体力の割合
	unsigned long c;	//体力ケージの色

	if (ratio > 66)			c = 0xFF5555FF;	//青
	else if (ratio > 33)	c = 0xFF00AA00;	//緑
	else					c = 0xFFFF0000;	//赤
	MikanDraw->DrawBox(X,     Y,       s, - s / 4, 0xFFFFFFFF);
	MikanDraw->DrawBox(X + f, Y - f, s - f * 2, - (s / 4 - f * 2), 0xFF000000);
	MikanDraw->DrawBox(X + f, Y - f, ((s - f * 2) * ratio) / 100, - (s / 4 - f * 2), c);
}

int Human::getX() {
	//x座標を返すメソッド
	return x;
}

int Human::getY() {
	//y座標を返すメソッド
	return y;
}

int Human::getLife() {
	return life;
}

Human::Type Human::getType() {
	return type;
}

Human::TypeInfo Human::getTypeInfo() {
	return getTypeInfo(type);
}

Human::Stance Human::getStance() {
	switch (type) {
	case Human::CITIZEN:
		return EVACUEE;
		break;
	case Human::INCENDIARY:
		return CRIMINAL;
		break;
	case Human::THIEF:
		return CRIMINAL;
		break;
	default:
		return MEMBER;
		break;
	}
}


//Member-----------------------------------
Member::Member(int X, int Y, Type T) : Human(X, Y, T) {
	//コンストラクタ
	hitEffect = 0;
	destiX = destiY = NOSELECT;
	attackTarget = nullptr;
}

void Member::move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//移動メソッド
	if (life > 0) {
		//最優先で放火魔の捕獲の機会を伺う
		if (arrestCriminal(h, MaxHuman)) interval += maxInterval;
		else if ((g[y][x]->getStance() == Gimmick::ROAD || g[y][x]->getStatus() == Gimmick::BROKEN) &&
			(destiX != NOSELECT || destiY != NOSELECT || attackTarget != nullptr)) {
				if (interval <= 0) {
					//移動先がNoSelectじゃない＝定まっているならば移動する
					if (destiX != NOSELECT || destiY != NOSELECT)	moveDir(g, h, MaxHei, MaxWid, MaxHuman);
					//そうでない場合は攻撃
					else											attack();
					interval += maxInterval;
				}
				interval -= getTypeInfo(type).speed;
		}
		//移動先と現在位置が同じならば、移動先をNoSelectに
		if (x == destiX && y == destiY) {
			destiX = NOSELECT;
			destiY = NOSELECT;
		}
		//攻撃対象が壊れたら攻撃終了
		if (attackTarget != nullptr && attackTarget->getStatus() == Gimmick::ERASED)
			attackTarget = nullptr;

		--hitEffect;
	} else if (x != 0 && y != 0) {
		x = y = 0;
		destiX = destiY = NOSELECT;
		attackTarget = nullptr;
	}
}

bool Member::moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//一歩移動するメソッド
	switch (decideDir(g, h, MaxHei, MaxWid, MaxHuman, x, y)) {
	case UP:
		--this->y;
		return true;
	case RIGHT:
		++this->x;
		return true;
	case DOWN:
		++this->y;
		return true;
	case LEFT:
		--this->x;
		return true;
	default:
		return false;
	}
}

Dir Member::decideDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman, int X, int Y) {
	//移動方向を決定するメソッド
	int dx[5] = {
		X, X, X + 1, X, X - 1
	};
	int dy[5] = {
		Y, Y - 1, Y, Y + 1, Y
	};

	//侵入可能かを示すマップ　ギミックがあればtrue、家があればtrue、人がいればtrue
	bool free[MAXIMUM][MAXIMUM], house[MAXIMUM][MAXIMUM], human[MAXIMUM][MAXIMUM];
	int way[MAXIMUM][MAXIMUM]; //移動先としての優先順位を示したマップ

	//初期化
	for (int y = 0; y < MAXIMUM; y++) {
		for (int x = 0; x < MAXIMUM; x++) {
			house[y][x] = human[y][x] = free[y][x] = false;
			way[y][x] = -1;
		}
	}
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			Gimmick::Stance s = g[y][x]->getStance();
			if (s != Gimmick::NONE && s != Gimmick::WALL) free[y][x] = true;
			if (s == Gimmick::HOUSE) house[y][x] = true;
		}
	}
	for (int i = 0; i < MaxHuman; i++)
		//犯罪者は人に非ず（放火魔を避けないように→逮捕し易いように）
			if (h[i] != this && h[i]->getLife() > 0 && h[i]->getStance() != CRIMINAL)
				human[h[i]->getY()][h[i]->getX()] = true;

	/*
	** 初期条件
	** while(制作されたマップの現在位置が-1の間) {
	** ・マップの初期化
	** ・現在の条件でマップ製作
	** ・下記の順番で条件緩和
	** }
	** 
	**	家を0戸まで許す
	**		隊員を0体まで許す
	** 			…
	** 		隊員をMAXまで許す
	** 	家を1戸まで許す
	**		隊員を0体まで許す
	** 			…
	** 		隊員をMAXまで許す
	**		…
	**/
	int houseMax = 0, humanMax = 0;
	while (way[dy[HERE]][dx[HERE]] == -1) {
		for (int y = 0; y < MAXIMUM; y++)
			for (int x = 0; x < MAXIMUM; x++)
				way[y][x] = -1;

		makeMap(free, house, human, way, destiX, destiY, houseMax, humanMax, 0, 0, 0);

		if (humanMax <= MaxHuman) ++humanMax;
		else {
			humanMax = 0;
			++houseMax;
		}
	}

	//移動方向の決定
	Dir d = HERE;
	for (int i = 1; i < 5; i++) {
		if (way[dy[i]][dx[i]] >= 0 && way[dy[d]][dx[d]] > way[dy[i]][dx[i]])
			d = (Dir)i;
	}

	//移動方向に人物や建物がある時は移動しない
	if (human[dy[d]][dx[d]] || house[dy[d]][dx[d]]) return HERE;

	return d;
}

void Member::makeMap(const bool free[MAXIMUM][MAXIMUM], const bool house[MAXIMUM][MAXIMUM], const bool human[MAXIMUM][MAXIMUM], int way[MAXIMUM][MAXIMUM],
					 const int X, const int Y, const int houseMax, const int humanMax, int houseNum, int humanNum, int stepNum) {
						 //優先順位を示すマップを作るメソッド
						 int dx[5] = {
							 X, X, X + 1, X, X - 1
						 };
						 int dy[5] = {
							 Y, Y - 1, Y, Y + 1, Y
						 };

						 //家や人を乗り越える度にカウントし、最大値を超えたら再帰終了
						 if (house[dy[HERE]][dx[HERE]])	++houseNum;
						 if (human[dy[HERE]][dx[HERE]])	++humanNum;
						 if (humanNum > humanMax || houseNum > houseMax) return;

						 way[dy[HERE]][dx[HERE]] = stepNum++;
						 for (int i = 1; i < 5; i++) {
							 if (free[dy[i]][dx[i]] && (way[dy[i]][dx[i]] == -1 || way[dy[i]][dx[i]] > stepNum)) {
								 makeMap(free, house, human, way, dx[i], dy[i], houseMax, humanMax, houseNum, humanNum, stepNum);
							 }
						 }
}

void Member::drawRoute(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman, int camX, int camY) {
	//予定の経路を表示するメソッド
	if (destiX != NOSELECT && destiY != NOSELECT) {
		int s = CHIP;
		int X = x, Y = y;

		//目的地まで書き続ける
		while(X != destiX || Y != destiY) {
			//移動方向を決定し
			Dir d = decideDir(g, h, MaxHei, MaxWid, MaxHuman, X, Y);
			//実際に移動する
			if (d == UP) {
				--Y;
			} else if (d == RIGHT) {
				++X;
			} else if (d == DOWN) {
				++Y;
			} else if (d == LEFT) {
				--X;
			} else if (d == HERE) {
				break;
			}
			MikanDraw->DrawTexture(T_CHIPS, (X - camX) * s, (Y - camY + SPACE) * s, 3 * s, 2 * s, s, s);
		}
		MikanDraw->DrawTexture(T_CHIPS, (destiX - camX) * s, (destiY - camY + SPACE) * s, 3 * s, 1 * s, s, s);
	}
}

void Member::attack() {
	//家に攻撃するメソッド
	if (attackTarget != nullptr) {
		attackTarget->beHitted(getTypeInfo(type).power);
		hitEffect = hitEffectTime;
	}
}

void Member::removeAttackTarget(House* h) {
	if (attackTarget == h) attackTarget = nullptr;
}

bool Member::arrestCriminal(Human *h[MAXIMUM], int MaxHuman) {
	for (int i = 0; i < MaxHuman; i++) {
		Incendiary *c = (Incendiary*)h[i];
		if (c->getStance() == CRIMINAL && abs(x - c->getX()) + abs(y - c->getY()) <= 1) {
			c->beArrest();
			return true;
		}
	}

	return false;
}

void Member::draw(int camX, int camY) {
	//描画メソッド
	if (life > 0) {	//生きている者のみ描画
		drawChara(x - camX, y - camY); //基本描画
		if (attackTarget != nullptr) { //臨時：攻撃対象表示
			int s = CHIP;
			int tx = attackTarget->getX() - camX;
			int ty = attackTarget->getY() - camY;
			int tw = attackTarget->getTypeInfo().w;
			int th = attackTarget->getTypeInfo().h;
			MikanDraw->DrawLine((x - camX) * s + s/2, (y - camY + SPACE) * s + s/2,
				tx * s + tw * s/2, (ty + SPACE) * s + th * s/2, 0xFFFFAAAA, 5);
		}
		if (hitEffect > 0 && attackTarget != nullptr) attackTarget->drawBeHitted(camX, camY);
	}
}

void Member::setDestination(int X,int Y, House *H) {
	//移動先と攻撃方向をセットするメソッド
	destiX = X;
	destiY = Y;
	attackTarget = H;	//NOSELECTなら待機
	interval = maxInterval;
}


//Evancuee------------------------------------
Evancuee::Evancuee(int X, int Y) : Human(X, Y, CITIZEN) {}

void Evancuee::move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//移動メソッド
	if (life > 0) {
		if (g[y][x]->getStance() == Gimmick::ROAD || g[y][x]->getStatus() == Gimmick::BROKEN) {
			//最寄りの炎から逃げ続ける
			if (interval <= 0 && moveDir(g, h, MaxHei, MaxWid, MaxHuman)) interval += maxInterval;
			interval -= getTypeInfo(type).speed;
		}
	} else {
		x = y = 0;
	}
}

bool Evancuee::moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//最寄りの炎から逃げる方向を返す
	bool roadMap[MAXIMUM][MAXIMUM], fireMap[MAXIMUM][MAXIMUM], humanMap[MAXIMUM][MAXIMUM];
	//道ならばtrue、燃えているならばtrue、人がいたらtrue
	int way[MAXIMUM][MAXIMUM];
	int nearFireDis = NOSELECT, nearFireX = NOSELECT, nearFireY = NOSELECT; //最寄りの炎の情報
	int dx[5] = {x, x, x + 1, x, x - 1};//HEREを含めた5方向のX,Y座標
	int dy[5] = {y, y - 1, y, y + 1, y};
	Dir d = HERE; //最終決定した移動方向

	for (int y = 0; y < MAXIMUM; y++) {
		for (int x = 0; x < MAXIMUM; x++) {
			roadMap[y][x] = fireMap[y][x] = humanMap[y][x] = false;
			way[y][x] = -1;
		}
	}
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			if (g[y][x]->getStance() == Gimmick::ROAD)	roadMap[y][x] = true;
			if (g[y][x]->getStatus() == Gimmick::FIRED)	fireMap[y][x] = true;
		}
	}
	for (int i = 0; i < MaxHuman; i++) {
		humanMap[h[i]->getY()][h[i]->getX()] = true;
	}

	//作成範囲は暫定的に10マス
	makeMap(roadMap, fireMap, way, dx[HERE], dy[HERE], 10, 0);
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			int fw = way[y][x];
			if (fireMap[y][x] && fw >= 0 && (nearFireDis == -1 || nearFireDis > fw)) {
				nearFireX = x;
				nearFireY = y;
				nearFireDis = fw;
			}
			way[y][x] = -1;
		}
	}
	if (nearFireDis == NOSELECT) return false;

	makeMap(roadMap, fireMap, way, nearFireX, nearFireY, nearFireDis + 1, 0);
	//最寄りの炎に行き当たらない道が存在し空いていれば、そちらの方向を目指す
	for (int i = 1; i < 5; i++) {
		if (roadMap[dy[i]][dx[i]] && !humanMap[dy[i]][dx[i]] && way[dy[i]][dx[i]] < 0) {
			d = (Dir)i;
			break;
		}
	}
	//↑が無ければ、最寄りの炎から遠ざかる方向へ向かう
	if (d == HERE) {
		for (int i = 1; i < 5; i++) {
			if (way[dy[d]][dx[d]] < way[dy[i]][dx[i]] && !humanMap[dy[i]][dx[i]]) d = (Dir)i;
		}
	}
	//↑の方向に人がいたら動かない
	if (humanMap[dy[d]][dx[d]]) d = HERE;


	//実際に移動する
	switch (d) {
	case UP:
		--this->y;
		return true;
	case RIGHT:
		++this->x;
		return true;
	case DOWN:
		++this->y;
		return true;
	case LEFT:
		--this->x;
		return true;
	default:
		return false;
	}
}

void Evancuee::makeMap(const bool roadMap[MAXIMUM][MAXIMUM], const bool fireMap[MAXIMUM][MAXIMUM], int way[MAXIMUM][MAXIMUM], const int X, const int Y, const int max, int n) {
	int dx[5] = {
		X, X, X + 1, X, X - 1
	};
	int dy[5] = {
		Y, Y - 1, Y, Y + 1, Y
	};

	way[Y][X] = n++;

	//距離maxマスまで調べる
	if (n <= max) {
		for (int i = 1; i < 5; i++) {
			if (roadMap[dy[i]][dx[i]] && (way[dy[i]][dx[i]] == -1 || way[dy[i]][dx[i]] > n)) {
				makeMap(roadMap, fireMap, way, dx[i], dy[i], max, n);
			}
		}
	}
}

void Evancuee::draw(int camX, int camY) {
	if (life > 0) drawChara(x - camX, y - camY); //基本描画
}


//Incendiary------------------------------------
Incendiary::Incendiary(int X, int Y) : Human(X, Y, INCENDIARY) {
	beenArrested = false;
	fireInter = fireInterMax;
}

void Incendiary::move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//移動メソッド
	if (life > 0) {
		if (g[y][x]->getStance() == Gimmick::ROAD || g[y][x]->getStatus() == Gimmick::BROKEN) {
			if (interval <= 0) {
				//前回の放火から十分に時間が経っており、周囲に燃やせる家があるなら放火する
				if (--fireInter <= 0 && fireHouse(g)) fireInter = fireInterMax;
				//そうでなければ移動する
				else if(moveDir(g, h, MaxHei, MaxWid, MaxHuman)) interval += maxInterval;
			}
			interval -= getTypeInfo(type).speed;
		}
	} else {
		x = y = 0;
	}
}

bool Incendiary::moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//隊員と炎から逃げ、燃えていない家を目指す方向を帰す
	//道ならばtrue、隊員がいたらtrue、人がいたらtrue
	bool road[MAXIMUM][MAXIMUM], human[MAXIMUM][MAXIMUM], member[MAXIMUM][MAXIMUM];
	int map[MAXIMUM][MAXIMUM]; //マス毎の得点（優先順位）を表わす配列
	//HEREを含めた5方向のX,Y座標
	int dx[5] = {x, x, x + 1, x, x - 1};
	int dy[5] = {y, y - 1, y, y + 1, y};
	bool isChecked[5] = {false, false, false, false, false}; //人とぶつかるか確認したか否か　最後の処理に使用
	Dir d; //最終決定した移動方向

	for (int y = 0; y < MAXIMUM; y++) {
		for (int x = 0; x < MAXIMUM; x++) {
			if (y < MaxHei && x < MaxWid) road[y][x] = (g[y][x]->getStance() == Gimmick::ROAD);
			human[y][x] = member[y][x] = false;
			map[y][x] = 0;
		}
	}

	for (int i = 0; i < MaxHuman; i++) {
		int x = h[i]->getX();
		int y = h[i]->getY();
		human[y][x] = true;
		if (h[i]->getStance() == Human::MEMBER) member[y][x] = true;
	}

	//マップに値を与える
	//点数…隊員：-400000、炎：-1000、燃えていない家に隣接：各+10000
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			if (road[y][x]) {
				int score = 0;
				if (member[y][x])
					score += -400000;
				if (g[y][x]->getStatus() == Gimmick::FIRED)
					score += -1000;
				//normalの家に隣接していたら+10000
				for (int i = 1; i < 5; i++) {
					House *h = (House*)g[dy[i] - this->y + y][dx[i] - this->x + x];
					if (h->getStance() == Gimmick::HOUSE && h->getStatus() == Gimmick::NORMAL &&
						!h->getTypeInfo().isKey)
						score += 10000;
				}

				//得点を拡散させる
				if (score != 0) {
					bool checked[MAXIMUM][MAXIMUM]; //下の関数内で、計算し終えたかどうか
					for (int i = 0; i < MaxHei; i++)
						for (int j = 0; j < MaxWid; j++)
							checked[i][j] = false;
					makeMap(road, map, checked, x, y, score);
				}
			}
		}
	}


	do {
		d = HERE;
		//最も点数が高い方向へ進む
		for (int i = 1; i < 5; i++) 
			if (!isChecked[i] && road[dy[i]][dx[i]] && map[dy[i]][dx[i]] > map[dy[(int)d]][dx[(int)d]]) d = (Dir)i;
		isChecked[d] = true;
	} while (d != HERE && human[dy[d]][dx[d]]);


	//実際に移動する
	switch (d) {
	case UP:
		--this->y;
		return true;
	case RIGHT:
		++this->x;
		return true;
	case DOWN:
		++this->y;
		return true;
	case LEFT:
		--this->x;
		return true;
	default:
		return false;
	}
}

void Incendiary::makeMap(const bool road[MAXIMUM][MAXIMUM], int map[MAXIMUM][MAXIMUM], bool checked[MAXIMUM][MAXIMUM], const int X, const int Y, int score) {
	const int dx[5] = {
		X, X, X + 1, X, X - 1
	};
	const int dy[5] = {
		Y, Y - 1, Y, Y + 1, Y
	};

	if (!score) return;		//scoreが0になったら終了
	map[Y][X] += score;		//scoreを加算
	checked[Y][X] = true;	//チェックする

	for (int i = 1; i < 5; i++) {
		int x = dx[i];
		int y = dy[i];
		//道である∧まだ加算されていない→scoreを半減して渡す
		if (road[y][x] && !checked[y][x]) makeMap(road, map, checked, x, y, score / 2);
	}
}

bool Incendiary::fireHouse(Gimmick *gim[MAXIMUM][MAXIMUM]) {
	//隣接する家1つに放火するメソッド
	int dy[5] = {
		y, y - 1, y, y + 1, y
	};
	int dx[5] = {
		x, x, x + 1, x, x - 1
	};

	for (int i = 0; i < 5; i++) {
		Gimmick *g = gim[dy[i]][dx[i]];
		if (g->getStance() == Gimmick::Stance::HOUSE) {
			House *h = (House*)g;
			if (h->getStance() == Gimmick::HOUSE &&	h->getStatus() == Gimmick::NORMAL && !h->getTypeInfo().isKey) { //キー家には放火しない
				h->beFired(1000000);
				return true;
			}
		}
	}
	return false;
}

void Incendiary::beArrest() {
	life = 0;
	beenArrested = true;
	MikanSound->Play(S_HIMEI2, false);
}

bool Incendiary::getBeenArrested() {
	return beenArrested;
}

void Incendiary::draw(int camX, int camY) {
	if (life > 0) drawChara(x - camX, y - camY); //基本描画
}


//Thief------------------------------------
Thief::Thief(int X, int Y) : Human(X, Y, THIEF) {
	beenArrested = false;
	desireMoney = 2;
}

void Thief::move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//移動メソッド
	if (life > 0) {
		if (interval <= 0 && (g[y][x]->getStance() == Gimmick::ROAD || g[y][x]->getStatus() == Gimmick::BROKEN)) {
			if (exit(g) || stealHouse(g) || moveDir(g, h, MaxHei, MaxWid, MaxHuman)) interval += maxInterval;
		} else interval -= getTypeInfo(type).speed;
	} else {
		x = y = 0;
	}
}

bool Thief::moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//隊員と炎から逃げ、燃えていない家を目指す方向を帰す
	//道ならばtrue、隊員がいたらtrue、人がいたらtrue
	bool road[MAXIMUM][MAXIMUM], human[MAXIMUM][MAXIMUM], member[MAXIMUM][MAXIMUM];
	int map[MAXIMUM][MAXIMUM]; //マス毎の得点（優先順位）を表わす配列
	//HEREを含めた5方向のX,Y座標
	int dx[5] = {x, x, x + 1, x, x - 1};
	int dy[5] = {y, y - 1, y, y + 1, y};
	bool isChecked[5] = {false, false, false, false, false}; //人とぶつかるか確認したか否か　最後の処理に使用
	Dir d; //最終決定した移動方向

	for (int y = 0; y < MAXIMUM; y++) {
		for (int x = 0; x < MAXIMUM; x++) {
			if (y < MaxHei && x < MaxWid) road[y][x] = (g[y][x]->getStance() == Gimmick::ROAD);
			human[y][x] = member[y][x] = false;
			map[y][x] = 0;
		}
	}
	for (int i = 0; i < MaxHuman; i++) {
		int x = h[i]->getX();
		int y = h[i]->getY();
		human[y][x] = true;
		if (h[i]->getStance() == Human::MEMBER) member[y][x] = true;
	}

	//マップに値を与える
	//点数…隊員：-400000、炎：-5000、金のある家 or 門に隣接：各+200000 or +500000
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			if (road[y][x]) {
				int score = 0;
				if (member[y][x])							score += -400000;
				if (g[y][x]->getStatus() == Gimmick::FIRED)	score += -5000;

				//ノルマを達成していなければ
				if (desireMoney > 0) {
					//金持ちの家に隣接していたら+200000
					for (int i = 1; i < 5; i++) {
						House *h = (House*)g[dy[i] - this->y + y][dx[i] - this->x + x];
						if (h->getStance() == Gimmick::HOUSE && h->getRich())
							score += 200000;
					}
					//ノルマを達成していたら
				} else {
					//門に隣接していたら+500000
					for (int i = 1; i < 5; i++) {
						Wall *w = (Wall*)g[dy[i] - this->y + y][dx[i] - this->x + x];
						if (w->getStance() == Gimmick::WALL && w->getType() == Wall::GATE)
							score += 500000;
					}
				}

				//得点を拡散させる
				if (score != 0) {
					bool checked[MAXIMUM][MAXIMUM]; //下の関数内で、計算し終えたかどうか
					for (int i = 0; i < MaxHei; i++)
						for (int j = 0; j < MaxWid; j++)
							checked[i][j] = false;
					makeMap(road, map, checked, x, y, score);
				}
			}
		}
	}


	do {
		d = HERE;
		//最も点数が高い方向へ進む
		for (int i = 1; i < 5; i++) 
			if (!isChecked[i] && road[dy[i]][dx[i]] && map[dy[i]][dx[i]] > map[dy[(int)d]][dx[(int)d]]) d = (Dir)i;
		isChecked[d] = true;
	} while (d != HERE && human[dy[d]][dx[d]]);


	//実際に移動する
	switch (d) {
	case UP:
		--this->y;
		return true;
	case RIGHT:
		++this->x;
		return true;
	case DOWN:
		++this->y;
		return true;
	case LEFT:
		--this->x;
		return true;
	default:
		return false;
	}
}

void Thief::makeMap(const bool road[MAXIMUM][MAXIMUM], int map[MAXIMUM][MAXIMUM], bool checked[MAXIMUM][MAXIMUM], const int X, const int Y, int score) {
	const int dx[5] = {
		X, X, X + 1, X, X - 1
	};
	const int dy[5] = {
		Y, Y - 1, Y, Y + 1, Y
	};

	if (!score) return;		//scoreが0になったら終了
	map[Y][X] += score;		//scoreを加算
	checked[Y][X] = true;	//チェックする

	for (int i = 1; i < 5; i++) {
		int x = dx[i];
		int y = dy[i];
		//道である∧まだ加算されていない→scoreを半減して渡す
		if (road[y][x] && !checked[y][x]) makeMap(road, map, checked, x, y, score / 2);
	}
}

bool Thief::stealHouse(Gimmick *g[MAXIMUM][MAXIMUM]) {
	int dx[5] = {x, x, x + 1, x, x - 1};
	int dy[5] = {y, y - 1, y, y + 1, y};
	for (int i = 1; i < 5; i++) {
		House *h = (House*)g[dy[i]][dx[i]];
		if (h->getStance() == Gimmick::HOUSE && h->getRich()) {
			--desireMoney;
			h->beStolen();
			return true;
		}
	}
	return false;
}

bool Thief::exit(Gimmick *g[MAXIMUM][MAXIMUM]) {
	int dx[5] = {x, x, x + 1, x, x - 1};
	int dy[5] = {y, y - 1, y, y + 1, y};

	if (desireMoney <= 0) {
		for (int i = 1; i < 5; i++) {
			Wall *w = (Wall*)g[dy[i]][dx[i]];
			if (w->getStance() == Gimmick::WALL && w->getType() == Wall::GATE) {
				life = 0;
				beenArrested = false;
				return true;
			}
		}
	}
	return false;
}

void Thief::beArrest() {
	life = 0;
	beenArrested = true;
	MikanSound->Play(S_HIMEI2, false);
}

bool Thief::getBeenArrested() {
	return beenArrested;
}

void Thief::draw(int camX, int camY) {
	if (life > 0) drawChara(x - camX, y - camY); //基本描画
}
