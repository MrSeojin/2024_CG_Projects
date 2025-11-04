#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

typedef struct line_list {
	string line;
	int word_num;
	line_list* next;
	line_list* prev;
}line_list;

line_list* head = new line_list;
line_list* tail = new line_list;
line_list* now_node = new line_list;

int count_str(string &str, int i, int num);
string to_lower(string& str);

int main()
{
	head = nullptr;
	tail = nullptr;
	ifstream file_data;
	file_data.open("data.txt");
	if (!file_data.is_open()) {
		cout << "파일열기 실패";
	}
	else {
		string read_line;
		while (getline(file_data, read_line)) {
			line_list* node = new line_list;
			node->line = read_line;
			node->word_num = count_str(node->line, 0, 0);
			node->next = head;
			if (head)
				head->prev = node;
			if (!tail)
				tail = node;
			head = node;
		}
		file_data.close();
		if (head)
			head->prev = tail;
		if (tail)
			tail->next = head;
	}
	static BOOL switch_c, switch_e, switch_f, switch_g;
	static int switch_r;
	static string out, in;
	int quit = 1;
	while (quit) {
		static char command;

		cout << "input the command: ";
		rewind(stdin);
		cin >> command;

		switch (command) {
		case 'c':
			now_node = tail;
			if (switch_c) {
				switch_c = false;
				while (now_node) {
					cout << now_node->line << '\n';
					if (now_node->prev == tail)
						break;
					else
						now_node = now_node->prev;
				}
			}
			else {
				switch_c = true;
				while (now_node) {
					for (int i = 0; i < now_node->line.length(); ++i) {
						if (64 < now_node->line[i] && now_node->line[i] < 90)
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
						else if (now_node->line[i] == ' ')
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
						cout << now_node->line[i];
					}
					cout << '\n';
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					if (now_node->prev == tail)
						break;
					else
						now_node = now_node->prev;
				}
			}
			cout << '\n';
			break;
		case 'd':
			now_node = tail;
			while (now_node) {
				/*
				for (int i = now_node->line.length(); i >= 0; --i)
					cout << now_node->line[i];
				cout << '\n';
				*/
				reverse(now_node->line.begin(), now_node->line.end());
				cout << now_node->line << endl;
				if (now_node->prev == tail)
					break;
				else
					now_node = now_node->prev;
			}
			cout << '\n';
			break;
		case 'e':
			now_node = tail;
			if (switch_e) {
				switch_e = false;
				while (now_node) {
					cout << now_node->line << '\n';
					if (now_node->prev == tail)
						break;
					else
						now_node = now_node->prev;
				}
			}
			else {
				switch_e = true;
				while (now_node) {
					/*
					static string new_line;
					static int count;
					if (now_node->line.find('@')) {
						for (int i = 0; i < now_node->line.length(); ++i) {
							new_line[count++] + now_node->line[i];
							cout << now_node->line[i];
							if ((i + 1) % 3 == 0) {
								new_line[count++] + '@';
								new_line[count++] + '@';
							}
						}
						now_node->line = new_line;
					}
					else {
						for (int i = 0; i < now_node->line.length(); ++i) {
							if (now_node->line[i] == '@');
							else
								new_line[++count] + now_node->line[i];
							cout << now_node->line[i];
						}
						now_node->line = new_line;
					}
					*/
					if (now_node->line.find('@')) {
						for (int i = 0; i < now_node->line.length(); ++i) {
							cout << now_node->line[i];
							if (i == now_node->line.length() - 1);
							else if ((i + 1) % 3 == 0)
								cout << "@@";
						}
						cout << '\n';
					}
					if (now_node->prev == tail)
						break;
					else
						now_node = now_node->prev;
				}
			}
			cout << '\n';
			break;
		case 'f':
			now_node = tail;
			while (now_node) {
				for (int i = 0; i < now_node->line.length(); ++i) {
					if (now_node->line[i] == ' ') {
						for (int c = i; c >= 0; --c) {
							if (c == i);
							else if (now_node->line[c] == ' ') {
								reverse(now_node->line.begin() + (c + 1), now_node->line.end() - (now_node->line.length() - i));
								break;
							}
							else if (c == 0) {
								reverse(now_node->line.begin(), now_node->line.end() - (now_node->line.length() - i));
								break;
							}
							else;
						}
					}
					else if (i == now_node->line.length() - 1) {
						for (int c = i; c >= 0; --c) {
							if (c == i);
							else if (now_node->line[c] == ' ') {
								reverse(now_node->line.begin() + (c + 1), now_node->line.end() - (now_node->line.length() - i - 1));
								break;
							}
							else if (c == 0) {
								reverse(now_node->line.begin(), now_node->line.end() - (now_node->line.length() - i - 1));
								break;
							}
							else;
						}
					}
					else;
				}
				cout << now_node->line << endl;
				if (now_node->prev == tail)
					break;
				else
					now_node = now_node->prev;
			}
			cout << '\n';
			break;
		case 'g':
			now_node = tail;
			if (switch_g) {
				switch_g = false;
				while (now_node) {
					static size_t pos;
					while ((pos = now_node->line.find(in)) != string::npos)
						now_node->line.replace(now_node->line.find(in), 1, out);
					cout << now_node->line << '\n';
					if (now_node->prev == tail)
						break;
					else
						now_node = now_node->prev;
				}
			}
			else {
				switch_g = true;
				cout << "바꿀 문자 입력: ";
				cin >> out;
				cout << "넣을 문자 입력: ";
				cin >> in;
				while (now_node) {
					static size_t pos;
					while ((pos = now_node->line.find(out)) != string::npos)
						now_node->line.replace(now_node->line.find(out), 1, in);
					cout << now_node->line << '\n';
					if (now_node->prev == tail)
						break;
					else
						now_node = now_node->prev;
				}
			}
			cout << '\n';
			break;
		case 'h':
			now_node = tail;
			while (now_node) {
				cout << now_node->line << ' ' << now_node->word_num << endl;
				if (now_node->prev == tail)
					break;
				else
					now_node = now_node->prev;
			}
			cout << '\n';
			break;
		case 'r':
			// 단어의 개수에 따라 오름차순으로 정렬 ? 아 그 문장 을 배열 하는데 단어의 개수별로 오름 차순
			if (switch_r == 2) {
				switch_r = 0;
				file_data.open("data.txt");
				if (!file_data.is_open()) {
					cout << "파일열기 실패";
				}
				else {
					now_node = tail;
					string read_line;
					while (getline(file_data, read_line)) {
						now_node->line = read_line;
						now_node->word_num = count_str(now_node->line, 0, 0);
						if (now_node == head)
							break;
						else
							now_node = now_node->prev;
					}
					file_data.close();
				}
				now_node = tail;
				while (now_node) {
					cout << now_node->line << ' ' << now_node->word_num << endl;
					if (now_node == head)
						break;
					else
						now_node = now_node->prev;
				}
			}
			else if (switch_r == 0) {
				switch_r = 1;
				line_list* input_node = new line_list;
				input_node = tail;
				if (tail)
					now_node = tail->prev;
				while (now_node != head && input_node) {
					while (now_node->word_num >= input_node->next->word_num) {	// 자리찾기
						if (input_node == tail)
							break;
						if (now_node->word_num == input_node->word_num && now_node->line.length() < input_node->line.length())
							break;
						else
							input_node = input_node->next;
					}
//					cout << "now_node: " << now_node->line << ' ' << now_node->word_num << "\ninput_node: " << input_node->line << ' ' << input_node->word_num << '\n';
					if (now_node->word_num < input_node->word_num);			// 바꿀 필요 없을때
					else if (now_node != input_node) {
						line_list* BackUp = new line_list;
						BackUp = now_node->next;
						//now빠져나오기
						now_node->prev->next = now_node->next;
						now_node->next->prev = now_node->prev;
						// input의 next로
						now_node->next = input_node->next;
						now_node->prev = input_node;
						now_node->next->prev = now_node;
						input_node->next = now_node;
						if (head->prev != tail)
							tail = head->prev;
						now_node = BackUp;
					}
					input_node = now_node;
					now_node = input_node->prev;
				}
				if (head)
					input_node = head->next;
				if (input_node) {
					while (head->word_num >= input_node->next->word_num) {
						if (input_node == tail)
							break;
						if (head->word_num == input_node->word_num && head->line.length() < input_node->line.length())
							break;
						else
							input_node = input_node->next;
					}
					if (head->word_num < input_node->word_num);			// 바꿀 필요 없을때
					else if (head != input_node) {
						//now빠져나오기
						tail->next = head->next;
						head->next->prev = head->prev;
						// input의 next로
						head->next = input_node->next;
						head->prev = input_node;
						head->next->prev = head;
						input_node->next = head;
						head = tail->next;
					}
				}
				now_node = tail;
				while (now_node) {
					cout << now_node->line << ' ' << now_node->word_num << endl;
					if (now_node->prev == tail)
						break;
					else
						now_node = now_node->prev;
				}
			}
			else {
				switch_r = 2;
				now_node = head;
				while (now_node) {
					cout << now_node->line << ' ' << count_str(now_node->line, 0, 0) << endl;
					if (now_node->next == head)
						break;
					else
						now_node = now_node->next;
				}
			}
			cout << '\n';
			break;
		case 's':
			// 명령어와 단어를 입력하면 입력받은 문장들을 모두 출력, 그단어를 찾아 다른 색으로 출력 몇개 있는지 계산하여 출력 (대소문자 구분x)
			now_node = tail;
			if (1) {
				string word;
				cout << "단어 입력: ";
				cin >> word;
				while (now_node) {
					if (to_lower(now_node->line).find(to_lower(word)) != string::npos) {
						for (int i = 0; i < now_node->line.length(); ++i) {
							if (i == to_lower(now_node->line).find(to_lower(word)))
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
							else if (now_node->line[i] == ' ')
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
							cout << now_node->line[i];
						}
						cout << '\n';
					}
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

					if (now_node->prev == tail)
						break;
					else
						now_node = now_node->prev;
				}
			}
			cout << '\n';
			break;
		case 'q':
			quit = 0;
			break;
		}
	}
}
int count_str(string& str, int i, int num) {
	if (str.length() - 1 <= i)
		return ++num;
	else if (str[i] == ' ' && str[i + 1] != ' ')
		count_str(str, ++i, ++num);
	else
		count_str(str, ++i, num);
}
string to_lower(string& str) {
	string back;
	for (int i = 0; str[i] != '\0'; ++i) {
		back += tolower(str[i]);
	}
	return back;
}
