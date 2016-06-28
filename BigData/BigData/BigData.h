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
		, s_data('\0')
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
			if (s_data[0] != bigdata.s_data[0])//²»Í¬ºÅ
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
	string Add(string left, string right);
	string Sub(string left, string right);
	void INT64ToStirng();
	bool IsLeftStrBig(char *pLeft, size_t LSize, char *pRight, size_t RSize);
private:
	INT64 _data;
	string s_data;
};