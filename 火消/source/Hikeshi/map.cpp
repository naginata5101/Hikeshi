#include "common.h"

//Gimmick----------------------------------------------
Gimmick::TypeInfo Gimmick::getTypeInfo(InputGimmick g) {
	TypeInfo i = { "�@����",1, 1, -1, -1, 0, 0, false};
	switch (g) {
	case InputGimmick::WALL:
		strncpy_s(i.name, "�@�y��", 15);
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
		strncpy_s(i.name, "�@����", 15);
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
		strncpy_s(i.name, "�d�v����", 15);
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
		strncpy_s(i.name, "�@����", 15);
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
		strncpy_s(i.name, "�d�v����", 15);
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
		strncpy_s(i.name, "�@���~", 15);
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
		strncpy_s(i.name, "�d�v���~", 15);
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
		strncpy_s(i.name, "�@�͖�", 15);
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
		strncpy_s(i.name, "  ��", 15);
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
		strncpy_s(i.name, " �A�g�� ", 15);
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
	//�R���X�g���N�^
	x = X;
	y = Y;
	stance = NONE;
	status = NORMAL;
}

int Gimmick::getX() {
	//x���W��Ԃ����\�b�h
	return x;
}

int Gimmick::getY() {
	//y���W��Ԃ����\�b�h
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
	//�`�惁�\�b�h
	//��芸�����D�F
	int s = CHIP;
	MikanDraw->DrawBox((x - camX) * s, (y - camY + SPACE) * s, s, s,0x55FFFFFF);
}

void Gimmick::drawFire(int camX, int camY) {
	//���̃M�~�b�N�S�̂ɉ���`�悷�郁�\�b�h
	//�l���̕`���ɌĂяo������
}

void Gimmick::drawOneFire(Stance S, int X, int Y) {
	//���݂̂̕`�惁�\�b�h�i�������j
	int s = CHIP;
	MikanDraw->SetAlpha(0, 180);
	if (S == Gimmick::ROAD)			MikanDraw->DrawTexture(0, X * s, (Y + SPACE) * s, 2 * s, 2 * s, s, s);
	else if (S == Gimmick::HOUSE)	MikanDraw->DrawTexture(0, X * s, (Y + SPACE) * s, 2 * s, 0 * s, s, s);
	MikanDraw->SetAlpha(0, 255);
}

void Gimmick::drawRoad(int X, int Y) {
	//���̕`�惁�\�b�h
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
	//�R���X�g���N�^
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
	//���_���[�W���󂯂郁�\�b�h
	//�����͉Η�
	fireLife -= power;
	nowBeFired = true;
	checkLife();
}

void House::beHitted(int power) {
	//�Ō��_���[�W���󂯂郁�\�b�h
	//�����͍U����
	if (hitLife > 0)	hitLife -= power;
	else				eraseLife -= power;
	checkLife();
}

void House::checkLife() {
	//�̗̓`�F�b�N���\�b�h
	//�̗͕\���ׂ̈ɁA0��菬�����̗̑͂�0�ɂ���
	if (eraseLife < 0)	eraseLife = 0;
	if (hitLife < 0)	hitLife = 0;
	if (fireLife < 0)	fireLife = 0;

	//�̗͂ɍ��킹�ď�Ԃ�ω�������
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
	//�`�惁�\�b�h
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

	//���̕`��
	if (nowBeFired)
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++)
				drawBeFired(x + i - camX, y + j - camY);
}

void House::drawFire(int camX, int camY) {
	//�����\�킷
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
	//�̗̓P�[�W�̕`��

	int f = 2;		//�g�̕�
	int f2 = f * 2;	//�{
	int c = CHIP;	//---����---//
	X = X * c;
	Y = Y * c;
	int S = c / 4;	//�Z��
	int L = c;		//����
	TypeInfo i = getTypeInfo();

	//�J�[�\���̍��E���ɑŌ��A�Ή��A�폜�̗�
	MikanDraw->DrawBox(X - S, Y, S, L, 0xFFFFFFFF);
	MikanDraw->DrawBox(X + c, Y, S, L, 0xFFFFFFFF);
	MikanDraw->DrawBox(X, Y + c, L, S, 0xFFFFFFFF);
	MikanDraw->DrawBox(X - S + f, Y + f, S - f2, L - f2, 0xFF000000);
	MikanDraw->DrawBox(X + c + f, Y + f, S - f2, L - f2, 0xFF000000);
	MikanDraw->DrawBox(X + f, Y + c + f, L - f2, S - f2, 0xFF000000);
	MikanDraw->DrawBox(X - S + f, Y + L - f, S - f2, -(L - f2) *  hitLife /  i.maxHitLife, 0xFF5555FF);	//�����Ō�
	MikanDraw->DrawBox(X + c + f, Y + L - f, S - f2, -(L - f2) * fireLife / i.maxFireLife, 0xFFFF0000);	//�E���Ή�
	MikanDraw->DrawBox(X + f, Y + c + f, (L - f2) * eraseLife / i.maxEraseLife, S - f2, 0xFF33AA33);	//�����폜
}

void House::drawBeFired(int X, int Y) {
	//�����ȉ��݂̂̕`�惁�\�b�h�i�������j
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
	//���ϐ���Ԃ����\�b�h
	return fireLife;
}

int House::getHitLife() {
	//�Ō��ϐ���Ԃ����\�b�h
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
	//�R���X�g���N�^
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
	//���_���[�W���󂯂郁�\�b�h
	//�������Ȃ�
}

void Wall::draw(int camX, int camY) {
	//�`�惁�\�b�h
	int s = CHIP;
	TypeInfo i = getTypeInfo(type);

	drawRoad(x - camX, y - camY);
	MikanDraw->DrawTexture(0, (x - camX) * s, (y - camY + SPACE) * s, i.rx * s, i.ry * s, s, s);
}

void Wall::drawFire(int camX, int camY) {
	//�������Ȃ�
}

Wall::Type Wall::getType() {
	return type;
}

Gimmick::TypeInfo Wall::getTypeInfo() {
	return getTypeInfo(type);
}


//Road--------------------------------------------------
Road::Road(int X, int Y) : Gimmick(X, Y) {
	//�R���X�g���N�^
	stance = ROAD;
}

void Road::beFired() {
	//���_���[�W���󂯂郁�\�b�h
	//���̃N���X�́A�Ăяo���ꂽ���_�ŉ���
	status = FIRED;
}

void Road::draw(int camX, int camY) {
	//�`�惁�\�b�h
	drawRoad(x - camX, y - camY);
}

void Road::drawFire(int camX, int camY) {
	if (status == FIRED) drawOneFire(Gimmick::ROAD, x - camX, y - camY);
}
