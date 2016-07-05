#pragma once

#include<assert.h>
#include<string>
#include<iostream>
using namespace std;

#define MAX_INT64 0x7fffffffffffffff
#define MIN_INT64 0x8000000000000000

typedef long long INT64;

class BigData
{
public:
	BigData()
		:_data(0)
		, s_data("0")
	{}
	BigData(INT64 data)
		:_data(data)
		, s_data("")
	{
		INT64ToStirng();
	}
	BigData(const char* pData)
	{
		assert(pData);
		char symbol;
		if (*pData == '+' || *pData == '-')
		{
			symbol = *pData;
			pData++;
		}
		else if (*pData <= '9' && *pData >= '0')
		{
			symbol = '+';
		}
		else
		{
			_data = 0;
			s_data = "0";
			return;
		}
		while (*pData == '0')
		{
			pData++;
		}
		_data = 0;
		s_data.resize(strlen(pData) + 2);
		int index = 0;
		s_data[index++] = symbol;
		while (*pData != '\0')
		{
			if (*pData <= '9'&&*pData >= '0')
			{
				_data = _data * 10 + *pData - '0';
				s_data[index++] = *pData;
				pData++;
			}
			else
			{
				break;
			}
			
		}
		s_data[index] = '\0';
		if (symbol == '-')
		{
			_data = 0 - _data;
		}
	}

	BigData operator+(const BigData& bigdata)
	{
		if (!IsINT64Owerflow() && !bigdata.IsINT64Owerflow())
		{
			if (s_data[0] != bigdata.s_data[0])//不同号
			{
				return BigData(_data + bigdata._data);
			}
			else
			{
				if (('+' == s_data[0] && MAX_INT64 - _data >= bigdata._data)
					|| ('-' == s_data[0] && INT_MIN + _data <= bigdata._data))
				{
					return BigData(_data + bigdata._data);
				}
			}
		}
		string strRet;
		if (s_data[0] == bigdata.s_data[0])
		{
			strRet = Add(s_data, bigdata.s_data);
		}
		else
		{
			strRet = Sub(s_data, bigdata.s_data);
		}
		return BigData(strRet.c_str());
	}
	BigData& operator-(const BigData& bigdata);
	BigData& operator*(const BigData& bigdata);
	BigData& operator/(const BigData& bigdata);

	bool IsINT64Owerflow()const
	{
		string strTmp;
		if ('+' == s_data[0])
		{
			strTmp = "+9223372036854775807";
		}
		else
		{
			strTmp = "-9223372036854775808";
		}
		if (s_data.size() > strTmp.size())
		{
			return true;
		}
		else if (s_data.size() == strTmp.size() && s_data > strTmp)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
private:
	string Add(string left, string right)
	{
		int L_Size = left.size();
		int R_Size = right.size();
		if (L_Size < R_Size)//保证大的数在left的位置
		{
			swap(L_Size, R_Size);
			swap(left, right);
		}
		string strRet;
		char Step;

		strRet.resize(L_Size + 1);//保证可以保存相加之后的字符串
		strRet[0] = left[0];
		Step = 0;                 //进位
		for (int index = 1; index < L_Size; ++index)//第一位符号位保留，左对齐相加
		{
			char Ret = left[L_Size - index] - '0' + Step;

			if (index < R_Size)
			{
				Ret += (right[R_Size - index] - '0');
			}
			strRet[L_Size - index + 1] = (Ret % 10 + '0');
			Step = Ret / 10;
		}
		strRet[1] = (Step + '0');

		return strRet;
	}
	string Sub(string left, string right)
	{
		int L_Size = left.size();
		int R_Size = right.size();
		if (L_Size < R_Size || (L_Size == R_Size && left < right))
		{
			swap(L_Size, R_Size);
			std::swap(left, right);
		}
		string strRet;

		strRet.resize(L_Size);
		strRet[0] = left[0];
		for (int index = 1; index < L_Size; index++)
		{
			char cRet = left[L_Size - index] - '0';
			if (index < R_Size)
			{
				cRet += (right[R_Size - index] - '0');
			}
			if (cRet < 0)
			{
				left[L_Size - index - 1] -= 1;
				cRet += 10;
			}
			strRet[L_Size - index] = (cRet + '0');
		}
		return strRet;
	}
	void INT64ToStirng()
	{
		char symbol = '+';
		INT64 tmp = _data;
		if (tmp < 0)
		{
			symbol = '-';
			tmp = 0 - tmp;
		}
		s_data.append(1, symbol);
		while (tmp >0)
		{
			s_data.append(1, tmp % 10 + '0');
			tmp = tmp / 10;
		}
		char* left = (char*)(s_data.c_str() + 1);
		char* right = (char*)(s_data.c_str() + s_data.size() - 1);
		while (left < right)
		{
			char ctmp = *left;
			*left++ = *right;
			*right-- = ctmp;
		}

	}
	bool IsLeftStrBig(char *pLeft, size_t LSize, char *pRight, size_t RSize);
private:
	INT64 _data;
	string s_data;
};