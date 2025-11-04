#include <iostream>
#include <conio.h>
#include <windows.h>
#include <random>
using namespace std;

random_device rd;
default_random_engine dre(rd());
uniform_int_distribution<int> uid(2, 27), way(0,5), go_num(1,5);

typedef struct Board {
	BOOL block, open;
}Board;
int Size_x = 30, Size_y = 30;
Board board[30][30];

int player_x, player_y, switch_r;

void print_board() {
	cout << "   ";
	for (int i = 0; i < Size_x; ++i) {
		if (i < 10)
			cout << ' ' << i;
		else
			cout  << i;
	}
	cout << "\n";
	for (int j = 0; j < Size_y; ++j) {
		if (j < 10)
			cout << j << "  ";
		else
			cout << j << ' ';
		for (int i = 0; i < Size_x; ++i) {
			if (switch_r && i == player_x && player_y == j) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
				cout << " *";
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			}
			else if (board[i][j].block) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				cout << " x";
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			}
			else if (board[i][j].open) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
				cout << " 0";
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			}
			else
				cout << " 1";
		}
		cout << '\n';
	}
}
void gotoxy(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
void creat_road() {
	int x = 0, y = 0, pre_dir = -1;
	while (1) {
		int dir = way(dre);
		int how_many = go_num(dre);
		if (pre_dir == -1) {
			while (dir % 2 == 0)
				dir = way(dre);
			if (dir == 4)
				dir = 1;
			else if (dir == 5)
				dir = 3;
		}
		else {
			if (pre_dir == 0)
				dir = 1;
			else if (pre_dir == 2)
				dir = 3;
			else if (x != 29 && board[x + 1][y].block == true) {
				dir = 2;
				how_many = 1;
			}
			else if (y != 29 && board[x][y + 1].block == true) {
				dir = 0;
				how_many = 1;
			}
			else if (x != 0 && board[x - 1][y].block == true) {
				dir = 1;
				how_many = 1;
			}
			else if (y != 0 && board[x][y - 1].block == true) {
				dir = 3;
				how_many = 1;
			}
			else {
				while (1) {
					if (dir == 4)
						dir = 1;
					else if (dir == 5)
						dir = 3;
					if (dir == pre_dir)
						dir = way(dre);
					else if (y == 0 && dir == 0)
						dir = way(dre);
					else if (x == 0 && dir == 2)
						dir = way(dre);
					else if (y == 29 && dir == 3)
						dir = way(dre);
					else if (x == 29 && dir == 1)
						dir = way(dre);
					else
						break;
				}
			}
		}
		switch (dir) {
		case 0:
			for (int i = 0; i < how_many; ++i) {
				if (x == 29 && y == 29)
					break;
				else if (y == 0)
					break;
				else if (x != 0 && !board[x][y - 1].block)
					board[x][--y].open = true;
				else
					break;
			}
			break;
		case 2:
			for (int i = 0; i < how_many; ++i) {
				if (x == 29 && y == 29)
					break;
				else if (x == 0)
					break;
				else if (x != 0 && !board[x - 1][y].block)
					board[--x][y].open = true;
				else
					break;
			}
			break;
		case 3:
			for (int i = 0; i < how_many; ++i) {
				if (x == 29 && y == 29)
					break;
				else if (y == 29)
					break;
				else if (y < Size_y - 1 && !board[x][y + 1].block)
					board[x][++y].open = true;
				else
					break;
			}
			break;
		case 1:
			for (int i = 0; i < how_many; ++i) {
				if (x == 29 && y == 29)
					break;
				else if (x == 29)
					break;
				else if (x < Size_x - 1 && !board[x + 1][y].block)
					board[++x][y].open = true;
				else
					break;
			}
			break;
		}
		pre_dir = dir;
		if (x == 29 && y == 29)
			break;
	}
	board[29][29].open = true;
}
int main() {
	for (int i = 0; i < 30; ++i)
		board[uid(dre)][uid(dre)].block = true;

	board[0][0].block = false;
	board[29][29].block = false;

	board[player_x][player_y].open = true;
	int out = 1, count_5 = 0;
	static char order_0;
	creat_road();
	while (out) {
		cout << "\n\n";
		print_board();
		
		static char order;
		gotoxy(0, 0);
		cout << "input order: ";
		order = cin.get();
		switch (order) {
		case 'w':
			if (count_5 == 0)
				count_5++;
			else if (order_0 == order)
				count_5++;
			else
				count_5 = 1;
			if (count_5 < 5) {
				--player_y;
				if (player_y < 0 || !board[player_x][player_y].open)
					++player_y;
			}
			break;
		case 'a':
			if (count_5 == 0)
				count_5++;
			else if (order_0 == order)
				count_5++;
			else
				count_5 = 1;
			if (count_5 < 5) {
				--player_x;
				if (player_x < 0 || !board[player_x][player_y].open)
					++player_x;
			}
			break;
		case 's':
			if (count_5 == 0)
				count_5++;
			else if (order_0 == order)
				count_5++;
			else
				count_5 = 1;
			if (count_5 < 5) {
				++player_y;
				if (player_y > Size_y || !board[player_x][player_y].open)
					--player_y;
			}
			break;
		case 'd':
			if (count_5 == 0)
				count_5++;
			else if (order_0 == order)
				count_5++;
			else
				count_5 = 1;
			if (count_5 < 5) {
				++player_x;
				if (player_x > Size_x || !board[player_x][player_y].open)
					--player_x;
			}
			break;
		case 'r':
			switch_r = true;
			player_x = 0; player_y = 0;
			break;
		case '\n':
			system("cls");
			switch_r = false;
			for (int j = 0; j < 30; ++j) {
				for (int i = 0; i < 30; ++i) {
					board[j][i].block = false;
					board[j][i].open = false;
				}
			}
			for (int i = 0; i < 30; ++i)
				board[uid(dre)][uid(dre)].block = true;
			player_x = 0; player_y = 0;
			board[player_x][player_y].open = true;
			board[0][0].block = false;
			board[29][29].block = false;
			creat_road();
			break;
		case 'q':
			out = 0;
			break;
		}
		gotoxy(0, 0);
		cout << "input order:    ";
		if (!order_0)
			system("cls");
		cin.ignore();
		if (count_5 < 5)
			order_0 = order;
	}
}
