/*
  Tetris ���� ���� �ڵ�
  - ��� ���� ���� �� �̵�
  - �浹 ���� �� ���� ����
  - ���� �� ���� ����
  - ���� ���� �� ��ŷ �ý��� ����
  - tabŰ ������ ���� ���� ��ϰ� ���� �ٲٱ�(�߰����)
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include "block.h"
#define STAGE_HEIGHT 26
#define STAGE_WIDTH 20

// ��ŷ 10������ ����
#define RANK 10
int ranking[RANK] = { 0 };

void eraseCursor(); // �ܼ��� Ŀ���� ����� �Լ�
void drawStage(); // ������ �⺻���� �������� Ʋ�� �׸��� �Լ�
int selectBlock(); // ����� �������� �����ϴ� �Լ�
void drawBlock(int x, int y, int blockNum); // ���õ� ����� �������� ���� �׸��� �Լ�
void eraseBlock(int x, int y, int blockNum); // ���õ� ����� ����� �Լ�
void keyBoardInput(int* x, int* y, int* blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT], int* nextBlockNum); // Ű���� �Է��� ó���ϴ� �Լ�
void stageInfo(int x, int y, int blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT], int curScore); // ����� ���������� ���� �� ���� ������Ʈ
int detectCollision(int x, int y, int blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT]); // �浹 ���θ� �����ϴ� �Լ�
void removeFullLines(int stage[STAGE_WIDTH][STAGE_HEIGHT], int* curScore); // ���� �� ������ �����ϰ� ���� ������Ʈ
void reDrawBlock(int stage[STAGE_WIDTH][STAGE_HEIGHT]); // ����� �ٽ� �׸��� �Լ�
void showNextBlock(int nextBlockNum); // ������ ��Ÿ�� ����� �����ִ� �Լ�
void removeNextBlock(int nextBlockNum); // ���� ��� ������ ȭ�鿡�� �����ϴ� �Լ�
void showScore(int curScore); // ���� ������ ����ϴ� �Լ�
void gameOver(int curScore); // ���� ���� ó���� �ϴ� �Լ�
int detectGameOver(int stage[STAGE_WIDTH][STAGE_HEIGHT]); // ���� ���� ���θ� Ȯ���ϴ� �Լ�
void updateRanking(int curScore); // ������ ��ŷ�� �ݿ��ϴ� �Լ�
void showRanking(); // ���� ��ŷ�� ����ϴ� �Լ�
void showLevelUp(int level); // ���� �� ������ ����ϴ� �Լ�
void runGame(int* level);// ���� ���� ���� �Լ�

int main() {
	srand((unsigned int)time(NULL)); // ���� ��� ������ ���� �õ�
	eraseCursor(); // ���� �ʱ� ����(Ŀ�� ����)
	drawStage(); // ���� �� �����
	int level = 1; // ���� ����

	// ���� ����
	runGame(&level);
	return 0;
}

// �ܼ� Ŀ���� ����� �Լ�
void eraseCursor() {
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO curCursorInfo;
	GetConsoleCursorInfo(hConsoleOut, &curCursorInfo);
	curCursorInfo.bVisible = 0; // Ŀ�� ����
	SetConsoleCursorInfo(hConsoleOut, &curCursorInfo);
}

// ���������� �׸��� ����, ���̵�, ���� ��� �� ��ŷ�κ��� ǥ��
void drawStage() {
	COORD pos = { 0, 0 };
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// ������ �׵θ�, ���� ���, ����, ����, ��ŷ �κ� ǥ��
	pos.X = 0;
	pos.Y = STAGE_HEIGHT;
	for (int i = 0; i < STAGE_WIDTH+9; i++) {
		pos.X++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("��");
	}
	pos.Y = 5;
	pos.X = STAGE_WIDTH;
	for (int i = 0; i < 9; i++) {
		pos.X++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("��");
	}
	pos.Y += 4;
	pos.X = STAGE_WIDTH;
	for (int i = 0; i < 9; i++) {
		pos.X++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("��");
	}
	pos.X = 0;
	pos.Y = 0;
	for (int i = 0; i < STAGE_HEIGHT; i++) {
		pos.Y++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("��");
	}
	pos.X = STAGE_WIDTH;
	pos.Y = 0;
	for (int i = 0; i < STAGE_HEIGHT; i++) {
		pos.Y++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("��");
	}
	pos.X = STAGE_WIDTH+9;
	pos.Y = 0;
	for (int i = 0; i < STAGE_HEIGHT; i++) {
		pos.Y++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("��");
	}
	pos.Y = 5;
	pos.X = STAGE_WIDTH+1;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("NEXT");
	pos.Y = 6;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("LEVEL:%d", 0);
	pos.Y = 7;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("SCORE");
	pos.Y = 8;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("0");
	pos.Y = 10;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("RANKING");
	
	// ��ŷ ǥ��
	showRanking();
}

// ����� �������� ����
int selectBlock() {
	int r = rand() % 7;
	return r * 4;
}

// ����� �׸��� ���� �ֿܼ� ���
void drawBlock(int x, int y, int blockNum) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[blockNum][i][j] == 1) {
				pos.X = 2 * (x + j);
				pos.Y = y + i;
				SetConsoleCursorPosition(hConsoleOut, pos);
				printf("��"); // ����� �ֿܼ� ���
			}
		}
	}
}

// ����� ����� ���� �� �������� ���
void eraseBlock(int x, int y, int blockNum) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[blockNum][i][j] == 1) {
				pos.X = 2 * (x + j);
				pos.Y = y + i;
				SetConsoleCursorPosition(hConsoleOut, pos);
				printf(" "); // ����� ����
			}
		}
	}
}

// Ű���� �Է��� �޾� ����� �̵�, ȸ��, �ѹ��� ������, ��� �ٲٱ�
void keyBoardInput(int* x, int* y, int* blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT], int* nextBlockNum) {
	int key;

	while (_kbhit()) {
		key = _getch();

		// ����Ű �� ȸ�� Ű ó��
		if (key == 224) {
			key = _getch();

			// ���� ȭ��ǥ: ���� �̵�
			if (key == 75) {
				(*x)--;
				if (detectCollision(*x, *y, *blockNum, stage)) {
					(*x)++;
				}
			}

			// ������ ȭ��ǥ: ���� �̵�
			else if (key == 77) {
				(*x)++;
				if (detectCollision(*x, *y, *blockNum, stage)) {
					(*x)--;
				}
			}

			// �Ʒ� ȭ��ǥ: �Ʒ��� �̵�
			else if (key == 80) {
				(*y)++;
				if (detectCollision(*x, *y, *blockNum, stage)) {
					(*y)--;
				}
			}

			// ���� ȭ��ǥ: ��� ȸ��
			else if (key == 72) {
				(*blockNum)++;
				if ((*blockNum) % 4 == 0) {
					(*blockNum) -= 4;
				}
				if (detectCollision(*x, *y, *blockNum, stage)) {
					(*blockNum)--;
				}
			}
		}

		// �����̽���: ����� ��� ��������
		else if (key == 32) {
			while (!detectCollision(*x, *y, *blockNum, stage)) {
				(*y)++;
			}
		}

		// �� Ű: ��� ��ü
		else if (key == 9) {
			removeNextBlock(*nextBlockNum);
			int temp = *blockNum;
			*blockNum = *nextBlockNum;
			*nextBlockNum = temp;

			// �ٲ� ���� ����� ǥ��
			showNextBlock(*nextBlockNum);
		}
	}
}

// ���������� ��� ������ ����
void stageInfo(int x, int y, int blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT], int curScore) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[blockNum][i][j] == 1) {
				int row = x + j;
				int col = y + i;

				// ���� ���� ����
				if (col <= 2) { 
					gameOver(curScore);
				}

				// ���������� ��� ����
				stage[row][col] = 1;
			}
		}
	}
}

// ��� �浹 ����
int detectCollision(int x, int y, int blockNum, int stage[STAGE_WIDTH][STAGE_HEIGHT]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[blockNum][i][j] == 1) {
				int newX = x + j;
				int newY = y + i;

				// �浹 �߻� ����(���� ��ų�, �ٸ� ��Ͽ� �����ų�)
				if (newX < 1 ||
					newX >= STAGE_WIDTH / 2 ||
					newY >= STAGE_HEIGHT - 1 ||
					stage[newX][newY] == 1) {
					return 1;
				}
			}

		}
	}

	// �浹 ����
	return 0;
}

// �ϼ��� ���� ����� ������ ����
void removeFullLines(int stage[STAGE_WIDTH][STAGE_HEIGHT], int* curScore) {
	int conq = 1;
	for (int i = STAGE_HEIGHT - 1; i >= 0; i--) {
		int fullLine = 1;

		// �� ���� ���� á���� Ȯ��
		for (int j = 1; j < STAGE_WIDTH / 2; j++) {
			if (stage[j][i] == 0) {
				fullLine = 0;
				break;
			}
		}

		// ���� �� ���� ����� ���� ����� �Ʒ��� ����
		if (fullLine) {
			for (int j = 1; j < STAGE_WIDTH / 2; j++) {
				stage[j][i] = 0;
			}

			// ���� �߰�(�������� ���� �ѹ��� ���ݴٸ� ���ʽ� ���� �ο�
			for (int e=0; e < conq; e++) {
				*curScore += 100;
			}
			conq++;

			// ���� ����� ���� ����� �Ʒ��� �̵�
			for (int k = i; k > 0; k--) {
				for (int j = 1; j < STAGE_WIDTH / 2; j++) {
					stage[j][k] = stage[j][k - 1];
				}
			}
			i++;
		}
	}
}

// ���������� �׷��� ����� �ٽ� �׸�
void reDrawBlock(int stage[STAGE_WIDTH][STAGE_HEIGHT]) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// �������� ����� �ٽ� �׷� ȭ�鿡 ǥ��
	for (int i = 1; i < STAGE_WIDTH / 2; i++) {
		for (int j = 0; j < STAGE_HEIGHT - 1; j++) {
			pos.X = 2 * i;
			pos.Y = j + 1;
			SetConsoleCursorPosition(hConsoleOut, pos);
			if (stage[i][j] == 0) {
				printf(" ");
			}
			else {
				printf("��");
			}
		}
	}
}

// ���� ����� ȭ�鿡 ǥ��
void showNextBlock(int nextBlockNum){
	int x = STAGE_WIDTH / 2 + 1;
	int y = 1;
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// ���� ����� ��ǥ�� �°� ���
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[nextBlockNum][i][j] == 1) {
				pos.X = 2 * (x + j);
				pos.Y = y + i;
				SetConsoleCursorPosition(hConsoleOut, pos);
				printf("��");
			}
		}
	}
}

// ���� ����� ������ ȭ�鿡�� ����
void removeNextBlock(int nextBlockNum)
{
	int x = STAGE_WIDTH / 2 + 1;
	int y = 1;
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// ���� ����� �� �������� ���� ����
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blockModel[nextBlockNum][i][j] == 1) {
				pos.X = 2 * (x + j);
				pos.Y = y + i;
				SetConsoleCursorPosition(hConsoleOut, pos);
				printf(" ");
			}
		}
	}
}

// ���� ������ ȭ�鿡 ǥ��
void showScore(int curScore) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	pos.X = STAGE_WIDTH + 1;
	pos.Y = 8;
	SetConsoleCursorPosition(hConsoleOut, pos);

	// ���� ���
	printf("%d", curScore);
}

// ���� ���� �� �޽��� ��� �� ��ŷ ������Ʈ, ����� ���� Ȯ��
void gameOver(int curScore) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	
	// ���ӿ���, ����� �޼��� ���
	pos.X = STAGE_WIDTH / 2 - 4;
	pos.Y = STAGE_HEIGHT / 2;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("GAME OVER!");
	pos.X -= 2;
	pos.Y += 2;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("Press Y Button");
	pos.X += 2;
	pos.Y += 2;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("to Restart ");
	pos.X += 1;
	pos.Y += 2;
	SetConsoleCursorPosition(hConsoleOut, pos);
	printf("the Game");

	// ���� ��ŷ ǥ��
	showRanking();

	char restart = _getch();

	//  ����۽�
	if (restart == 'Y' || restart == 'y') {

		// ��ŷ ������Ʈ
		updateRanking(curScore);
		system("cls");
		int level = 0;

		// ���� ��ŷ ǥ�� �� ���� �缳��
		showRanking();
		eraseCursor();
		drawStage();
		runGame(&level);
	}
}

// ���� ���� ����, �������� ��ܿ� ����� �׿����� Ȯ��
int detectGameOver(int stage[STAGE_WIDTH][STAGE_HEIGHT]) {
	for (int i = 1; i < STAGE_WIDTH / 2; i++) {
		if (stage[i][0] == 1) { 
			// ���� ������ 1���
			return 1;
		}
	}

	// ���� ��� ���� ���ɽ� 0���
	return 0;
}

// ������ ���� ��ŷ�� ������Ʈ
void updateRanking(int curScore) {
	for (int i = 0; i < RANK; i++) {

		// ��ŷ ����
		if (curScore > ranking[i]) {
			for (int j = RANK - 1; j > i; j--) {
				ranking[j] = ranking[j - 1];
			}
			ranking[i] = curScore;
			break;
		}
	}
}

// ��ŷ�� ȭ�鿡 ���
void showRanking() {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

	pos.X = STAGE_WIDTH + 1;
	pos.Y = 11;
	SetConsoleCursorPosition(hConsoleOut, pos);

	// ��ŷ ������ ���
	for (int i = 0; i < RANK; i++) {
		pos.Y++;
		SetConsoleCursorPosition(hConsoleOut, pos);
		printf("%d: %d", i + 1, ranking[i]);  
	}
}

// ���� ������ ȭ�鿡 ǥ��
void showLevelUp(int level) {
	COORD pos;
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	pos.X = STAGE_WIDTH + 1;
	pos.Y = 6;
	SetConsoleCursorPosition(hConsoleOut, pos);

	// ���� ���
	printf("LEVEL: %d", level);
}

// ���� ����, ���� �� ���ھ�, ����� �����ϸ� ���� ���� ����
void runGame(int* level) {
	// ���� �ʱ� ����
	int stage[STAGE_WIDTH][STAGE_HEIGHT] = { 0 };
	int nextBlockNum = selectBlock();
	int curScore = 0;

	// ���� �ʱ�ȭ �� ���
	showScore(curScore);

	while (1) {
		int blockNum = nextBlockNum;

		// ���ο� ��� ����
		nextBlockNum = selectBlock();

		// ���� ��� ǥ��
		showNextBlock(nextBlockNum);

		// ��� ������ǥ
		int x = 4;
		int y = 1;

		while (1) {
			// ������ ���� ���� ����
			if (curScore <= 4500) {
				*level = 1 + (curScore / 500);
			}

			// ���� ���� ǥ��
			showLevelUp(*level);

			// ����� ����� �̵�
			eraseBlock(x, y, blockNum);
			y++;

			// �浹 ������
			if (detectCollision(x, y, blockNum, stage)) {
				// ����� ���� ��ġ�� ����
				y--;
				drawBlock(x, y, blockNum);
				
				// �������� ���� ������Ʈ
				stageInfo(x, y, blockNum, stage, curScore);

				// ������ ����� ���� ��
				if (detectGameOver(stage)) {
					// ���� ���� ó��
					gameOver(curScore);
					return ;
				}

				// �ϼ��� �� ����
				removeFullLines(stage, &curScore);

				// ���� ������Ʈ
				showScore(curScore);

				// �������� �ٽ� �׸�
				reDrawBlock(stage);
				break;
			}

			// ����� �׷��� �̵� ���� �ݿ�
			drawBlock(x, y, blockNum);
		
			// Ű���� �Է� ó��
			for (int i = 0; i < 20; i++) {
				if (_kbhit()) {
					eraseBlock(x, y, blockNum);
					keyBoardInput(&x, &y, &blockNum, stage, &nextBlockNum);

					// �浹 ���� �� �ٽ� ���� �ø�
					if (detectCollision(x, y, blockNum, stage)) y--;

					// ����� �ٽ� �׸�
					drawBlock(x, y, blockNum);
				}

				// ������ ���� �ӵ� ����
				Sleep(25-((*level)*2));
			}
		}

		// ���� ��� ����
		removeNextBlock(nextBlockNum);
	}
}