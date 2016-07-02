#pragma once
#include <cassert>
#include <string>
#include <iostream>

#define UN_INIT 0xcccccccccccccccc
#define MAX_INT64 0x7fffffffffffffff
#define MIN_INT64 0x8000000000000000

typedef long long INT64;

class BigData
{
public:
	BigData(INT64 data)
		: m_llValue(data)
		, m_strData("")
	{
		INT64ToString();
	}
	BigData(const char *_pData)
	{
		//1234567 00234567 asdfg12345 123456asdfgh -1234567
		assert(NULL != _pData);

		char symbol = _pData[0];
		char* pData = (char*)_pData;
		if ('+' == symbol || '-' == symbol)
		{
			pData++;
		}
		else if (*pData >= '0' && *pData <= '9')
		{
			symbol = '+';
		}
		else
		{
			m_llValue = 0;
			m_strData = "0";
			return;
		}

		// 去掉前置0
		while ('0' == *pData)
			pData++;

		// "12457aaa123"
		m_strData.resize(strlen(pData) + 1);
		m_llValue = 0;
		m_strData[0] = symbol;
		int count = 1;
		while (pData)
		{
			if (*pData >= '0' && *pData <= '9')
			{
				m_llValue = m_llValue * 10 + *pData - '0';
				m_strData[count++] = *pData++;
			}
			else
			{
				break;
			}
		}

		m_strData.resize(count);

		if ('-' == symbol)
		{
			m_llValue = 0 - m_llValue;
		}
	}

	BigData operator+(BigData& bigData);
	BigData operator-(const BigData& bigData);
	BigData operator*(const BigData& bigData);
	BigData operator/(const BigData& bigData);
	BigData operator%(const BigData& bigData);

	//=======================================
	bool operator<(const BigData& bigData);
	bool operator>(const BigData& bigData);
	bool operator==(const BigData& bigData);
	
	std::ostream& operator<<(std::ostream& _cout);

	friend std::ostream& operator<<(std::ostream& _cout, const BigData& bigData);
	friend std::istream& operator>>(std::istream& _cin, BigData bigData);
	
private:
	std::string Add(std::string left, std::string right)
	{
		int LSize = left.size();
		int RSize = right.size();

		if (LSize < RSize)
		{
			std::swap(left, right);
			std::swap(LSize, RSize);
		}

		std::string strRet;
		strRet.resize(LSize + 1);
		strRet[0] = left[0];
		char step = 0;
 
		for (int index = 1; index < LSize; ++index)
		{
			char cRet = left[LSize - index] - '0' + step;

			if (index < RSize)
			{
				cRet += (right[RSize - index] - '0');
			}

			strRet[LSize - index + 1] = (cRet % 10 + '0');
			step = cRet / 10;
		}

		strRet[1] = (step + '0');

		return strRet;
	}
	std::string Sub(std::string left, std::string right)
	{
		//left > right ? left:right
		int LSize = left.size();
		int RSize = right.size();
		if (LSize < RSize || (LSize == RSize && left < right))
		{
			std::swap(left, right);
			std::swap(LSize, RSize);
		}
		char symbol = left[0];

		std::string strRet;
		strRet.resize(LSize);
		strRet[0] = symbol;

		// 111111111111
		// -   11111112
		// 111099999999
		for (int index = 1; index < LSize; index++)
		{
			char cRet = left[LSize - index] - '0';
			if (index < RSize)
			{
				cRet -= (right[RSize - index] - '0');
			}

			if (cRet < 0)
			{
				left[LSize - index - 1] -= 1;
				cRet += 10;
			}

			strRet[LSize - index] = (cRet + '0');
		}

		return strRet;
	}

	std::string Mul(std::string left, std::string right)
	{
		int LSize = left.size();
		int RSize = right.size();
		if (LSize > RSize)
		{
			std::swap(left, right);
			std::swap(LSize, RSize);
		}

		char symbol = '+';
		if (left[0] != right[0])
		{
			symbol = '-';
		}

		std::string strRet;
		strRet.assign(LSize + RSize - 1, '0');
		strRet[0] = symbol;
		int DataLen = strRet.size();
		int Offset = 0;

		for (int index = 1; index < LSize; ++index)
		{
			char Left = left[LSize - index] - '0';
			char step = 0;
			if (0 == Left)
			{
				Offset++;
				continue;
			}

			for (int iRIdx = 1; iRIdx < RSize; ++iRIdx)
			{
				char cRet = Left*(right[RSize - iRIdx] - '0');
				cRet += step;
				cRet += (strRet[DataLen - Offset - iRIdx] - '0');
				strRet[DataLen - Offset - iRIdx] = cRet % 10 + '0';
				step = cRet / 10;
			}

			strRet[DataLen - Offset - RSize] += step;
			Offset++;
		}

		return strRet;
	}

	std::string Div(std::string left, std::string right)
	{
		char symbol = '+';
		if (left[0] != right[0])
		{
			symbol = '-';
		}

		int LSize = left.size();
		int RSize = right.size();
		if (LSize < RSize || LSize == RSize
			&& strcmp(left.c_str() + 1, right.c_str() + 1) < 0)
		{
			return "0";
		}
		else
		{
			if ("+1" == right || "-1" == right)
			{
				left[0] = symbol;
				return left;
			}
		}

		std::string strRet;
		strRet.append(1, symbol);
		char *pLeft = (char*)(left.c_str() + 1);
		char *pRight = (char*)(right.c_str() + 1);
		int DataLen = 1;
		LSize -= 1;
		// "2422222222"  33
		for (int index = 0; index < LSize;)
		{
			if ('0' == *pLeft)
			{
				strRet.append(1, '0');
				pLeft++;
				index++;

				continue;
			}

			if (!IsLeftStrBig(pLeft, DataLen, pRight, RSize - 1))
			{
				strRet.append(1, '0');
				DataLen++;
				if (index + DataLen > LSize)
				{
					break;
				}
				continue;
			}
			else
			{
				strRet.append(1, SubLoop(pLeft, DataLen, pRight, RSize - 1));

				// pLeft
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

	void INT64ToString()
	{
		//12345
		char symbol = '+';
		INT64 temp = m_llValue;
		if (temp < 0)
		{
			symbol = '-';
			temp = 0 - temp;
		}

		m_strData.append(1, symbol);
		int count = 1;
		// 54321
		while (temp)
		{
			m_strData.append(1, temp % 10 + '0');
			temp /= 10;
		}

		char *pLeft = (char*)(m_strData.c_str() + 1);
		char *pRight = (char*)(m_strData.c_str() + m_strData.size() - 1);
		while (pLeft < pRight)
		{
			char ctemp = *pLeft;
			*pLeft++ = *pRight;
			*pRight-- = ctemp;
		}
	}

	bool IsINT64Owerflow()const
	{
		std::string strTemp;
		if ('+' == m_strData[0])
		{
			strTemp = "+9223372036854775807";
		}
		else
		{
			strTemp = "-9223372036854775808";
		}

		if (m_strData.size() > strTemp.size())
		{
			return true;
		}
		else if (m_strData.size() == strTemp.size() && m_strData > strTemp)
		{
			return true;
		}

		return false;
	}

	bool IsLeftStrBig(char *pLeft, size_t LSize, char *pRight, size_t RSize)
	{
		assert(NULL != pLeft && NULL != pRight);
		if (LSize > RSize || LSize == RSize 
			&& strncmp(pLeft, pRight, LSize) >= 0)
		{
			return true;
		}

		return false;
	}

	char SubLoop(char *pLeft, size_t LSize, char *pRight, size_t RSize)
	{
		assert(NULL != pLeft && NULL != pRight);

		char cRet = '0';
		while (true)
		{
			if (!IsLeftStrBig(pLeft, LSize, pRight, RSize))
			{
				break;
			}

			// -=
			int LDataLen = LSize - 1;
			int RDataLen = RSize - 1;
			while (RDataLen >= 0 && LDataLen >= 0)
			{
				if (pLeft[LDataLen] < pRight[RDataLen])
				{
					pLeft[LDataLen - 1] -= 1;
					pLeft[LDataLen] += 10;
				}

				pLeft[LDataLen] = pLeft[LDataLen] - pRight[RDataLen] + '0';
				LDataLen--;
				RDataLen--;
			}

			// "990000000000000000000000000099"
			while ('0' == *pLeft && LSize > 0)
			{
				pLeft++;
				LSize--;
			}

			cRet++;
		}

		return cRet;
	}


private:
	long long m_llValue;
	std::string m_strData;
};
BigData BigData::operator+(BigData& bigData)
{
	if (!IsINT64Owerflow() && !bigData.IsINT64Owerflow())
	{
		// '+' '-'||'-' '+'  7 + (-3)  10
		if (m_strData[0] != bigData.m_strData[0])
		{
			return BigData(m_llValue + bigData.m_llValue);
		}
		else
		{
			// '+' '+' 3 + 7  10 - 7 >= 3
			// '-' '-' -3 + (-7)  -10 - (-7) =< -3
			if (('+' == m_strData[0] && MAX_INT64 - m_llValue >= bigData.m_llValue) ||
				('-') == m_strData[0] && MIN_INT64 - m_llValue <= bigData.m_llValue)
			{
				return BigData(m_llValue + bigData.m_llValue);
			}
		}
	}

	std::string strRet;
	if (m_strData[0] == bigData.m_strData[0])
	{
		strRet = Add(m_strData, bigData.m_strData);
	}
	else
	{
		strRet = Sub(m_strData, bigData.m_strData);
	}

	return BigData(strRet.c_str());
}
BigData BigData::operator-(const BigData& bigData)
{
	if (!IsINT64Owerflow() && !bigData.IsINT64Owerflow())
	{
		//7 - 3  -7 - (-3)
		if (m_strData[0] == bigData.m_strData[0])
		{
			return BigData(m_llValue - bigData.m_llValue);
		}
		else
		{
			// '+' '-' 10 + (-7) >= 3
			// '-' '+' -10 + 3 <= -7 
			if (('+' == m_strData[0] && MAX_INT64 + bigData.m_llValue >= m_llValue) ||
				('-' == m_strData[0] && MIN_INT64 + bigData.m_llValue <= m_llValue))
			{
				return BigData(m_llValue - bigData.m_llValue);
			}
		}
	}

	std::string strRet;
	if (m_strData[0] != bigData.m_strData[0])
	{
		strRet = Add(m_strData, bigData.m_strData);
	}
	else
	{
		strRet = Sub(m_strData, bigData.m_strData);
	}
	return BigData(strRet.c_str());
}
BigData BigData::operator*(const BigData& bigData)
{
	if (0 == m_llValue || 0 == bigData.m_llValue)
	{
		return BigData(INT64(0));
	}

	if (!IsINT64Owerflow() && !bigData.IsINT64Owerflow())
	{
		if (m_strData[0] == bigData.m_strData[0])
		{
			//'+''+' 10 /2 = 5 >= 1 2 3 4 5
			//'-''-' 10 /-2 = -5 <= -5 -4 -3 -2 -1 
			if (('+' == m_strData[0] && MAX_INT64 / m_llValue >= bigData.m_llValue) ||
				('-' == m_strData[0] && MAX_INT64 / m_llValue <= bigData.m_llValue))
			{
				return BigData(m_llValue*bigData.m_llValue);
			}
		}
		else
		{
			//'+''-' -10 /2 = -5 <= -5 -4 -3 -2 -1
			//'-''+' -10/-2 = 5 >= 5 4 3 2 1
			if (('+' == m_strData[0] && MIN_INT64 / m_llValue <= bigData.m_llValue) ||
				('-' == m_strData[0] && MIN_INT64 / m_llValue >= bigData.m_llValue))
			{
				return BigData(m_llValue*bigData.m_llValue);
			}
		}
	}

	return BigData(Mul(m_strData, bigData.m_strData).c_str());
}
BigData BigData::operator/(const BigData& bigData)
{
	if (0 == bigData.m_llValue)
	{
		assert("除数不能为0！");
		return BigData(INT64(0));
	}

	if (!IsINT64Owerflow() && !bigData.IsINT64Owerflow())
	{
		return BigData(m_llValue / bigData.m_llValue);
	}

	return BigData(Div(m_strData, bigData.m_strData).c_str());
}