#include<iostream>
#include<string>
#include<vector>

using std::cout;
using std::endl;
using std::cin;
using std::vector;
using std::string;
int main()
{
	vector<string> grade = { "F", "D", "C", "B", "A", "A++" };
	int score = 0;
	while (cin >> score) {
		if (score < 60) {
			cout << grade[0] << endl;
		}
		else {
			cout << grade[(score - 50) / 10];
		}

	}

	return 0;
}