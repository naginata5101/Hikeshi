#include "common.h"

//Human------------------------------------------------
Human::TypeInfo Human::getTypeInfo(Type t) {
	TypeInfo i = {"�G��", 0, 0, 10, 1, 1};
	switch (t) 	{
	case Member::IPPAN:
		strncpy_s(i.name, "������", 15);
		i.rx = 0;
		i.ry = 0;
		i.maxLife = 100;
		i.power = 10;
		i.speed = 10;
		break;
	case Member::GANKO:
		strncpy_s(i.name, "��Ŗ�", 15);
		i.rx = 0;
		i.ry = 1;
		i.maxLife = 200;
		i.power = 10;
		i.speed = 5;
		break;
	case Member::GORIKI:
		strncpy_s(i.name, "�͎���", 15);
		i.rx = 0;
		i.ry = 2;
		i.maxLife = 150;
		i.power = 20;
		i.speed = 5;
		break;
	case Member::IDATEN:
		strncpy_s(i.name, "��ʓV", 15);
		i.rx = 0;
		i.ry = 3;
		i.maxLife = 75;
		i.power = 5;
		i.speed = 15;
		break;
	case Member::CITIZEN:
		strncpy_s(i.name, "��", 15);
		i.rx = 1;
		i.ry = 0;
		i.maxLife = 50;
		i.power = 0;
		i.speed = 3;
		break;
	case Member::INCENDIARY:
		strncpy_s(i.name, "���Ζ�", 15);
		i.rx = 1;
		i.ry = 1;
		i.maxLife = 150;
		i.power = 0;
		i.speed = 3;
		break;
	case Member::THIEF:
		strncpy_s(i.name, "�Ύ��D�_", 15);
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
	//�R���X�g���N�^
	x = X;
	y = Y;
	type = T;
	life = getTypeInfo(type).maxLife;
	interval = maxInterval;
}

void Human::move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	if (life > 0) {
		//�ړ����\�b�h
		if (interval <= 0) interval -= getTypeInfo(type).speed;
	}
}

void Human::beFired(int damage) {
	//���_���[�W���󂯂郁�\�b�h
	life -= damage;
	if (life <= 0)	MikanSound->Play(S_HIMEI1, false);
}

void Human::draw(int camX, int camY) {
	if (life > 0) {	//�����Ă���҂̂ݕ`��
		drawChara(x - camX, y - camY);
	}
}

void Human::drawChara(int X, int Y) {
	//�L�����N�^�[���g�Ƒ̗̓o�[��`�悷�郁�\�b�h
	int s = CHIP;
	TypeInfo i = getTypeInfo(type);
	MikanDraw->DrawTexture(0, X * s, (Y + SPACE) * s, i.rx * s,i.ry * s, s, s);
}

void Human::drawLife(int camX, int camY) {
	//�̗̓o�[�̕`��
	int s = CHIP;
	int f = 2; //Frame�̑���
	int X = (x - camX) * s;
	int Y = (y - camY + SPACE) * s;
	TypeInfo i = getTypeInfo(type);
	int ratio = 100 * life / i.maxLife;	//�̗͂̊���
	unsigned long c;	//�̗̓P�[�W�̐F

	if (ratio > 66)			c = 0xFF5555FF;	//��
	else if (ratio > 33)	c = 0xFF00AA00;	//��
	else					c = 0xFFFF0000;	//��
	MikanDraw->DrawBox(X,     Y,       s, - s / 4, 0xFFFFFFFF);
	MikanDraw->DrawBox(X + f, Y - f, s - f * 2, - (s / 4 - f * 2), 0xFF000000);
	MikanDraw->DrawBox(X + f, Y - f, ((s - f * 2) * ratio) / 100, - (s / 4 - f * 2), c);
}

int Human::getX() {
	//x���W��Ԃ����\�b�h
	return x;
}

int Human::getY() {
	//y���W��Ԃ����\�b�h
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
	//�R���X�g���N�^
	hitEffect = 0;
	destiX = destiY = NOSELECT;
	attackTarget = nullptr;
}

void Member::move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//�ړ����\�b�h
	if (life > 0) {
		//�ŗD��ŕ��Ζ��̕ߊl�̋@����f��
		if (arrestCriminal(h, MaxHuman)) interval += maxInterval;
		else if ((g[y][x]->getStance() == Gimmick::ROAD || g[y][x]->getStatus() == Gimmick::BROKEN) &&
			(destiX != NOSELECT || destiY != NOSELECT || attackTarget != nullptr)) {
				if (interval <= 0) {
					//�ړ��悪NoSelect����Ȃ�����܂��Ă���Ȃ�Έړ�����
					if (destiX != NOSELECT || destiY != NOSELECT)	moveDir(g, h, MaxHei, MaxWid, MaxHuman);
					//�����łȂ��ꍇ�͍U��
					else											attack();
					interval += maxInterval;
				}
				interval -= getTypeInfo(type).speed;
		}
		//�ړ���ƌ��݈ʒu�������Ȃ�΁A�ړ����NoSelect��
		if (x == destiX && y == destiY) {
			destiX = NOSELECT;
			destiY = NOSELECT;
		}
		//�U���Ώۂ���ꂽ��U���I��
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
	//����ړ����郁�\�b�h
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
	//�ړ����������肷�郁�\�b�h
	int dx[5] = {
		X, X, X + 1, X, X - 1
	};
	int dy[5] = {
		Y, Y - 1, Y, Y + 1, Y
	};

	//�N���\���������}�b�v�@�M�~�b�N�������true�A�Ƃ������true�A�l�������true
	bool free[MAXIMUM][MAXIMUM], house[MAXIMUM][MAXIMUM], human[MAXIMUM][MAXIMUM];
	int way[MAXIMUM][MAXIMUM]; //�ړ���Ƃ��Ă̗D�揇�ʂ��������}�b�v

	//������
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
		//�ƍߎ҂͐l�ɔ񂸁i���Ζ�������Ȃ��悤�Ɂ��ߕ߂��Ղ��悤�Ɂj
			if (h[i] != this && h[i]->getLife() > 0 && h[i]->getStance() != CRIMINAL)
				human[h[i]->getY()][h[i]->getX()] = true;

	/*
	** ��������
	** while(���삳�ꂽ�}�b�v�̌��݈ʒu��-1�̊�) {
	** �E�}�b�v�̏�����
	** �E���݂̏����Ń}�b�v����
	** �E���L�̏��Ԃŏ����ɘa
	** }
	** 
	**	�Ƃ�0�˂܂ŋ���
	**		������0�̂܂ŋ���
	** 			�c
	** 		������MAX�܂ŋ���
	** 	�Ƃ�1�˂܂ŋ���
	**		������0�̂܂ŋ���
	** 			�c
	** 		������MAX�܂ŋ���
	**		�c
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

	//�ړ������̌���
	Dir d = HERE;
	for (int i = 1; i < 5; i++) {
		if (way[dy[i]][dx[i]] >= 0 && way[dy[d]][dx[d]] > way[dy[i]][dx[i]])
			d = (Dir)i;
	}

	//�ړ������ɐl���⌚�������鎞�͈ړ����Ȃ�
	if (human[dy[d]][dx[d]] || house[dy[d]][dx[d]]) return HERE;

	return d;
}

void Member::makeMap(const bool free[MAXIMUM][MAXIMUM], const bool house[MAXIMUM][MAXIMUM], const bool human[MAXIMUM][MAXIMUM], int way[MAXIMUM][MAXIMUM],
					 const int X, const int Y, const int houseMax, const int humanMax, int houseNum, int humanNum, int stepNum) {
						 //�D�揇�ʂ������}�b�v����郁�\�b�h
						 int dx[5] = {
							 X, X, X + 1, X, X - 1
						 };
						 int dy[5] = {
							 Y, Y - 1, Y, Y + 1, Y
						 };

						 //�Ƃ�l�����z����x�ɃJ�E���g���A�ő�l�𒴂�����ċA�I��
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
	//�\��̌o�H��\�����郁�\�b�h
	if (destiX != NOSELECT && destiY != NOSELECT) {
		int s = CHIP;
		int X = x, Y = y;

		//�ړI�n�܂ŏ���������
		while(X != destiX || Y != destiY) {
			//�ړ����������肵
			Dir d = decideDir(g, h, MaxHei, MaxWid, MaxHuman, X, Y);
			//���ۂɈړ�����
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
	//�ƂɍU�����郁�\�b�h
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
	//�`�惁�\�b�h
	if (life > 0) {	//�����Ă���҂̂ݕ`��
		drawChara(x - camX, y - camY); //��{�`��
		if (attackTarget != nullptr) { //�Վ��F�U���Ώە\��
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
	//�ړ���ƍU���������Z�b�g���郁�\�b�h
	destiX = X;
	destiY = Y;
	attackTarget = H;	//NOSELECT�Ȃ�ҋ@
	interval = maxInterval;
}


//Evancuee------------------------------------
Evancuee::Evancuee(int X, int Y) : Human(X, Y, CITIZEN) {}

void Evancuee::move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//�ړ����\�b�h
	if (life > 0) {
		if (g[y][x]->getStance() == Gimmick::ROAD || g[y][x]->getStatus() == Gimmick::BROKEN) {
			//�Ŋ��̉����瓦��������
			if (interval <= 0 && moveDir(g, h, MaxHei, MaxWid, MaxHuman)) interval += maxInterval;
			interval -= getTypeInfo(type).speed;
		}
	} else {
		x = y = 0;
	}
}

bool Evancuee::moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//�Ŋ��̉����瓦���������Ԃ�
	bool roadMap[MAXIMUM][MAXIMUM], fireMap[MAXIMUM][MAXIMUM], humanMap[MAXIMUM][MAXIMUM];
	//���Ȃ��true�A�R���Ă���Ȃ��true�A�l��������true
	int way[MAXIMUM][MAXIMUM];
	int nearFireDis = NOSELECT, nearFireX = NOSELECT, nearFireY = NOSELECT; //�Ŋ��̉��̏��
	int dx[5] = {x, x, x + 1, x, x - 1};//HERE���܂߂�5������X,Y���W
	int dy[5] = {y, y - 1, y, y + 1, y};
	Dir d = HERE; //�ŏI���肵���ړ�����

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

	//�쐬�͈͎͂b��I��10�}�X
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
	//�Ŋ��̉��ɍs��������Ȃ��������݂��󂢂Ă���΁A������̕�����ڎw��
	for (int i = 1; i < 5; i++) {
		if (roadMap[dy[i]][dx[i]] && !humanMap[dy[i]][dx[i]] && way[dy[i]][dx[i]] < 0) {
			d = (Dir)i;
			break;
		}
	}
	//����������΁A�Ŋ��̉����牓����������֌�����
	if (d == HERE) {
		for (int i = 1; i < 5; i++) {
			if (way[dy[d]][dx[d]] < way[dy[i]][dx[i]] && !humanMap[dy[i]][dx[i]]) d = (Dir)i;
		}
	}
	//���̕����ɐl�������瓮���Ȃ�
	if (humanMap[dy[d]][dx[d]]) d = HERE;


	//���ۂɈړ�����
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

	//����max�}�X�܂Œ��ׂ�
	if (n <= max) {
		for (int i = 1; i < 5; i++) {
			if (roadMap[dy[i]][dx[i]] && (way[dy[i]][dx[i]] == -1 || way[dy[i]][dx[i]] > n)) {
				makeMap(roadMap, fireMap, way, dx[i], dy[i], max, n);
			}
		}
	}
}

void Evancuee::draw(int camX, int camY) {
	if (life > 0) drawChara(x - camX, y - camY); //��{�`��
}


//Incendiary------------------------------------
Incendiary::Incendiary(int X, int Y) : Human(X, Y, INCENDIARY) {
	beenArrested = false;
	fireInter = fireInterMax;
}

void Incendiary::move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//�ړ����\�b�h
	if (life > 0) {
		if (g[y][x]->getStance() == Gimmick::ROAD || g[y][x]->getStatus() == Gimmick::BROKEN) {
			if (interval <= 0) {
				//�O��̕��΂���\���Ɏ��Ԃ��o���Ă���A���͂ɔR�₹��Ƃ�����Ȃ���΂���
				if (--fireInter <= 0 && fireHouse(g)) fireInter = fireInterMax;
				//�����łȂ���Έړ�����
				else if(moveDir(g, h, MaxHei, MaxWid, MaxHuman)) interval += maxInterval;
			}
			interval -= getTypeInfo(type).speed;
		}
	} else {
		x = y = 0;
	}
}

bool Incendiary::moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//�����Ɖ����瓦���A�R���Ă��Ȃ��Ƃ�ڎw���������A��
	//���Ȃ��true�A������������true�A�l��������true
	bool road[MAXIMUM][MAXIMUM], human[MAXIMUM][MAXIMUM], member[MAXIMUM][MAXIMUM];
	int map[MAXIMUM][MAXIMUM]; //�}�X���̓��_�i�D�揇�ʁj��\�킷�z��
	//HERE���܂߂�5������X,Y���W
	int dx[5] = {x, x, x + 1, x, x - 1};
	int dy[5] = {y, y - 1, y, y + 1, y};
	bool isChecked[5] = {false, false, false, false, false}; //�l�ƂԂ��邩�m�F�������ۂ��@�Ō�̏����Ɏg�p
	Dir d; //�ŏI���肵���ړ�����

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

	//�}�b�v�ɒl��^����
	//�_���c�����F-400000�A���F-1000�A�R���Ă��Ȃ��ƂɗאځF�e+10000
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			if (road[y][x]) {
				int score = 0;
				if (member[y][x])
					score += -400000;
				if (g[y][x]->getStatus() == Gimmick::FIRED)
					score += -1000;
				//normal�̉Ƃɗאڂ��Ă�����+10000
				for (int i = 1; i < 5; i++) {
					House *h = (House*)g[dy[i] - this->y + y][dx[i] - this->x + x];
					if (h->getStance() == Gimmick::HOUSE && h->getStatus() == Gimmick::NORMAL &&
						!h->getTypeInfo().isKey)
						score += 10000;
				}

				//���_���g�U������
				if (score != 0) {
					bool checked[MAXIMUM][MAXIMUM]; //���̊֐����ŁA�v�Z���I�������ǂ���
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
		//�ł��_�������������֐i��
		for (int i = 1; i < 5; i++) 
			if (!isChecked[i] && road[dy[i]][dx[i]] && map[dy[i]][dx[i]] > map[dy[(int)d]][dx[(int)d]]) d = (Dir)i;
		isChecked[d] = true;
	} while (d != HERE && human[dy[d]][dx[d]]);


	//���ۂɈړ�����
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

	if (!score) return;		//score��0�ɂȂ�����I��
	map[Y][X] += score;		//score�����Z
	checked[Y][X] = true;	//�`�F�b�N����

	for (int i = 1; i < 5; i++) {
		int x = dx[i];
		int y = dy[i];
		//���ł���Ȃ܂����Z����Ă��Ȃ���score�𔼌����ēn��
		if (road[y][x] && !checked[y][x]) makeMap(road, map, checked, x, y, score / 2);
	}
}

bool Incendiary::fireHouse(Gimmick *gim[MAXIMUM][MAXIMUM]) {
	//�אڂ����1�ɕ��΂��郁�\�b�h
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
			if (h->getStance() == Gimmick::HOUSE &&	h->getStatus() == Gimmick::NORMAL && !h->getTypeInfo().isKey) { //�L�[�Ƃɂ͕��΂��Ȃ�
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
	if (life > 0) drawChara(x - camX, y - camY); //��{�`��
}


//Thief------------------------------------
Thief::Thief(int X, int Y) : Human(X, Y, THIEF) {
	beenArrested = false;
	desireMoney = 2;
}

void Thief::move(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//�ړ����\�b�h
	if (life > 0) {
		if (interval <= 0 && (g[y][x]->getStance() == Gimmick::ROAD || g[y][x]->getStatus() == Gimmick::BROKEN)) {
			if (exit(g) || stealHouse(g) || moveDir(g, h, MaxHei, MaxWid, MaxHuman)) interval += maxInterval;
		} else interval -= getTypeInfo(type).speed;
	} else {
		x = y = 0;
	}
}

bool Thief::moveDir(Gimmick *g[MAXIMUM][MAXIMUM], Human *h[MAXIMUM], int MaxHei, int MaxWid, int MaxHuman) {
	//�����Ɖ����瓦���A�R���Ă��Ȃ��Ƃ�ڎw���������A��
	//���Ȃ��true�A������������true�A�l��������true
	bool road[MAXIMUM][MAXIMUM], human[MAXIMUM][MAXIMUM], member[MAXIMUM][MAXIMUM];
	int map[MAXIMUM][MAXIMUM]; //�}�X���̓��_�i�D�揇�ʁj��\�킷�z��
	//HERE���܂߂�5������X,Y���W
	int dx[5] = {x, x, x + 1, x, x - 1};
	int dy[5] = {y, y - 1, y, y + 1, y};
	bool isChecked[5] = {false, false, false, false, false}; //�l�ƂԂ��邩�m�F�������ۂ��@�Ō�̏����Ɏg�p
	Dir d; //�ŏI���肵���ړ�����

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

	//�}�b�v�ɒl��^����
	//�_���c�����F-400000�A���F-5000�A���̂���� or ��ɗאځF�e+200000 or +500000
	for (int y = 0; y < MaxHei; y++) {
		for (int x = 0; x < MaxWid; x++) {
			if (road[y][x]) {
				int score = 0;
				if (member[y][x])							score += -400000;
				if (g[y][x]->getStatus() == Gimmick::FIRED)	score += -5000;

				//�m���}��B�����Ă��Ȃ����
				if (desireMoney > 0) {
					//�������̉Ƃɗאڂ��Ă�����+200000
					for (int i = 1; i < 5; i++) {
						House *h = (House*)g[dy[i] - this->y + y][dx[i] - this->x + x];
						if (h->getStance() == Gimmick::HOUSE && h->getRich())
							score += 200000;
					}
					//�m���}��B�����Ă�����
				} else {
					//��ɗאڂ��Ă�����+500000
					for (int i = 1; i < 5; i++) {
						Wall *w = (Wall*)g[dy[i] - this->y + y][dx[i] - this->x + x];
						if (w->getStance() == Gimmick::WALL && w->getType() == Wall::GATE)
							score += 500000;
					}
				}

				//���_���g�U������
				if (score != 0) {
					bool checked[MAXIMUM][MAXIMUM]; //���̊֐����ŁA�v�Z���I�������ǂ���
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
		//�ł��_�������������֐i��
		for (int i = 1; i < 5; i++) 
			if (!isChecked[i] && road[dy[i]][dx[i]] && map[dy[i]][dx[i]] > map[dy[(int)d]][dx[(int)d]]) d = (Dir)i;
		isChecked[d] = true;
	} while (d != HERE && human[dy[d]][dx[d]]);


	//���ۂɈړ�����
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

	if (!score) return;		//score��0�ɂȂ�����I��
	map[Y][X] += score;		//score�����Z
	checked[Y][X] = true;	//�`�F�b�N����

	for (int i = 1; i < 5; i++) {
		int x = dx[i];
		int y = dy[i];
		//���ł���Ȃ܂����Z����Ă��Ȃ���score�𔼌����ēn��
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
	if (life > 0) drawChara(x - camX, y - camY); //��{�`��
}
