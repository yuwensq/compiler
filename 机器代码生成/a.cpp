#include <iostream>
using namespace std;
bool isLegalImm(int imm)
{
	unsigned int num = (unsigned int)imm;
	for (int i = 0; i < 16; i++)
	{
		if (num <= 0xff)
		{
			return true;
		}
		num = ((num << 2) | (num >> 30));
	}
	return false;
}
int main()
{
	int a;
	while (true)
	{
		cin >> a;
		cout << isLegalImm(a) << endl;
	}
	return 0;
}