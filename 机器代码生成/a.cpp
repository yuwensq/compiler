#include <iostream>
using namespace std;
bool isLegalImm(int imm)
{
	unsigned int num = (unsigned int)imm;
	int bit;
	for (int i = 0; i < 16; i++)
	{
		num = ((num << 2) | (num >> 30));
		if (num <= 0xff)
		{
			return true;
		}
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