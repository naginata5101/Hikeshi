#include "common.h"

//Gimmick----------------------------------------------
Gimmick::TypeInfo Gimmick::getTypeInfo(InputGimmick g) {
	TypeInfo i = { "　虚無",1, 1, -1, -1, 0, 0, false};
	switch (g) {
	case InputGimmick::WALL:
		strncpy_s(i.name, "　土壁", 15);
		i.w = 1;
		i.h = 1;
		i.rx = 4;
		i.ry = 1;
		i.maxFireLife = 999999999;
		i.maxHitLife =  999999999;
		i.maxEraseLife = 999999999;
		i.power = 0;
		i.isKey = false;
		break;
	case InputGimmick::HOUSE1:
		strncpy_s(i.name, "　小屋", 15);
		i.w = 1;
		i.h = 1;
		i.rx = 5;
		i.ry = 0;
		i.maxFireLife = 100;
		i.maxHitLife =  100;
		i.maxEraseLife = 300;
		i.power = 10;
		i.isKey = false;
		break;
	case InputGimmick::HOUSE2:
		strncpy_s(i.name, "重要小屋", 15);
		i.w = 1;
		i.h = 1;
		i.rx = 5;
		i.ry = 2;
		i.maxFireLife = 100;
		i.maxHitLife =  100;
		i.maxEraseLife = 300;
		i.power = 10;
		i.isKey = true;
		break;
	case InputGimmick::NAGAYA1:
		strncpy_s(i.name, "　長屋", 15);
		i.w = 2;
		i.h = 1;
		i.rx = 6;
		i.ry = 0;
		i.maxFireLife = 140;
		i.maxHitLife =  150;
		i.maxEraseLife = 450;
		i.power = 20;
		i.isKey = false;
		break;
	case InputGimmick::NAGAYA2:
		strncpy_s(i.name, "重要長屋", 15);
		i.w = 2;
		i.h = 1;
		i.rx = 6;
		i.ry = 2;
		i.maxFireLife = 150;
		i.maxHitLife =  150;
		i.maxEraseLife = 450;
		i.power = 20;
		i.isKey = true;
		break;
	case InputGimmick::YASHIKI1:
		strncpy_s(i.name, "　屋敷", 15);
		i.w = 2;
		i.h = 2;
		i.rx = 8;
		i.ry = 0;
		i.maxFireLife = 200;
		i.maxHitLife =  200;
		i.maxEraseLife = 600;
		i.power = 30;
		i.isKey = false;
		break;
	case InputGimmick::YASHIKI2:
		strncpy_s(i.name, "重要屋敷", 15);
		i.w = 2;
		i.h = 2;
		i.rx = 10;
		i.ry = 0;
		i.maxFireLife = 200;
		i.maxHitLife =  200;
		i.maxEraseLife = 600;
		i.power = 30;
		i.isKey = true;
		break;
	case InputGimmick::TREE:
		strncpy_s(i.name, "　枯木", 15);
		i.w = 1;
		i.h = 1;
		i.rx = 12;
		i.ry = 0;
		i.maxFireLife = 20;
		i.maxHitLife = 30;
		i.maxEraseLife = 10;
		i.power = 50;
		i.isKey = false;
		break;
	case InputGimmick::GATE:
		strncpy_s(i.name, "  門", 15);
		i.w = 1;
		i.h = 1;
		i.rx = 4;
		i.ry = 2;
		i.maxFireLife = 999999999;
		i.maxHitLife =  999999999;
		i.maxEraseLife = 999999999;
		i.power = 0;
		i.isKey = false;
		break;
	case InputGimmick::LINKED_HOUSE:
		strncpy_s(i.name, " 連携家 ", 15);
		i.w = 1;
		i.h = 1;
		i.rx = 12;
		i.ry = 2;
		i.maxFireLife = 100;
		i.maxHitLife =  100;
		i.maxEraseLife = 300;
		i.power = 10;
		i.isKey = false;
		break;
	}
	return i;
}


Gimmick::Gimmick(int X, int Y) {
	//コンストラクタ
	x = X;
	y = Y;
	stance = NONE;
	status = NORMAL;
}

int Gimmick::getX() {
	//x座標を返すメソッド
	return x;
}

int Gimmick::getY() {
	//y座標を返すメソッド
	return y;
}

Gimmick::Stance Gimmick::getStance() {
	return stance;
}

Gimmick::Status Gimmick::getStatus() {
	return status;
}

void Gimmick::beFired() {
	status = FIRED;
}

void Gimmick::refreshFired() {
	if (status == FIRED) status = NORMAL;
};

void Gimmick::draw(int camX, int camY) {
	//描画メソッド
	//取り敢えず灰色
	int s = CHIP;
	MikanDraw->DrawBox((x - camX) * s, (y - camY + SPACE) * s, s, s,0x55FFFFFF);
}

void Gimmick::drawFire(int camX, int camY) {
	//このギミック全体に炎を描画するメソッド
	//人物の描画後に呼び出すこと
}

void Gimmick::drawOneFire(Stance S, int X, int Y) {
	//炎のみの描画メソッド（半透明）
	int s = CHIP;
	MikanDraw->SetAlpha(0, 180);
	if (S == Gimmick::ROAD)			MikanDraw->DrawTexture(0, X * s, (Y + SPACE) * s, 2 * s, 2 * s, s, s);
	else if (S == Gimmick::HOUSE)	MikanDraw->DrawTexture(0, X * s, (Y + SPACE) * s, 2 * s, 0 * s, s, s);
	MikanDraw->SetAlpha(0, 255);
}

void Gimmick::drawRoad(int X, int Y) {
	//道の描画メソッド
	int s = CHIP;
	MikanDraw->DrawTexture(0, X * s, (Y + SPACE) * s, 4 * s, 0 * s, s, s);
}


//House------------------------------------------------
Gimmick::TypeInfo House::getTypeInfo(Type t) {
	switch (t) {
	case Type::HOUSE1:
		return Gimmick::getTypeInfo(InputGimmick::HOUSE1);
	case Type::HOUSE2:
		return Gimmick::getTypeInfo(InputGimmick::HOUSE2);
	case Type::NAGAYA1:
		return Gimmick::getTypeInfo(InputGimmick::NAGAYA1);
	case Type::NAGAYA2:
		return Gimmick::getTypeInfo(InputGimmick::NAGAYA2);
	case Type::YASHIKI1:
		return Gimmick::getTypeInfo(InputGimmick::YASHIKI1);
	case Type::YASHIKI2:
		return Gimmick::getTypeInfo(InputGimmick::YASHIKI2);
	case Type::TREE:
		return Gimmick::getTypeInfo(InputGimmick::TREE);
	case Type::LINKED:
		return Gimmick::getTypeInfo(InputGimmick::LINKED_HOUSE);
	}
	TypeInfo i = {1, 1, 0, 0};
	return i;
}


House::House(int X, int Y, InputGimmick G, InputStatus S) : Gimmick(X, Y) {
	//コンストラクタ
	status = Gimmick::NORMAL;
	stance = HOUSE;
	rich = false;
	nowBeFired = false;

	TypeInfo i = Gimmick::getTypeInfo(G);
	fireLife = i.maxFireLife;
	hitLife = i.maxHitLife;
	eraseLife = i.maxEraseLife;

	switch (S) {
	case InputStatus::FIRED:
		status = Gimmick::FIRED;
		break;
	case InputStatus::RICH:
		rich = true;
		break;
	}
	switch (G) {
	case InputGimmick::HOUSE1:
		type = Type::HOUSE1;
		break;
	case InputGimmick::HOUSE2:
		type = Type::HOUSE2;
		break;
	case InputGimmick::NAGAYA1:
		type = Type::NAGAYA1;
		break;
	case InputGimmick::NAGAYA2:
		type = Type::NAGAYA2;
		break;
	case InputGimmick::YASHIKI1:
		type = Type::YASHIKI1;
		break;
	case InputGimmick::YASHIKI2:
		type = Type::YASHIKI2;
		break;
	case InputGimmick::TREE:
		type = Type::TREE;
		break;
	case InputGimmick::LINKED_HOUSE:
		type = Type::LINKED;
		break;
	}

	if (status == FIRED) {
		fireLife = 0;
	}
	if (status == BROKEN) {
		hitLife = 0;
	}
}

void House::beFired(int power) {
	//炎ダメージを受けるメソッド
	//引数は火力
	fireLife -= power;
	nowBeFired = true;
	checkLife();
}

void House::beHitted(int power) {
	//打撃ダメージを受けるメソッド
	//引数は攻撃力
	if (hitLife > 0)	hitLife -= power;
	else				eraseLife -= power;
	checkLife();
}

void House::checkLife() {
	//体力チェックメソッド
	//体力表示の為に、0より小さいの体力を0にする
	if (eraseLife < 0)	eraseLife = 0;
	if (hitLife < 0)	hitLife = 0;
	if (fireLife < 0)	fireLife = 0;

	//体力に合わせて状態を変化させる
	if (eraseLife <= 0)		status = ERASED;
	else if (hitLife <= 0)	status = BROKEN;
	else if (fireLife <= 0)	status = FIRED;
	if (status != NORMAL)	rich = false;
}

void House::beStolen() {
	rich = false;
}

void House::refreshFired() {
	nowBeFired = false;
}

void House::draw(int camX, int camY) {
	//描画メソッド
	int s = CHIP;
	int X = x - camX;
	int Y = y - camY;
	TypeInfo ti = getTypeInfo();
	int w = ti.w;
	int h = ti.h;
	int rx = ti.rx;
	int ry = ti.ry;

	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			drawRoad(x + i - camX, y + j - camY);

	if (status != BROKEN)
		MikanDraw->DrawTexture(0, X * s, (Y + SPACE) * s, rx * s, ry * s, w * s, h * s);
	else
		MikanDraw->DrawTexture(0, X * s, (Y + SPACE) * s, rx * s, (ry + h) * s, w * s, h * s);

	//炎の描画
	if (nowBeFired)
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++)
				drawBeFired(x + i - camX, y + j - camY);
}

void House::drawFire(int camX, int camY) {
	//金も表わす
	TypeInfo ti = getTypeInfo();
	if (status == FIRED) {
		for (int i = 0; i < ti.w; i++) {
			for (int j = 0; j < ti.h; j++) {
				drawOneFire(Gimmick::HOUSE, x + i - camX, y + j - camY);
			}
		}
	} else if (rich) {
		int s = CHIP;
		for (int i = 0; i < ti.w; i++) {
			for (int j = 0; j < ti.h; j++) {
				MikanDraw->DrawTexture(0, (x + i - camX) * s, (y + j - camY + SPACE) * s, 2 * s, 3 * s, s, s);
			}
		}
	}
}

void House::drawLife(int X, int Y) {
	//体力ケージの描画

	int f = 2;		//枠の幅
	int f2 = f * 2;	//倍
	int c = CHIP;	//---注意---//
	X = X * c;
	Y = Y * c;
	int S = c / 4;	//短辺
	int L = c;		//長辺
	TypeInfo i = getTypeInfo();

	//カーソルの左右下に打撃、火炎、削除体力
	MikanDraw->DrawBox(X - S, Y, S, L, 0xFFFFFFFF);
	MikanDraw->DrawBox(X + c, Y, S, L, 0xFFFFFFFF);
	MikanDraw->DrawBox(X, Y + c, L, S, 0xFFFFFFFF);
	MikanDraw->DrawBox(X - S + f, Y + f, S - f2, L - f2, 0xFF000000);
	MikanDraw->DrawBox(X + c + f, Y + f, S - f2, L - f2, 0xFF000000);
	MikanDraw->DrawBox(X + f, Y + c + f, L - f2, S - f2, 0xFF000000);
	MikanDraw->DrawBox(X - S + f, Y + L - f, S - f2, -(L - f2) *  hitLife /  i.maxHitLife, 0xFF5555FF);	//左が打撃
	MikanDraw->DrawBox(X + c + f, Y + L - f, S - f2, -(L - f2) * fireLife / i.maxFireLife, 0xFFFF0000);	//右が火炎
	MikanDraw->DrawBox(X + f, Y + c + f, (L - f2) * eraseLife / i.maxEraseLife, S - f2, 0xFF33AA33);	//下が削除
}

void House::drawBeFired(int X, int Y) {
	//小さな炎のみの描画メソッド（半透明）
	int s = CHIP;
	MikanDraw->SetAlpha(0, 180);
	MikanDraw->DrawTexture(0, X * s, (Y + SPACE) * s, 2 * s, 1 * s, s, s);
	MikanDraw->SetAlpha(0, 255);
}

void House::drawBeHitted(int camX, int camY) {
	const int s = CHIP;
	TypeInfo t = getTypeInfo();
	MikanDraw->DrawTextureScaling(T_CHIPS, (x - camX) * s, (y + SPACE - camY) * s, 2 * s, 4 * s, s, s, t.w * s, t.h * s, 0);
}

int House::getFireLife() {
	//炎耐性を返すメソッド
	return fireLife;
}

int House::getHitLife() {
	//打撃耐性を返すメソッド
	return hitLife;
}

int House::getEraseLife() {
	return eraseLife;
}

bool House::getRich() {
	return rich;
}

House::Type House::getType() {
	return type;
}

House::TypeInfo House::getTypeInfo() {
	return getTypeInfo(type);
}


//Wall--------------------------------------------------
Gimmick::TypeInfo Wall::getTypeInfo(Type t) {
	switch (t) {
	case Wall::WALL:
		return Gimmick::getTypeInfo(InputGimmick::WALL);
		break;
	case Wall::GATE:
		return Gimmick::getTypeInfo(InputGimmick::GATE);
		break;
	}
	return Gimmick::getTypeInfo(InputGimmick::NONE);
}


Wall::Wall(int X, int Y, InputGimmick g) : Gimmick(X, Y) {
	//コンストラクタ
	stance = Gimmick::WALL;
	switch (g) {
	case InputGimmick::WALL:
		type = Wall::WALL;
		break;
	case InputGimmick::GATE:
		type = Wall::GATE;
		break;
	}
}

void Wall::beFired() {
	//炎ダメージを受けるメソッド
	//何もしない
}

void Wall::draw(int camX, int camY) {
	//描画メソッド
	int s = CHIP;
	TypeInfo i = getTypeInfo(type);

	drawRoad(x - camX, y - camY);
	MikanDraw->DrawTexture(0, (x - camX) * s, (y - camY + SPACE) * s, i.rx * s, i.ry * s, s, s);
}

void Wall::drawFire(int camX, int camY) {
	//何もしない
}

Wall::Type Wall::getType() {
	return type;
}

Gimmick::TypeInfo Wall::getTypeInfo() {
	return getTypeInfo(type);
}


//Road--------------------------------------------------
Road::Road(int X, int Y) : Gimmick(X, Y) {
	//コンストラクタ
	stance = ROAD;
}

void Road::beFired() {
	//炎ダメージを受けるメソッド
	//道のクラスは、呼び出された時点で延焼
	status = FIRED;
}

void Road::draw(int camX, int camY) {
	//描画メソッド
	drawRoad(x - camX, y - camY);
}

void Road::drawFire(int camX, int camY) {
	if (status == FIRED) drawOneFire(Gimmick::ROAD, x - camX, y - camY);
}
