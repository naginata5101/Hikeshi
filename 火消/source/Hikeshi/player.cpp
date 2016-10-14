#include "common.h"

//Menu--------------------------------------------
Menu::Menu(Mode m, int X, int Y, Gimmick *aro[]) {
	//コンストラクタ
	x = X;
	y = Y;
	select = 0;
	mode = m;
	if (m == MEMBER) {
		for (int i = 0; i <= 5; i++) {	// 中心含めた周囲=5
			around[i] = aro[i];
		}
	}
}

void Menu::move(bool byMouse) {
	//選択の移動メソッド
	//引数のx,yは画面内の座標（スクロールは関係ない
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
				// マウスが「断念」の上にある場合のみ、「断念」を選択
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
			//上方向
			if (MikanInput->GetKeyNum(K_UP) == 1)	++select;
			//下方向
			if (MikanInput->GetKeyNum(K_DOWN) == 1)	--select;
			if (select < 0)			select = itemNum - 1;
			if (select >= itemNum)	select = 0;
			break;
		}
	}
}

int Menu::choice() {
	//選択の決定メソッド
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
	//描画メソッド
	//引数は呼び出したカーソルの座標
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
		MikanDraw->Printf(F_BY, (WIDTH - s * 3 * 3) / 2, 150, "小休止");

		MikanDraw->DrawBox(X, Y, W, itemNum * s, 0xFF000000);
		//選択中の項目に白要素を追加
		MikanDraw->DrawBox(X, Y + select * s, W, s, 0x88FFFFFF);

		MikanDraw->Printf(F_SW, X, Y + 0 * s, "　継　続　");
		MikanDraw->Printf(F_SW, X, Y + 1 * s, "　断　念　");


		//int s = CHIP;
		//int X = getStandX();
		//int Y = getStandY();

		//MikanDraw->DrawBox(X * s, Y * s, w * s, itemNum * CHARA0_H, 0xFF000000);
		////文字の描画
		//MikanDraw->Printf(F_SW, X * s, Y * s + 0 * CHARA0_H, "継続");
		//MikanDraw->Printf(F_SW, X * s, Y * s + 1 * CHARA0_H, "断念");
		////選択中の項目に白要素を追加
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
	//Ground型のメニューの描画の基点にするx座標を返すメソッド
	int s = CHIP;
	if (WIDTH < (x + 1 + w) * s) {
		return x - w;
	} else {
		return x + 1;
	}
}

int Menu::getStandY() {
	//Ground型のメニューの描画の基点にするy座標を返すメソッド
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
	//移動メソッド
	if (menu == nullptr) {
		//メニューを持っていない場合は、通常操作
		int inter = 15;
		//Xキー押しっぱなしで高速移動
		if (MikanInput->GetKeyNum(K_X) > 0)	inter = 5;
		const int MaxX = MaxWid - 1;
		const int MaxY = MaxHei - 1;

		if (byMouse) {
			x = getMouseX() + camX;
			y = getMouseY() + camY;
		} else {
			//キーボードに合わせて移動する
			if (y > 0 && MikanInput->GetKeyNum(K_UP) % inter == 1)			--y;
			if (x < MaxX && MikanInput->GetKeyNum(K_RIGHT) % inter == 1)	++x;
			if (y < MaxY && MikanInput->GetKeyNum(K_DOWN) % inter == 1)		++y;
			if (x > 0 && MikanInput->GetKeyNum(K_LEFT) % inter == 1)		--x;
		}

		//範囲外に出ないようにする
		if (x < 0)			x = 0;
		else if (x > MaxX)	x = MaxX;
		if (y < 0)			y = 0;
		else if (y > MaxY)	y = MaxY;
	} else {
		//メニューを持っている場合、操作を渡す
		menu->move(byMouse);
	}
}

void Cursor::setMember(int num) {
	select = num;
}

int Cursor::orderMember(Gimmick *around[], int camX, int camY) {
	//選択した隊員の番号を取得し、返り値で隊員に指示を与えるメソッド
	if (menu == nullptr) {
		if (select == NOSELECT) {
			//メニュー無＋選択中隊員無
			//地面用のメニューを生成
			menu = new Menu(Menu::GROUND, x - camX, y - camY, nullptr);
		} else if (around[HERE]->getStance() == Gimmick::ROAD) {
			//メニュー無＋選択中隊員有＋カーソル下が道
			//隊員用のメニューを生成
			menu = new Menu(Menu::MEMBER, x - camX, y - camY, around);
		}
	} else {
		//メニュー有ならば
		int choice = menu->choice();
		select = NOSELECT;
		delete menu;
		menu = nullptr;
		//メニューの選択結果を返す
		return choice;
	}
	//何も無ければNOSELECT
	return NOSELECT;
}

void Cursor::cancel() {
	//全てをキャンセルするメソッド
	if (menu != nullptr) {
		//メニュー有なら、それを解除して終了
		delete menu;
		menu = nullptr;
		return;
	}
	select = NOSELECT;
}

int Cursor::getX() {
	//カーソルのx座標を返すメソッド
	return x;
}

int Cursor::getY() {
	//カーソルのy座標を返すメソッド
	return y;
}

void Cursor::draw(int camX, int camY) {
	//描画メソッド
	int s = CHIP;
	int X = (x - camX) * s;
	int Y = (y - camY + SPACE) * s;

	//デフォルトは白
	if (select == NOSELECT)	MikanDraw->DrawBox(X, Y, s, s, 0x88FFFFFF);
	//選択中は淡い緑色に
	else					MikanDraw->DrawBox(X, Y, s, s, 0x8800FF00);

	// 上に枠を表示
	MikanDraw->DrawTexture(T_CHIPS, X, Y, 3 * s, 4 * s, s, s);

	//メニューを持っている時は、そっちも描画する
	if (menu != nullptr) menu->draw();
}

int Cursor::getMember() {
	//選択中の隊員の番号を返すメソッド
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
