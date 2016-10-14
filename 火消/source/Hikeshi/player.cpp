#include "common.h"

//Menu--------------------------------------------
Menu::Menu(Mode m, int X, int Y, Gimmick *aro[]) {
	//�R���X�g���N�^
	x = X;
	y = Y;
	select = 0;
	mode = m;
	if (m == MEMBER) {
		for (int i = 0; i <= 5; i++) {	// ���S�܂߂�����=5
			around[i] = aro[i];
		}
	}
}

void Menu::move(bool byMouse) {
	//�I���̈ړ����\�b�h
	//������x,y�͉�ʓ��̍��W�i�X�N���[���͊֌W�Ȃ�
	switch (mode) {
	case Menu::MEMBER:
		if (byMouse) {
			int X = getMouseX();
			int Y = getMouseY();
			if (x == X && y == Y)	select = HERE;
			if (x == X && y - 1 == Y && checkAttackable(UP))	select = UP;
			if (x + 1 == X && y == Y && checkAttackable(RIGHT))	select = RIGHT;
			if (x == X && y + 1 == Y && checkAttackable(DOWN))	select = DOWN;
			if (x - 1 == X && y == Y && checkAttackable(LEFT))	select = LEFT;
		} else {
			if (MikanInput->GetKeyNum(K_UP) == 1) {
				if (select == DOWN) {
					select = HERE;
				} else if (checkAttackable(UP)) {
					select = UP;
				}
			}
			if (MikanInput->GetKeyNum(K_RIGHT) == 1) {
				if (select == LEFT) {
					select = HERE;
				} else if (checkAttackable(RIGHT)) {
					select = RIGHT;
				}
			}
			if (MikanInput->GetKeyNum(K_DOWN) == 1) {
				if (select == UP) {
					select = HERE;
				} else if (checkAttackable(DOWN)) {
					select = DOWN;
				}
			}
			if (MikanInput->GetKeyNum(K_LEFT) == 1) {
				if (select == RIGHT) {
					select = HERE;
				} else if (checkAttackable(LEFT)) {
					select = LEFT;
				}
			}
		}
		break;

	case Menu::GROUND:
		if (byMouse) {
			int mx = MikanInput->GetMousePosX();
			int my = MikanInput->GetMousePosY();
			const int s = CHIP;
			const int W = w * s;
			const int X = (WIDTH - W) /2;
			const int Y = 300;
			if (mx >= X && mx < X + W && my >= Y + s && my < Y + s + s)
				// �}�E�X���u�f�O�v�̏�ɂ���ꍇ�̂݁A�u�f�O�v��I��
				select = 1;
			else	select = 0;
			/*
			int mx = getMouseX();
			int my = getMouseY();
			int sx = getStandX();
			int sy = getStandY() - SPACE;
			if (X >= sx && X < sx + w * CHIP) {
				for (int i = 0; i < itemNum; i++) {
					if (Y >= sy + i && Y < sy + i + 1) {
						select = i;
						break;
					}
				}
			}
			*/
		} else {
			int itemNum = 2;
			//�����
			if (MikanInput->GetKeyNum(K_UP) == 1)	++select;
			//������
			if (MikanInput->GetKeyNum(K_DOWN) == 1)	--select;
			if (select < 0)			select = itemNum - 1;
			if (select >= itemNum)	select = 0;
			break;
		}
	}
}

int Menu::choice() {
	//�I���̌��胁�\�b�h
	switch (mode)
	{
	case Menu::MEMBER:
		return select;
		break;
	case Menu::GROUND:
		if (select == 1)	goPreTitle();
		break;
	}
	return NOSELECT;
}

int Menu::getSelect() {
	return select;
}

Menu::Mode Menu::getMode() {
	return mode;
}

void Menu::draw() {
	//�`�惁�\�b�h
	//�����͌Ăяo�����J�[�\���̍��W
	if (mode == MEMBER) {
		drawAttackable(x, y, HERE);
		drawAttackable(x, y - 1, UP);
		drawAttackable(x + 1, y, RIGHT);
		drawAttackable(x, y + 1, DOWN);
		drawAttackable(x - 1, y, LEFT);
	} else {
		const int s = CHIP;
		const int W = w * s;
		const int X = (WIDTH - W) /2;
		const int Y = 300;

		MikanDraw->DrawBox(0, 0, WIDTH, HEIGHT, 0x88000000);
		MikanDraw->Printf(F_BY, (WIDTH - s * 3 * 3) / 2, 150, "���x�~");

		MikanDraw->DrawBox(X, Y, W, itemNum * s, 0xFF000000);
		//�I�𒆂̍��ڂɔ��v�f��ǉ�
		MikanDraw->DrawBox(X, Y + select * s, W, s, 0x88FFFFFF);

		MikanDraw->Printf(F_SW, X, Y + 0 * s, "�@�p�@���@");
		MikanDraw->Printf(F_SW, X, Y + 1 * s, "�@�f�@�O�@");


		//int s = CHIP;
		//int X = getStandX();
		//int Y = getStandY();

		//MikanDraw->DrawBox(X * s, Y * s, w * s, itemNum * CHARA0_H, 0xFF000000);
		////�����̕`��
		//MikanDraw->Printf(F_SW, X * s, Y * s + 0 * CHARA0_H, "�p��");
		//MikanDraw->Printf(F_SW, X * s, Y * s + 1 * CHARA0_H, "�f�O");
		////�I�𒆂̍��ڂɔ��v�f��ǉ�
		//MikanDraw->DrawBox(X * s, Y * s + select * CHARA0_H, w * s, CHARA0_H, 0x88FFFFFF);
	}
}

void Menu::drawAttackable(int X, int Y, int dir) {
	int s = CHIP;
	if (dir == select) {
		MikanDraw->DrawBox(X * s, (Y + SPACE) * s, s, s, 0xAAFF0000);
		if (dir != HERE)	((House*)around[dir])->drawLife(X, Y + SPACE);
	} else if (checkAttackable(dir) == true) {
		MikanDraw->DrawBox(X * s, (Y + SPACE) * s, s, s, 0x880000FF);
	}
}

bool Menu::checkAttackable(int dir) {
	if (around[dir] != nullptr &&
		around[dir]->getStance() == Gimmick::HOUSE &&
		around[dir]->getStatus() != Gimmick::ERASED) {
			return true;
	}
	return false;
}

int Menu::getStandX() {
	//Ground�^�̃��j���[�̕`��̊�_�ɂ���x���W��Ԃ����\�b�h
	int s = CHIP;
	if (WIDTH < (x + 1 + w) * s) {
		return x - w;
	} else {
		return x + 1;
	}
}

int Menu::getStandY() {
	//Ground�^�̃��j���[�̕`��̊�_�ɂ���y���W��Ԃ����\�b�h
	int s = CHIP;
	if (HEIGHT < (y + SPACE) * s + itemNum * CHARA0_H) {
		return y + SPACE - itemNum * (CHARA0_H / s) + 1;
	} else {
		return y + SPACE;
	}
}

//Cursor-----------------------------------------
Cursor::Cursor() {
	x = mWID / 2 - 1;
	y = mHEI / 2;
	select = NOSELECT;
	menu = nullptr;
}

void Cursor::move(bool byMouse, Gimmick::Stance around[5], int MaxWid, int MaxHei, int camX, int camY) {
	//�ړ����\�b�h
	if (menu == nullptr) {
		//���j���[�������Ă��Ȃ��ꍇ�́A�ʏ푀��
		int inter = 15;
		//X�L�[�������ςȂ��ō����ړ�
		if (MikanInput->GetKeyNum(K_X) > 0)	inter = 5;
		const int MaxX = MaxWid - 1;
		const int MaxY = MaxHei - 1;

		if (byMouse) {
			x = getMouseX() + camX;
			y = getMouseY() + camY;
		} else {
			//�L�[�{�[�h�ɍ��킹�Ĉړ�����
			if (y > 0 && MikanInput->GetKeyNum(K_UP) % inter == 1)			--y;
			if (x < MaxX && MikanInput->GetKeyNum(K_RIGHT) % inter == 1)	++x;
			if (y < MaxY && MikanInput->GetKeyNum(K_DOWN) % inter == 1)		++y;
			if (x > 0 && MikanInput->GetKeyNum(K_LEFT) % inter == 1)		--x;
		}

		//�͈͊O�ɏo�Ȃ��悤�ɂ���
		if (x < 0)			x = 0;
		else if (x > MaxX)	x = MaxX;
		if (y < 0)			y = 0;
		else if (y > MaxY)	y = MaxY;
	} else {
		//���j���[�������Ă���ꍇ�A�����n��
		menu->move(byMouse);
	}
}

void Cursor::setMember(int num) {
	select = num;
}

int Cursor::orderMember(Gimmick *around[], int camX, int camY) {
	//�I�����������̔ԍ����擾���A�Ԃ�l�ő����Ɏw����^���郁�\�b�h
	if (menu == nullptr) {
		if (select == NOSELECT) {
			//���j���[���{�I�𒆑�����
			//�n�ʗp�̃��j���[�𐶐�
			menu = new Menu(Menu::GROUND, x - camX, y - camY, nullptr);
		} else if (around[HERE]->getStance() == Gimmick::ROAD) {
			//���j���[���{�I�𒆑����L�{�J�[�\��������
			//�����p�̃��j���[�𐶐�
			menu = new Menu(Menu::MEMBER, x - camX, y - camY, around);
		}
	} else {
		//���j���[�L�Ȃ��
		int choice = menu->choice();
		select = NOSELECT;
		delete menu;
		menu = nullptr;
		//���j���[�̑I�����ʂ�Ԃ�
		return choice;
	}
	//�����������NOSELECT
	return NOSELECT;
}

void Cursor::cancel() {
	//�S�Ă��L�����Z�����郁�\�b�h
	if (menu != nullptr) {
		//���j���[�L�Ȃ�A������������ďI��
		delete menu;
		menu = nullptr;
		return;
	}
	select = NOSELECT;
}

int Cursor::getX() {
	//�J�[�\����x���W��Ԃ����\�b�h
	return x;
}

int Cursor::getY() {
	//�J�[�\����y���W��Ԃ����\�b�h
	return y;
}

void Cursor::draw(int camX, int camY) {
	//�`�惁�\�b�h
	int s = CHIP;
	int X = (x - camX) * s;
	int Y = (y - camY + SPACE) * s;

	//�f�t�H���g�͔�
	if (select == NOSELECT)	MikanDraw->DrawBox(X, Y, s, s, 0x88FFFFFF);
	//�I�𒆂͒W���ΐF��
	else					MikanDraw->DrawBox(X, Y, s, s, 0x8800FF00);

	// ��ɘg��\��
	MikanDraw->DrawTexture(T_CHIPS, X, Y, 3 * s, 4 * s, s, s);

	//���j���[�������Ă��鎞�́A���������`�悷��
	if (menu != nullptr) menu->draw();
}

int Cursor::getMember() {
	//�I�𒆂̑����̔ԍ���Ԃ����\�b�h
	return select;
}

Menu* Cursor::getMenu() {
	return menu;
}

bool Cursor::getHavePause() {
	if (menu != nullptr && menu->getMode() == Menu::Mode::GROUND)
		return true;
	return false;
}
