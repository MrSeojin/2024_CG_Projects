#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <list>
using namespace std;

struct data {
	int x, y, z;
};

int line(list<struct data>::iterator k) {
	return k->x * k->x + k->y * k->y + k->z * k->z;
}

int main() {
	list<struct data> phone;
	list<struct data> Max_;
	list<struct data> Min_;
	list<struct data>::iterator iter;

	int start = 0;

	int out = 1;
	while (out) {
		static char command;
		cout << "command: ";
		cin >> command;
		switch (command) {
		case '+':
			if (phone.size() < 20) {
				static struct data new_data;
				cout << "x: ";
				cin >> new_data.x;
				cout << "y: ";
				cin >> new_data.y;
				cout << "z: ";
				cin >> new_data.z;
				if (start + phone.size()==19) {
					--start;
					phone.push_front(new_data);
				}
				else
					phone.push_back(new_data);
			}
			else
				cout << "공간이 없습니다(20/20)\n";
			if (1) {
				int i = start;
				for (iter = phone.begin(); iter != phone.end(); iter++)
					cout << i++ << " - x: " << iter->x << ", y: " << iter->y << ", z: " << iter->z << '\n';
			}
			cout << '\n';
			break;
		case '-':
			if (phone.empty())
				cout << "리스트에 데이터가 없습니다.\n";
			else {
				phone.pop_back();
				if (!phone.empty()) {
					int i = start;
					for (iter = phone.begin(); iter != phone.end(); iter++)
						cout << i++ << " - x: " << iter->x << ", y: " << iter->y << ", z: " << iter->z << '\n';
				}
				else
					cout << "리스트가 비었습니다.\n";
			}
			cout << '\n';
			break;
		case 'e':
			if (phone.size() < 20) {
				static struct data new_data;
				cout << "x: ";
				cin >> new_data.x;
				cout << "y: ";
				cin >> new_data.y;
				cout << "z: ";
				cin >> new_data.z;
				if (start != 0)
					--start;
				phone.push_front(new_data);
			}
			else
				cout << "공간이 없습니다(20/20)\n";
			if (1) {
				int i = start;
				for (iter = phone.begin(); iter != phone.end(); iter++)
					cout << i++ << " - x: " << iter->x << ", y: " << iter->y << ", z: " << iter->z << '\n';
			}
			cout << '\n';
			break;
		case 'd':
			if (phone.empty())
				cout << "리스트에 데이터가 없습니다.\n";
			else {
				phone.pop_front();
				if (!phone.empty()) {
					int i = start;
					for (iter = phone.begin(); iter != phone.end(); iter++)
						cout <<i++ << " - x: " << iter->x << ", y: " << iter->y << ", z: " << iter->z << '\n';
				}
				else
					cout << "리스트가 비었습니다.\n";
			}
			cout << '\n';
			break;
		case 'l':
			cout << "점의 개수: " << phone.size() << '\n';
			break;
		case 'c':
			while (!Min_.empty())
				Min_.pop_front();
			while (!Max_.empty())
				Max_.pop_front();
			while (!phone.empty())
				phone.pop_front();
			start = 0;
			cout << "모두 삭제 완료\n";
			break;
		case 'm':
			if (!phone.empty()) {
				list<struct data>::iterator max_iter;
				max_iter = phone.begin();
				int i = start;
				for (iter = phone.begin(); iter != phone.end(); iter++) {
					if (line(iter) > line(max_iter)) {
						max_iter = iter;
						i++;
					}
				}
				cout << "result: \n";
				cout << i <<" - x: " << max_iter->x << ", y: " << max_iter->y << ", z: " << max_iter->z;
			}
			else
				cout << "데이터가 없습니다.";
			cout << '\n';
			break;
		case 'n':
			if (!phone.empty()) {
				list<struct data>::iterator min_iter;
				min_iter = phone.begin();
				int i = start;
				for (iter = phone.begin(); iter != phone.end(); iter++) {
					if (line(iter) < line(min_iter)) {
						min_iter = iter;
					}
					i++;
				}
				cout << "result: \n";
				cout << i << " - x: " << min_iter->x << ", y: " << min_iter->y << ", z: " << min_iter->z;
			}
			else
				cout << "데이터가 없습니다.";
			cout << '\n';
			break;
		case 'a':
			if (!phone.empty()) {
				if (Max_.empty()) {
					//정렬 넣자
					for (iter = phone.begin(); iter != phone.end(); iter++) {
						static struct data min_data;
						min_data.x = iter->x;
						min_data.y = iter->y;
						min_data.z = iter->z;
						if (Max_.empty())
							Max_.push_front(min_data);
						else {
							list<struct data>::iterator min_iter;
							for (min_iter = Max_.begin(); min_iter != Max_.end(); min_iter++) {
								if (line(iter) < line(min_iter)) {
									Max_.insert(min_iter, min_data);
									break;
								}
							}
							if (min_iter == Max_.end())
								Max_.push_back(min_data);
						}
					}
					cout << "result: \n";
					int i = start;
					for (iter = Max_.begin(); iter != Max_.end(); iter++)
						cout << i++ << " - x: " << iter->x << ", y: " << iter->y << ", z: " << iter->z << '\n';
				}
				else {
					while (!Max_.empty())
						Max_.pop_front();
					cout << "result: \n";
					int i = start;
					for (iter = phone.begin(); iter != phone.end(); iter++)
						cout << i++ << " - x: " << iter->x << ", y: " << iter->y << ", z: " << iter->z << '\n';
				}
			}
			else
				cout << "데이터가 없습니다.";
			cout << '\n';
			break;
		case 's':
			if (!phone.empty()) {
				if (Min_.empty()) {
					//정렬 넣자
					for (iter = phone.begin(); iter != phone.end(); iter++) {
						static struct data min_data;
						min_data.x = iter->x;
						min_data.y = iter->y;
						min_data.z = iter->z;
						if (Min_.empty())
							Min_.push_front(min_data);
						else {
							list<struct data>::iterator min_iter;
							for (min_iter = Min_.begin(); min_iter != Min_.end(); min_iter++) {
								if (line(iter) > line(min_iter)) {
									Min_.insert(min_iter, min_data);
									break;
								}
							}
							if (min_iter == Min_.end())
								Min_.push_back(min_data);
						}
					}
					cout << "result: \n";
					int i = start;
					for (iter = Min_.begin(); iter != Min_.end(); iter++)
						cout << i++ << " - x: " << iter->x << ", y: " << iter->y << ", z: " << iter->z << '\n';
				}
				else {
					while (!Min_.empty())
						Min_.pop_front();
					cout << "result: \n";
					int i = start;
					for (iter = phone.begin(); iter != phone.end(); iter++)
						cout << i++ << " - x: " << iter->x << ", y: " << iter->y << ", z: " << iter->z << '\n';
				}
			}
			else
				cout << "데이터가 없습니다.";
			cout << '\n';
			break;
		case 'q':
			out = 0;
			break;
		}
	}
}
//eof