#include <iostream>
#include <string>
using namespace std;
string history[3000];
int cnt = 0;
int main() {
	string s;
	while (cin >> s) {
		history[cnt++] = s;
		if (s.find('E') != s.npos) {
			break;
		}
	}
	bool over = false;
	int self = 0, opp = 0;
	// 11分
	for (int i = 0; i < cnt && !over; i++) {
		for (auto c : history[i]) {
			if (c == 'W') {
				self++;
			}
			else if (c == 'L') {
				opp++;
			}
			else if (c == 'E') {
				over = true;
				break;
			}
			if ()
		}
	}
	return 0;
}
