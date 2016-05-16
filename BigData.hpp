#pragma once
#include<iostream>
#include<cassert>
using namespace std;
#define UN_INIT   0xcccccccccccccccc
#define MAX_INT64 0x7fffffffffffffff
#define MIN_INT64 0x8000000000000000

typedef long long INT64;

class BigData
{
public:
	BigData(INT64 data = UN_INIT);
	BigData(const char* pData);

	BigData operator+(const BigData& bigdata);
	BigData operator-(const BigData& bigdata);
	BigData operator*(const BigData& bigdata);
	BigData operator/(const BigData& bigdata);
	BigData operator%(const BigData& bigdata);

	bool operator>(const BigData& bigdata);
	bool operator<(const BigData& bigdata);
	bool operator==(const BigData& bigdata);

	friend ostream& operator<<(ostream &os, const BigData& bigdata);
	friend istream& operator>>(istream &is, BigData& bigdata);
	bool IsINT64Overflow() const;
private:
	string Add(string left, string right);
	string Sub(string left, string right);
	string Mul(string left, string right);
	string Div(string left, string right);

	void INT64ToString();
	bool IsLeftStrBig(char *pLeft, size_t LSize, char *pRight, size_t RSize);
	char SubLoop(char *pLeft, size_t LSize, char *pRight, size_t RSize);
private:
	long long _value;
	string _strData;
};

BigData::BigData(INT64 data )
        :_value(data)
        , _strData("")
{
	INT64ToString();
}

BigData::BigData(const char* pStr)
{
	assert(pStr);

	char cSybom = pStr[0];
	char *pData = (char*)pStr;
	
	if ('+' == cSybom || '-' == cSybom)//"+12322" "-12345" "+b123aa123" "-b123aa123"
	{
		pData++;
	}
	else if (*pData >= '0' && *pData <= '9')//"123456"
	{
		cSybom = '+';
	}
	else                                    //"acb123"
	{
		_value = 0;
		_strData = "0";
		return;
	}
	//ȥ��ǰ��0
	while ('0' == *pData)  //"0000125"
	{
		pData++;
	}
	_strData.resize(strlen(pData)+1);
	_value = 0;
	_strData[0] = cSybom;
	int iCount = 1;
	//"123aaa55"
	while (pData)
	{
		if (*pData >= '0' && *pData <= '9')
		{
			_value = _value * 10 + *pData - '0';
			_strData[iCount] = *pData++;
		}
		else
		{
			break;
		}
	}
	_strData.resize(iCount);
	if ('-' == cSybom)
	{
		_value = 0 - _value;
	}
}

BigData BigData::operator+(const BigData& bigdata)
{
	//���Ҳ�������δ���
	if (!IsINT64Overflow() && !bigdata.IsINT64Overflow())
	{
		//���
		if (_strData[0] != bigdata._strData[0])
		{
			return BigData(_value+bigdata._value);
		}
		else
		{
			//ͬ�� ��ӽ��δ���
			if (('+' == _strData[0] && MAX_INT64 - _value >= bigdata._value) ||
				('-' == _strData[0] && MIN_INT64 - _value <= bigdata._value))
			{
				return BigData(_value + bigdata._value);
			}
	    }
	}
	// ������һ�����
	// ���������
	string strRet;
	if (_strData[0] == bigdata._strData[0])
	{
		strRet = Add(_strData,bigdata._strData);
	}
	else
	{
		strRet = Sub(_strData, bigdata._strData);
	}
	return BigData(strRet.c_str());
}

BigData BigData::operator-(const BigData& bigdata)
{
	//���Ҳ�������δ���
	if (!IsINT64Overflow() && !bigdata.IsINT64Overflow())
	{
		//ͬ��  
		if (_strData[0] == bigdata._strData[0])
		{
			return BigData(_value - bigdata._value);
		}
		else
		{
			//��� ������δ���    
			//-10 + 8 = -2 < -1        10 + -2 = 8 > 7
			if (('+' == _strData[0] && MAX_INT64 + bigdata._value >= _value) ||
				('-' == _strData[0] && MIN_INT64 + bigdata._value <= _value))
			{
				return BigData(_value - bigdata._value);
			}
		}
	}

	// 1��������һ�����������
	// 2������Ľ��һ�������
	string strRet;
	if (_strData[0] != bigdata._strData[0])
	{
		strRet = Add(_strData,bigdata._strData);
	}
	else
	{
		strRet = Sub(_strData, bigdata._strData);
	}
	return BigData(strRet.c_str());
}

BigData BigData::operator*(const BigData& bigdata)
{ 
	//���Ҳ���������0 
	if (0 == _value || 0 == bigdata._value)
	{
		return BigData(INT64(0));
	}
	if (!IsINT64Overflow() && !bigdata.IsINT64Overflow())
	{
		if (_strData[0] == bigdata._strData[0])
		{
			//10/2=5 >=1 2 3 4 5    10/-2=-5 <=-5 -4 -3 -2 -1
			if (('+' == _strData[0] && MAX_INT64 / _value >= bigdata._value) ||
				('-' == _strData[0] && MIN_INT64 / _value <= bigdata._value))
			{
				return BigData(_value*bigdata._value);
			}
		}
		else
		{
			if (('+' == _strData[0] && MIN_INT64 / _value <= bigdata._value) ||
				('-' == _strData[0] && MAX_INT64 / _value >= bigdata._value))
			{
				return BigData(_value*bigdata._value);
			}
		}
	}
	return BigData(Mul(_strData,bigdata._strData).c_str());
}

BigData BigData::operator/(const BigData& bigdata)
{
	assert(bigdata._value != 0);
	if (0 == _value)
	{
		return BigData(INT64(0));
	}
	if (!IsINT64Overflow() && !bigdata.IsINT64Overflow())
	{
		return BigData(_value/bigdata._value);
	}
	return BigData(Div(_strData,bigdata._strData).c_str());
}

string BigData::Add(string left, string right)
{
	int LSize = left.size();
	int RSize = right.size();
	if (LSize < RSize)//���������λ�������Ҳ�����λ��
	{
		swap(left,right);
		swap(LSize,RSize);
	}

	string strRet;
	strRet.resize(LSize + 1);
	strRet[0] = left[0];
	char step = 0;

	for (int index = 1; index < LSize; ++index)
	{
		char ret = left[LSize - index] - '0' + step;
		if (index < RSize)
		{
			ret += (right[RSize - index] - '0');
		}
		strRet[LSize - index + 1] = (ret % 10 )+ '0';
		step = ret / 10;
	}
	strRet[1] = step + '0';
	return strRet;
}

string BigData::Sub(string left, string right)
{
	// 1��������� > �Ҳ�����
	// 2��ȷ������λ
	int LSize = left.size();
	int RSize = right.size();
	char cSymbol = left[0];
	if (LSize < RSize || (LSize == RSize&&left < right))
	{
		swap(left, right);
		swap(LSize ,RSize);
		if ('+' == cSymbol)
		{
			cSymbol = '-';
		}
		else
		{
			cSymbol = '+';
		}
	}

	string strRet;
	strRet.resize(LSize);
	strRet[0] = cSymbol;
	// ��λ���
	// 1��ȡ�������ÿһλ���Ӻ���ǰȡ
	// 2�����Ҳ�����û�г���  ȡ�Ҳ�����ÿһλ �Ӻ���ǰȡ
	// 3��ֱ�����  <0,��λ �������ǰһλ-1����ǰλ+10
	// 4�� ������
	for (int index = 1; index < LSize; ++index)
	{
		char ret = left[LSize - index] - '0' ;
		if (index < RSize)
		{
			ret -= (right[RSize - index] - '0');
		}
		if (ret < 0)
		{
			left[LSize - index - 1] -= 1;
			ret += 10;
		}
		strRet[LSize - index] = ret + '0';
	}
	
	return strRet;
}

string BigData::Mul(string left, string right)
{
	int LSize = left.size();
	int RSize = right.size();

	if (LSize > RSize)
	{
		swap(left, right);
		swap(LSize, RSize);
	}
	char cSymbol = '+';
	if (left[0] != right[0])
	{
		cSymbol = '-';
	}

	string strRet;
	strRet.assign(LSize+RSize-1,'0');
	strRet[0] = cSymbol;
	int DataLen = strRet.size();
	int OffSet = 0;//��λ��־

	for (int index = 1; index < LSize; ++index)
	{
		char Left = left[LSize - index] - '0';//ȡ�������ÿһλ���Ӻ���ǰȡ
		char step = 0;
		if (0 == Left) //ȡ��0ֱ�Ӵ�λ
		{
			OffSet++;
			continue;
		}
		for (int RIndex = 1; RIndex < RSize; ++RIndex)
		{
			char ret = Left*(right[RSize - RIndex] - '0');//ȡrightÿһλ (�Ӻ���ǰȡ)��left���
			ret += step; 
			ret += (strRet[DataLen-OffSet-RIndex]-'0');
			strRet[DataLen - OffSet - RIndex] = ret % 10 + '0';
			step = ret / 10;
		}
		strRet[DataLen - OffSet - RSize] += step;
		OffSet++;
	}
	return strRet;
}

string BigData::Div(string left, string right)
{
	int LSize = left.size();
	int RSize = right.size();
	char cSymbol = '+';

	if (LSize < RSize || (LSize == RSize&&strcmp(left.c_str() + 1, right.c_str() + 1) < 0))
	{
		return "0";
	}
	if (left[0] != right[0])
	{
		cSymbol = '-';
	}
	if ("+1" == right || "-1" == right)
	{
		left[0] = cSymbol;
		return left;
	}

	string strRet;
	strRet.append(1, cSymbol);
	char *pLeft = (char*)(left.c_str() + 1);
	char *pRight = (char*)(right.c_str() + 1);
	int DataLen = 1;
	LSize -= 1;
	// "2222222222" / 33
	for (int index = 0; index < LSize-1;)
	{
		//���������е�0  9900000000099/33   
		if ('0' == *pLeft)
		{
			strRet.append(1, '0');
			pLeft++;
			index++;
			
			continue;
		}

		if (!IsLeftStrBig(pLeft, DataLen, pRight, RSize - 1))
		{
			DataLen++;
			if (index + DataLen > LSize)
			{
				break;
			}
			strRet.append(1, '0');
			continue;
		}
		else
		{
			//ѭ�����
			strRet.append(1, SubLoop(pLeft, DataLen, pRight, RSize - 1));
			//ɾ������Ϊ���м��������ֵ�0
			while ('0' == *pLeft && DataLen > 0)
			{
				pLeft++;
				index++;
				DataLen--;
			}
			DataLen++;
			if (index + DataLen > LSize)
			{
				break;
			}
		}
	}
	return strRet;
}

bool BigData::IsLeftStrBig(char *pLeft, size_t LSize, char *pRight, size_t RSize)
{
	assert(pLeft != NULL || pRight != NULL);
	if (LSize > RSize || (LSize == RSize&&strncmp(pLeft, pRight, LSize) >= 0))
	{
		return true;
	}
	return false;
}
//ѭ�����
char BigData::SubLoop(char *pLeft, size_t LSize, char *pRight, size_t RSize)
{
	assert(pLeft != NULL || pRight != NULL);
	char ret = '0';
	while (true)
	{
		if (!IsLeftStrBig(pLeft, LSize, pRight, RSize))
		{
			break;
		}
		// ��-=
		int LDataLen = LSize - 1;
		int RDataLen = RSize - 1;
		while (RDataLen >= 0 && LDataLen >= 0)
		{
			if (pLeft[LDataLen] < pRight[RDataLen])
			{
				pLeft[LDataLen - 1] -= 1;//��ǰһλ��λ
				pLeft[LDataLen] += 10;//��ǰλ+10
			}

			pLeft[LDataLen] = pLeft[LDataLen] - pRight[RDataLen] + '0';
			LDataLen--;
			RDataLen--;
		}

		//�������е�0   "990000000000000000000000000099"
		while ('0' == *pLeft && LSize > 0)
		{
			pLeft++;
			LSize--;
		}
		ret++;//ѭ�����һ��ret+1��ret������
	}
	return ret;
}

void BigData::INT64ToString()
{
	//12345
	char cSymbol = '+';
	INT64 temp = _value;
	if (temp < 0)
	{
		cSymbol = '-';
		temp = 0 - temp;
	}
	_strData.append(1, cSymbol);
	int Count = 1;
	//54321
	while (temp)
	{
		_strData.append(1, temp % 10 + '0');
		temp /= 10;
	}

	char *pLeft = (char*)(_strData.c_str() + 1);
	char *pRight = (char*)(_strData.c_str() + _strData.size() - 1);

	while (pLeft < pRight)
	{
		char ctemp = *pLeft;
		*pLeft ++= *pRight;
		*pRight-- = ctemp;
	}
}

bool BigData::IsINT64Overflow()const
{
	string strTemp;
	if ('+' == _strData[0])
	{
		strTemp = "+9223372036854775807";
	}
	else
	{
		strTemp = "-9223372036854775808";
	}
	if (_strData.size() > strTemp.size())
	{
		return true;
	}
	else if (_strData.size() == strTemp.size() && _strData > strTemp)
	{
		return true;
	}
	return false;
}

ostream& operator<<(ostream& os, const BigData& bigdata)
{
	if (!bigdata.IsINT64Overflow()) // û�����
	{
		os << bigdata._value;
	}
	else
	{
		char *strRet = (char*)bigdata._strData.c_str();
		if ('+' == strRet[0])
		{
			strRet++;
		}
		os << strRet;
	}
	return os;
}

void Test0()
{
	BigData left1("22222222");
	BigData right1("33");

	cout << left1 + right1 << endl;


	BigData left2("-11111222");
	BigData right2(33);

	cout << left2 + right2 << endl;

}
void Test1()
{
	BigData left1("111111");
	BigData right1("9999999999999");

	cout << left1 - right1 << endl;


	BigData left2("22222222");
	BigData right2(-33);

	cout << left2 - right2 << endl;

	BigData left3(-3333333333333);
	BigData right3(-33);

	cout << left3 - right3 << endl;
}

void Test2()
{
	BigData left1("22000000222");
	BigData right1(1111);

	cout << left1 * right1 << endl;


	BigData left2("22222222");
	BigData right2("110000011");

	cout << left2 * right2 << endl;
}

void Test3()
{
	BigData left1("9990000099");
	BigData right1(33);

	cout << left1 / right1 << endl;


	BigData left2("22222222");
	BigData right2(33);

	cout << left2 / right2 << endl;
}

