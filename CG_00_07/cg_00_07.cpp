#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;
typedef struct square {
	int start_x, start_y, end_x, end_y;
}sq;
sq sq1, sq2;
int Size_x = 30, Size_y = 30;

void print_board() {
	cout << "   ";
	for (int i = 0; i < Size_x; ++i) {
		if (i < 10)
			cout << ' ' << i;
		else
			cout << i;
	}
	cout << "\n\n";
	for (int j = 0; j < Size_y; ++j) {
		if (j < 10)
			cout << j << "  ";
		else
			cout << j << ' ';
		for (int i = 0; i < Size_x; ++i) {
			if (sq1.start_y <= sq1.end_y && sq1.start_y <= j && j <= sq1.end_y) {
				if (sq1.start_x <= sq1.end_x && sq1.start_x <= i && i <= sq1.end_x) {
					if (sq2.start_y <= sq2.end_y && sq2.start_y <= j && j <= sq2.end_y) {
						if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
							cout << " #";
						else if (sq2.start_x > sq2.end_x && (sq2.end_x >= i || i >= sq2.start_x))
							cout << " #";
						else
							cout << " 0";
					}
					else if (sq2.start_y > sq2.end_y && (sq2.end_y >= j || j >= sq2.start_y)) {
						if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
							cout << " #";
						else if (sq2.start_x > sq2.end_x && (sq2.end_x >= i || i >= sq2.start_x))
							cout << " #";
						else
							cout << " 0";
					}
					else
						cout << " 0";
				}
				else if (sq1.start_x > sq1.end_x && (sq1.end_x >= i || i >= sq1.start_x)) {
					if (sq2.start_y <= sq2.end_y && sq2.start_y <= j && j <= sq2.end_y) {
						if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
							cout << " #";
						else if (sq2.start_x > sq2.end_x && (sq2.end_x >= i || i >= sq2.start_x))
							cout << " #";
						else
							cout << " 0";
					}
					else if (sq2.start_y > sq2.end_y && (sq2.end_y >= j || j >= sq2.start_y)) {
						if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
							cout << " #";
						else if (sq2.start_x > sq2.end_x && (sq2.end_x >= i || i >= sq2.start_x))
							cout << " #";
						else
							cout << " 0";
					}
					else
						cout << " 0";
				}
				else if (sq2.start_y <= sq2.end_y && sq2.start_y <= j && j <= sq2.end_y) {
					if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
						cout << " x";
					else if (sq2.start_x > sq2.end_x && (sq2.end_x <= i || i <= sq2.start_x))
						cout << " x";
					else
						cout << " *";
				}
				else if (sq2.start_y > sq2.end_y && (sq2.end_y <= j || j <= sq2.start_y)) {
					if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
						cout << " x";
					else if (sq2.start_x > sq2.end_x && (sq2.end_x <= i || i <= sq2.start_x))
						cout << " x";
					else
						cout << " *";
				}
				else
					cout << " *";
			}
			else if (sq1.start_y > sq1.end_y && (sq1.end_y >= j || j >= sq1.start_y)) {
				if (sq1.start_x <= sq1.end_x && sq1.start_x <= i && i <= sq1.end_x) {
					if (sq2.start_y <= sq2.end_y && sq2.start_y <= j && j <= sq2.end_y) {
						if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
							cout << " #";
						else if (sq2.start_x > sq2.end_x && (sq2.end_x >= i || i >= sq2.start_x))
							cout << " #";
						else
							cout << " 0";
					}
					else if (sq2.start_y > sq2.end_y && (sq2.end_y >= j || j >= sq2.start_y)) {
						if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
							cout << " #";
						else if (sq2.start_x > sq2.end_x && (sq2.end_x >= i || i >= sq2.start_x))
							cout << " #";
						else
							cout << " 0";
					}
					else
						cout << " 0";
				}
				else if (sq1.start_x > sq1.end_x && (sq1.end_x >= i || i >= sq1.start_x)) {
					if (sq2.start_y <= sq2.end_y && sq2.start_y <= j && j <= sq2.end_y) {
						if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
							cout << " #";
						else if (sq2.start_x > sq2.end_x && (sq2.end_x >= i || i >= sq2.start_x))
							cout << " #";
						else
							cout << " 0";
					}
					else if (sq2.start_y > sq2.end_y && (sq2.end_y >= j || j >= sq2.start_y)) {
						if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
							cout << " #";
						else if (sq2.start_x > sq2.end_x && (sq2.end_x >= i || i >= sq2.start_x))
							cout << " #";
						else
							cout << " 0";
					}
					else
						cout << " 0";
				}
				else if (sq2.start_y <= sq2.end_y && sq2.start_y <= j && j <= sq2.end_y) {
					if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
						cout << " x";
					else if (sq2.start_x > sq2.end_x && (sq2.end_x <= i || i <= sq2.start_x))
						cout << " x";
					else
						cout << " *";
				}
				else if (sq2.start_y > sq2.end_y && (sq2.end_y <= j || j <= sq2.start_y)) {
					if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
						cout << " x";
					else if (sq2.start_x > sq2.end_x && (sq2.end_x <= i || i <= sq2.start_x))
						cout << " x";
					else
						cout << " *";
				}
				else
					cout << " *";
			}
			else if (sq2.start_y <= sq2.end_y && sq2.start_y <= j && j <= sq2.end_y) {
				if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
					cout << " x";
				else if (sq2.start_x > sq2.end_x && (sq2.end_x <= i || i <= sq2.start_x))
					cout << " x";
				else
					cout << " *";
			}
			else if (sq2.start_y > sq2.end_y && (sq2.end_y <= j || j <= sq2.start_y)) {
				if (sq2.start_x <= sq2.end_x && sq2.start_x <= i && i <= sq2.end_x)
					cout << " x";
				else if (sq2.start_x > sq2.end_x && (sq2.end_x <= i || i <= sq2.start_x))
					cout << " x";
				else
					cout << " *";
			}
			else
				cout << " *";
		}
		cout << '\n';
	}
}
void gotoxy(int x, int y) {
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int main() {
	gotoxy(0, 36);
	cout << "input coord value1(0~29): ";
	cin >> sq1.start_x >> sq1.start_y >> sq1.end_x >> sq1.end_y;
	cout << "input coord value2(0~29): ";
	cin >> sq2.start_x >> sq2.start_y >> sq2.end_x >> sq2.end_y;
	system("cls");
	cout << "\n\n";
	print_board();

	int out = 1;
	while (out) {
		static char order;
		gotoxy(0, 0);
		cout << "input order: ";
		cin >> order;
		switch (order) {
		case 'x':
			sq1.start_x++;
			sq1.end_x++;
			if (sq1.end_x >= Size_x)
				sq1.end_x -= Size_x;
			if (sq1.start_x >= Size_x)
				sq1.start_x -= Size_x;
			break;
		case 'X':
			sq1.start_x--;
			sq1.end_x--;
			if (sq1.end_x < 0)
				sq1.end_x += Size_x;
			if (sq1.start_x < 0)
				sq1.start_x += Size_x;
			break;
		case 'y':
			sq1.start_y++;
			sq1.end_y++;
			if (sq1.end_y >= Size_y)
				sq1.end_y -= Size_y;
			if (sq1.start_y >= Size_y)
				sq1.start_y -= Size_y;
			break;
		case 'Y':
			sq1.start_y--;
			sq1.end_y--;
			if (sq1.end_y < 0)
				sq1.end_y += Size_y;
			if (sq1.start_y < 0)
				sq1.start_y += Size_y;
			break;
		case 's':		
			if (sq1.start_x != sq1.end_x)
				sq1.end_x--;
			if (sq1.end_x < 0)
				sq1.end_x += Size_x;
			if (sq1.start_y != sq1.end_y)
				sq1.end_y--;
			if (sq1.end_y < 0)
				sq1.end_y += Size_y;
			break;
		case 'S':		
			sq1.end_x++;
			if (sq1.start_x == sq1.end_x)
				sq1.end_x--;
			else if (sq1.end_x >= Size_x)
				sq1.end_x -= Size_x;
			sq1.end_y++;
			if (sq1.start_y == sq1.end_y)
				sq1.end_y--;
			else if (sq1.end_y >= Size_y)
				sq1.end_y -= Size_y;
			break;
		case 'w':
			sq2.start_x++;
			sq2.end_x++;
			if (sq2.end_x >= Size_x)
				sq2.end_x -= Size_x;
			if (sq2.start_x >= Size_x)
				sq2.start_x -= Size_x;
			break;
		case 'W':
			sq2.start_x--;
			sq2.end_x--;
			if (sq2.end_x < 0)
				sq2.end_x += Size_x;
			if (sq2.start_x < 0)
				sq2.start_x += Size_x;
			break;
		case 'a':
			sq2.start_y++;
			sq2.end_y++;
			if (sq2.end_y >= Size_y)
				sq2.end_y -= Size_y;
			if (sq2.start_y >= Size_y)
				sq2.start_y -= Size_y;
			break;
		case 'A':
			sq2.start_y--;
			sq2.end_y--;
			if (sq2.end_y < 0)
				sq2.end_y += Size_y;
			if (sq2.start_y < 0)
				sq2.start_y += Size_y;
			break;
		case 'd':
			if (sq2.start_x != sq2.end_x)
				sq2.end_x--;
			if (sq2.end_x < 0)
				sq2.end_x += Size_x;
			if (sq2.start_y != sq2.end_y)
				sq2.end_y--;
			if (sq2.end_y < 0)
				sq2.end_y += Size_y;
			break;
		case 'D':
			sq2.end_x++;
			if (sq2.start_x == sq2.end_x)
				sq2.end_x--;
			else if (sq2.end_x >= Size_x)
				sq2.end_x -= Size_x;
			sq2.end_y++;
			if (sq2.start_y == sq2.end_y)
				sq2.end_y--;
			else if (sq2.end_y >= Size_y)
				sq2.end_y -= Size_y;
			break;
		case 'r':
			system("cls");
			cout << "input coord value1(0~29): ";
			cin >> sq1.start_x >> sq1.start_y >> sq1.end_x >> sq1.end_y;
			cout << "input coord value2(0~29): ";
			cin >> sq2.start_x >> sq2.start_y >> sq2.end_x >> sq2.end_y;
			system("cls");
			cout << '\n';
			break;
		case 'q':
			out = 0;
			break;
		}
		cout << '\n';
		print_board();
		gotoxy(0, 0);
		cout << "input order:   ";
	}
}
