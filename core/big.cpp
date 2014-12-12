/*
 * big.cpp
 * Blaze Game Engine v0.11
 *
 * Created by Ned Bingham on June 10, 2012.
 * Modified by Ned Bingham on June 10, 2012.
 * Copyright 2011 Sol Union. All rights reserved.
 *
 * Blaze Game Engine v0.11 is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * Blaze Game Engine v0.11 is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Blaze Game Engine v0.11.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "big.h"
#include "math.h"
#include <float.h>

namespace core
{

// Integers

Integer::Integer()
{
	sign = 0;
}

Integer::Integer(int i)
{
	data.push_back(i & 0x7FFFFFFF);
	if (i < 0)
		sign = 0x7FFFFFFF;
	else
		sign = 0;
}

Integer::Integer(unsigned int i)
{
	while (i > 0)
	{
		data.push_back((int)(i & 0x7FFFFFFF));
		i >>= 31;
	}
	sign = 0;
}

Integer::Integer(long long i)
{
	bool neg = (i < 0);
	i *= -1;

	while (i > 0)
	{
		data.push_back((int)(i & 0x7FFFFFFF));
		i >>= 31;
	}
	sign = 0;

	if (neg)
		*this = -*this;
}

Integer::Integer(unsigned long long i)
{
	while (i > 0)
	{
		data.push_back((int)(i & 0x7FFFFFFF));
		i >>= 31;
	}
	sign = 0;
}

Integer::Integer(float i)
{
	bool neg = false;
	if (i < 0)
	{
		neg = true;
		i = -i;
	}

	sign = 0;

	while (i != 0)
	{
		i /= 2147483648.0;
		double fractional = (i - trunc(i));
		data.push_back((unsigned int)(2147483648.0*fractional));
		i -= fractional;
	}

	if (neg)
		*this = -*this;
}

Integer::Integer(double i)
{
	bool neg = false;
	if (i < 0)
	{
		neg = true;
		i = -i;
	}

	sign = 0;

	while (i != 0)
	{
		i /= 2147483648.0;
		double fractional = (i - trunc(i));
		data.push_back((unsigned int)(2147483648.0*fractional));
		i -= fractional;
	}

	if (neg)
		*this = -*this;
}

Integer::~Integer()
{
}

Integer::operator double()
{
	double result = 0;
	for (int i = 0; i < data.size(); i++)
		result += ::pow(2147483648.0, (double)i)*(double)(data[i] ^ sign);

	if (sign != 0)
	{
		result += 1.0;
		result *= -1.0;
	}

	return result;
}

int Integer::size()
{
	return (data.size()*(sizeof(int)*8 - 1) + 7)/8;
}

Integer &Integer::operator=(Integer i)
{
	sign = i.sign;
	data = i.data;
	return *this;
}

Integer &Integer::operator+=(Integer i)
{
	int m = min(data.size(), i.data.size());
	int carry = 0;
	for (int j = 0; j < m; j++)
	{
		data[j] += i.data[j] + carry;
		carry = (int)(data[j] < 0);
		data[j] &= 0x7FFFFFFF;
	}
	for (int j = m; j < i.data.size(); j++)
	{
		data.push_back(i.data[j] + sign + carry);
		carry = (int)(data[j] < 0);
		data[j] &= 0x7FFFFFFF;
	}
	for (int j = m; j < data.size(); j++)
	{
		data[j] += i.sign + carry;
		carry = (int)(data[j] < 0);
		data[j] &= 0x7FFFFFFF;
	}

	if (sign != i.sign)
		sign = (sign + i.sign + carry) & 0x7FFFFFFF;
	else if (sign == 0 && carry == 1)
		data.push_back(1);
	else if (sign == 0x7FFFFFFF && carry == 0)
		data.push_back(0x7FFFFFFE);

	return *this;
}

Integer &Integer::operator-=(Integer i)
{
	int m = min(data.size(), i.data.size());
	int carry = 1;
	int isign = (~i.sign & 0x7FFFFFFF);
	for (int j = 0; j < m; j++)
	{
		data[j] += (~i.data[j] & 0x7FFFFFFF) + carry;
		carry = (int)(data[j] < 0);
		data[j] &= 0x7FFFFFFF;
	}
	for (int j = m; j < i.data.size(); j++)
	{
		data.push_back((~i.data[j] & 0x7FFFFFFF) + sign + carry);
		carry = (int)(data[j] < 0);
		data[j] &= 0x7FFFFFFF;
	}
	for (int j = m; j < data.size(); j++)
	{
		data[j] += isign + carry;
		carry = (int)(data[j] < 0);
		data[j] &= 0x7FFFFFFF;
	}

	if (sign != isign)
		sign = (sign + isign + carry) & 0x7FFFFFFF;
	else if (sign == 0 && carry == 1)
		data.push_back(1);
	else if (sign == 0x7FFFFFFF && carry == 0)
		data.push_back(0x7FFFFFFE);

	return *this;
}

Integer &Integer::operator*=(Integer i)
{
	*this = *this * i;
	return *this;
}

Integer &Integer::operator/=(Integer i)
{
	*this = *this / i;
	return *this;
}

Integer &Integer::operator%=(Integer i)
{
	*this = *this % i;
	return *this;
}

Integer &Integer::operator&=(Integer i)
{
	int m = min(data.size(), i.data.size());
	for (int j = 0; j < m; j++)
		data[j] &= i.data[j];
	for (int j = m; j < data.size(); j++)
		data[j] &= i.sign;
	for (int j = m; j < i.data.size(); j++)
		data.push_back(sign & i.data[j]);
	sign &= i.sign;
	return *this;
}

Integer &Integer::operator|=(Integer i)
{
	int m = min(data.size(), i.data.size());
	for (int j = 0; j < m; j++)
		data[j] |= i.data[j];
	for (int j = m; j < data.size(); j++)
		data[j] |= i.sign;
	for (int j = m; j < i.data.size(); j++)
		data.push_back(sign | i.data[j]);
	sign |= i.sign;
	return *this;
}

Integer &Integer::operator^=(Integer i)
{
	int m = min(data.size(), i.data.size());
	for (int j = 0; j < m; j++)
		data[j] ^= i.data[j];
	for (int j = m; j < data.size(); j++)
		data[j] ^= i.sign;
	for (int j = m; j < i.data.size(); j++)
		data.push_back(sign ^ i.data[j]);
	sign ^= i.sign;
	return *this;
}

Integer &Integer::operator<<=(Integer i)
{
	*this = *this << i;
	return *this;
}

Integer &Integer::operator>>=(Integer i)
{
	*this = *this >> i;
	return *this;
}

file &operator<<(file &f, Integer i)
{
	f << (double)i << " " << i.data << " " << i.sign;
	return f;
}

Integer operator-(Integer i)
{
	int carry = 1;
	for (int j = 0; j < i.data.size(); j++)
	{
		i.data[j] = (~i.data[j] & 0x7FFFFFFF) + carry;
		carry = (int)(i.data[j] < 0);
		i.data[j] &= 0x7FFFFFFF;
	}

	i.sign = ~i.sign & 0x7FFFFFFF;

	if (i.sign == 0x7FFFFFFF)
		i.sign = (i.sign + carry) & 0x7FFFFFFF;
	else if (carry > 0)
		i.data.push_back(1);

	return i;
}

Integer operator+(Integer i1, Integer i2)
{
	Integer result;
	result.data.reserve(max(i1.data.size(), i2.data.size()));

	int m = min(i1.data.size(), i2.data.size());
	int carry = 0;
	for (int j = 0; j < m; j++)
	{
		result.data.push_back(i1.data[j] + i2.data[j] + carry);
		carry = (int)(result.data[j] < 0);
		result.data[j] &= 0x7FFFFFFF;
	}
	for (int j = m; j < i2.data.size(); j++)
	{
		result.data.push_back(i2.data[j] + i1.sign + carry);
		carry = (int)(result.data[j] < 0);
		result.data[j] &= 0x7FFFFFFF;
	}
	for (int j = m; j < i1.data.size(); j++)
	{
		result.data.push_back(i1.data[j] + i2.sign + carry);
		carry = (int)(result.data[j] < 0);
		result.data[j] &= 0x7FFFFFFF;
	}

	if (i1.sign != i2.sign)
		result.sign = (i1.sign + i2.sign + carry) & 0x7FFFFFFF;
	else if (i1.sign == 0)
	{
		if (carry == 1)
			result.data.push_back(1);
		result.sign = 0;
	}
	else if (i1.sign == 0x7FFFFFFF)
	{
		if (carry == 0)
			result.data.push_back(0x7FFFFFFE);
		result.sign = 0x7FFFFFFF;
	}

	while (result.data.back() == result.sign)
		result.data.pop_back();

	return result;
}

Integer operator-(Integer i1, Integer i2)
{
	Integer result;
	result.data.reserve(max(i1.data.size(), i2.data.size()));

	int m = min(i1.data.size(), i2.data.size());
	int i2sign = (~i2.sign & 0x7FFFFFFF);
	int carry = 1;
	for (int j = 0; j < m; j++)
	{
		result.data.push_back(i1.data[j] + (~i2.data[j] & 0x7FFFFFFF) + carry);
		carry = (int)(result.data[j] < 0);
		result.data[j] &= 0x7FFFFFFF;
	}
	for (int j = m; j < i2.data.size(); j++)
	{
		result.data.push_back((~i2.data[j] & 0x7FFFFFFF) + i1.sign + carry);
		carry = (int)(result.data[j] < 0);
		result.data[j] &= 0x7FFFFFFF;
	}
	for (int j = m; j < i1.data.size(); j++)
	{
		result.data.push_back(i1.data[j] + i2sign + carry);
		carry = (int)(result.data[j] < 0);
		result.data[j] &= 0x7FFFFFFF;
	}

	if (i1.sign != i2sign)
		result.sign = (i1.sign + i2sign + carry) & 0x7FFFFFFF;
	else if (i1.sign == 0)
	{
		if (carry == 1)
			result.data.push_back(1);
		result.sign = 0;
	}
	else if (i1.sign == 0x7FFFFFFF)
	{
		if (carry == 0)
			result.data.push_back(0x7FFFFFFE);
		result.sign = 0x7FFFFFFF;
	}

	while (result.data.back() == result.sign)
		result.data.pop_back();

	return result;
}

Integer operator*(Integer i1, Integer i2)
{
	array<array<int> > partial_products;
	partial_products.extend(i1.data.size() + i2.data.size()+1);

	int vali, valj;

	for (int i = 0; i < i1.data.size()*2; i++)
	{
		if (i < i1.data.size())
			vali = i1.data[i];
		else
			vali = i1.sign;

		if (vali != 0)
			for (int j = 0; j < i2.data.size()*2; j++)
			{
				if (j < i2.data.size())
					valj = i2.data[j];
				else
					valj = i2.sign;

				if (i+j < i1.data.size() + i2.data.size())
				{
					int i00, i01, i10, i11;
					i00 = (vali & 0x0000FFFF) * (valj & 0x0000FFFF);
					i10 = ((vali & 0xFFFF0000) >> 16) * (valj & 0x0000FFFF);
					i01 = (vali & 0x0000FFFF) * ((valj & 0xFFFF0000) >> 16);
					i11 = (((vali & 0xFFFF0000) >> 16) * ((valj & 0xFFFF0000) >> 16)) << 1;

					partial_products[i+j].push_back(i00);
					partial_products[i+j].push_back((i10 << 16) & 0x7FFFFFFF);
					partial_products[i+j].push_back((i01 << 16) & 0x7FFFFFFF);
					partial_products[i+j+1].push_back((i10 >> 15) & 0x0000FFFF);
					partial_products[i+j+1].push_back((i01 >> 15) & 0x0000FFFF);
					partial_products[i+j+1].push_back(i11);
				}
			}
	}

	Integer result;
	result.data.reserve(partial_products.size());
	result.sign = i1.sign ^ i2.sign;

	for (int i = 0; i < partial_products.size()-1; i++)
	{
		int carry = 0;
		if (partial_products[i].size() > 0 && partial_products[i].back() < 0)
		{
			carry++;
			partial_products[i].back() &= 0x7FFFFFFF;
		}

		for (int j = partial_products[i].size()-1; j > 0; j--)
		{
			if (partial_products[i][j-1] < 0)
			{
				carry++;
				partial_products[i][j-1] &= 0x7FFFFFFF;
			}

			partial_products[i][j-1] += partial_products[i][j];
			partial_products[i].pop_back();
			if (partial_products[i][j-1] < 0)
			{
				carry++;
				partial_products[i][j-1] &= 0x7FFFFFFF;
			}
		}

		if (partial_products[i].size() > 0)
			result.data.push_back(partial_products[i][0]);
		else
			result.data.push_back(0);

		if (carry > 0 && i+1 < partial_products.size())
			partial_products[i+1].push_back(carry);
		else if (carry > 0)
			partial_products.push_back(array<int>(1, carry));
	}

	while (result.data.back() == result.sign)
		result.data.pop_back();

	return result;
}

Integer operator/(Integer i1, Integer i2)
{
	bool neg = false;
	if (i1.sign != 0 && i2.sign != 0)
	{
		i1 = -i1;
		i2 = -i2;
	}
	else if (i1.sign != 0)
	{
		neg = true;
		i1 = -i1;
	}
	else if (i2.sign != 0)
	{
		neg = true;
		i2 = -i2;
	}

	Integer quotient;
	Integer remainder;
	for (int i = i1.data.size()-1; i >= 0; i--)
	{
		// shift the remainder left by 1 place
		remainder.data.push_front(i1.data[i]);

		// At this point, I am guaranteed that remainder.data.size() - i2.data.size() <= 1
		// y > x, worst case is if y = x+1
		// (x*2^31 + z)/x = 2^31 + z/x < 2^32
		unsigned long long ll = 0;
		for (int j = remainder.data.size()-1; j >= i2.data.size()-1; j--)
		{
			ll <<= 31;
			ll |= remainder.data[j];
		}

		ll /= (i2.data.back()+1);
		Integer result(ll);
		remainder -= i2*result;
		int k = 0;
		while (remainder >= i2)
		{
			remainder -= i2;
			result += 1;
			k++;
		}

		quotient += (result << (i*31));
	}

	if (neg)
		return -quotient;
	else
		return quotient;
}

Integer operator%(Integer i1, Integer i2)
{
	bool neg = false;
	if (i1.sign != 0 && i2.sign != 0)
	{
		i1 = -i1;
		i2 = -i2;
	}
	else if (i1.sign != 0)
	{
		neg = true;
		i1 = -i1;
	}
	else if (i2.sign != 0)
	{
		neg = true;
		i2 = -i2;
	}

	Integer quotient;
	Integer remainder;
	for (int i = i1.data.size()-1; i >= 0; i--)
	{
		// shift the remainder left by 1 place
		remainder.data.push_front(i1.data[i]);

		// At this point, I am guaranteed that remainder.data.size() - i2.data.size() <= 1
		// y > x, worst case is if y = x+1
		// (x*2^31 + z)/x = 2^31 + z/x < 2^32
		unsigned long long ll = 0;
		for (int j = remainder.data.size()-1; j >= i2.data.size()-1; j--)
		{
			ll <<= 31;
			ll |= remainder.data[j];
		}

		ll /= (i2.data.back()+1);
		Integer result(ll);
		remainder -= i2*result;
		int k = 0;
		while (remainder >= i2)
		{
			remainder -= i2;
			result += 1;
			k++;
		}

		quotient += (result << (i*31));
	}

	if (neg)
		return -remainder;
	else
		return remainder;
}

bool operator==(Integer i1, Integer i2)
{
	int m = min(i1.data.size(), i2.data.size());
	for (int i = 0; i < m; i++)
		if (i1.data[i] != i2.data[i])
			return false;
	for (int i = m; i < i1.data.size(); i++)
		if (i1.data[i] != i2.sign)
			return false;
	for (int i = m; i < i2.data.size(); i++)
		if (i2.data[i] != i1.sign)
			return false;

	return (i1.sign == i2.sign);
}

bool operator!=(Integer i1, Integer i2)
{
	int m = min(i1.data.size(), i2.data.size());
	for (int i = 0; i < m; i++)
		if (i1.data[i] != i2.data[i])
			return true;
	for (int i = m; i < i1.data.size(); i++)
		if (i1.data[i] != i2.sign)
			return true;
	for (int i = m; i < i2.data.size(); i++)
		if (i2.data[i] != i1.sign)
			return true;

	return (i1.sign != i2.sign);
}

bool operator>(Integer i1, Integer i2)
{
	if (i1.sign != i2.sign)
		return (i1.sign < i2.sign);

	int m = min(i1.data.size(), i2.data.size());
	for (int i = i1.data.size()-1; i >= i2.data.size(); i--)
		if (i1.data[i] != i2.sign)
			return (i1.data[i] > i2.sign);
	for (int i = i2.data.size()-1; i >= i1.data.size(); i--)
		if (i2.data[i] != i1.sign)
			return (i1.sign > i2.data[i]);
	for (int i = m-1; i >= 0; i--)
		if (i1.data[i] != i2.data[i])
			return (i1.data[i] > i2.data[i]);

	return false;
}

bool operator<(Integer i1, Integer i2)
{
	if (i1.sign != i2.sign)
		return (i1.sign > i2.sign);

	int m = min(i1.data.size(), i2.data.size());
	for (int i = i1.data.size()-1; i >= i2.data.size(); i--)
		if (i1.data[i] != i2.sign)
			return (i1.data[i] < i2.sign);
	for (int i = i2.data.size()-1; i >= i1.data.size(); i--)
		if (i2.data[i] != i1.sign)
			return (i1.sign < i2.data[i]);
	for (int i = m-1; i >= 0; i--)
		if (i1.data[i] != i2.data[i])
			return (i1.data[i] < i2.data[i]);

	return false;
}

bool operator>=(Integer i1, Integer i2)
{
	if (i1.sign != i2.sign)
		return (i1.sign < i2.sign);

	int m = min(i1.data.size(), i2.data.size());
	for (int i = i1.data.size()-1; i >= i2.data.size(); i--)
		if (i1.data[i] != i2.sign)
			return (i1.data[i] > i2.sign);
	for (int i = i2.data.size()-1; i >= i1.data.size(); i--)
		if (i2.data[i] != i1.sign)
			return (i1.sign > i2.data[i]);
	for (int i = m-1; i >= 0; i--)
		if (i1.data[i] != i2.data[i])
			return (i1.data[i] > i2.data[i]);

	return true;
}

bool operator<=(Integer i1, Integer i2)
{
	if (i1.sign != i2.sign)
		return (i1.sign > i2.sign);

	int m = min(i1.data.size(), i2.data.size());
	for (int i = i1.data.size()-1; i >= i2.data.size(); i--)
		if (i1.data[i] != i2.sign)
			return (i1.data[i] < i2.sign);
	for (int i = i2.data.size()-1; i >= i1.data.size(); i--)
		if (i2.data[i] != i1.sign)
			return (i1.sign < i2.data[i]);
	for (int i = m-1; i >= 0; i--)
		if (i1.data[i] != i2.data[i])
			return (i1.data[i] < i2.data[i]);

	return true;
}

Integer operator&(Integer i1, Integer i2)
{
	Integer result;
	result.data.reserve(max(i1.data.size(), i2.data.size()));
	int m = min(i1.data.size(), i2.data.size());
	for (int i = 0; i < m; i++)
		result.data.push_back(i1.data[i] & i2.data[i]);
	for (int i = m; i < i1.data.size(); i++)
		result.data.push_back(i1.data[i] & i2.sign);
	for (int i = m; i < i2.data.size(); i++)
		result.data.push_back(i1.sign & i2.data[i]);
	result.sign = i1.sign & i2.sign;
	return result;
}

Integer operator|(Integer i1, Integer i2)
{
	Integer result;
	result.data.reserve(max(i1.data.size(), i2.data.size()));
	int m = min(i1.data.size(), i2.data.size());
	for (int i = 0; i < m; i++)
		result.data.push_back(i1.data[i] | i2.data[i]);
	for (int i = m; i < i1.data.size(); i++)
		result.data.push_back(i1.data[i] | i2.sign);
	for (int i = m; i < i2.data.size(); i++)
		result.data.push_back(i1.sign | i2.data[i]);
	result.sign = i1.sign | i2.sign;
	return result;
}

Integer operator^(Integer i1, Integer i2)
{
	Integer result;
	result.data.reserve(max(i1.data.size(), i2.data.size()));
	int m = min(i1.data.size(), i2.data.size());
	for (int i = 0; i < m; i++)
		result.data.push_back(i1.data[i] ^ i2.data[i]);
	for (int i = m; i < i1.data.size(); i++)
		result.data.push_back(i1.data[i] ^ i2.sign);
	for (int i = m; i < i2.data.size(); i++)
		result.data.push_back(i1.sign ^ i2.data[i]);
	result.sign = i1.sign ^ i2.sign;
	return result;
}

Integer operator<<(Integer i1, int i2)
{
	if (i2 < 0)
		return i1 >> (-i2);
	else if (i1.data.size() == 0)
		return i1;
	else
	{
		int a = i2/31;
		int b = i2%31;

		Integer result;
		result.data.resize(i1.data.size() + a + 1);

		result.data.back() = ((i1.sign << b) & 0x7FFFFFFF) | ((i1.data.back() >> (31 - b)) & 0x7FFFFFFF);
		for (int i = a+1; i < result.data.size()-1; i++)
			result.data[i] = ((i1.data[i-a] << b) & 0x7FFFFFFF) | ((i1.data[i-a-1] >> (31 - b)) & 0x7FFFFFFF);
		result.data[a] = (i1.data[0] << b) & 0x7FFFFFFF;
		for (int i = 0; i < a; i++)
			result.data[i] = 0;
		result.sign = i1.sign;

		while (result.data.back() == result.sign)
			result.data.pop_back();

		return result;
	}
}

Integer operator>>(Integer i1, int i2)
{
	if (i2 < 0)
		return i1 << (-i2);
	else
	{
		int a = i2/31;
		int b = i2%31;

		Integer result;
		if (i1.data.size() > a + 1)
		{
			result.data.reserve(i1.data.size() - a);

			for (int i = 0; i < i1.data.size() - a - 1; i++)
				result.data.push_back(((i1.data[i+a] >> b) | (i1.data[i+a+1] << (31 - b))) & 0x7FFFFFFF);

			result.data.push_back(((i1.data.back() >> b) | (i1.sign << (31 - b))) & 0x7FFFFFFF);
			result.sign = i1.sign;

			while (result.data.back() == result.sign)
				result.data.pop_back();
		}

		return result;
	}
}

Integer abs(Integer i)
{
	if (i.sign == 0x7FFFFFFF)
		return -i;
	else
		return i;
}

Integer pow(Integer f, int p)
{
	Integer result;
	while (p > 0)
	{
		if ((p & 1) != 0)
			result += f;
		p >>= 1;
		f *= f;
	}
	return result;
}

Integer sqrt(Integer f)
{
	// TODO
	return Integer();
}

Integer root(Integer f, int r)
{
	// TODO
	return Integer();
}

Real::Real()
{
	exp = 0;
	limit = 5;
}

Real::Real(int f) : num(f)
{
	exp = 0;
	limit = 5;
}

Real::Real(unsigned int f) : num(f)
{
	exp = 0;
	limit = 5;
}

Real::Real(long long f) : num(f)
{
	exp = 0;
	limit = 5;
}

Real::Real(unsigned long long f) : num(f)
{
	exp = 0;
	limit = 5;
}

Real::Real(float f)
{
	double sig = frexp(f, &exp);
	int *fi = (int*)&sig;
	num.data.push_back(*fi & 0x7FFFFFFF);
	num.data.push_back(((*(fi+1) & 0x000FFFFF) << 1) | ((*fi & 0x80000000) >> 31) | 0x00200000);
	exp -= 53;
	if ((*fi & 0x80000000) != 0)
		num = -num;
	limit = 5;
}

Real::Real(double f)
{
	double sig = frexp(f, &exp);
	int *fi = (int*)&sig;
	num.data.push_back(*fi & 0x7FFFFFFF);
	num.data.push_back(((*(fi+1) & 0x000FFFFF) << 1) | ((*fi & 0x80000000) >> 31) | 0x00200000);
	exp -= 53;
	if ((*(fi+1) & 0x80000000) != 0)
		num = -num;
	limit = 5;
}

Real::~Real()
{

}

Real::operator double()
{
	return (double)num*::pow(2.0, (double)exp);
}

Real &Real::operator=(Real f)
{
	exp = f.exp;
	num = f.num;
	return *this;
}

Real &Real::operator+=(Real f)
{
	*this = *this + f;
	return *this;
}

Real &Real::operator-=(Real f)
{
	*this = *this - f;
	return *this;
}

Real &Real::operator*=(Real f)
{
	*this = *this * f;
	return *this;
}

Real &Real::operator/=(Real f)
{
	*this = *this / f;
	return *this;
}

file &operator<<(file &fout, Real f)
{
	fout << (double)f;
	return fout;
}

Real operator-(Real f)
{
	f.num = -f.num;
	return f;
}

Real operator+(Real f1, Real f2)
{
	Real result;
	result.exp = f1.exp;
	if (f1.exp > f2.exp)
	{
		f1.num <<= (f1.exp - f2.exp);
		result.exp = f2.exp;
	}
	else if (f2.exp > f1.exp)
	{
		f2.num <<= (f2.exp - f1.exp);
		result.exp = f1.exp;
	}

	result.num = f1.num + f2.num;
	return result;
}

Real operator-(Real f1, Real f2)
{
	Real result;
	result.exp = f1.exp;
	if (f1.exp > f2.exp)
	{
		f1.num <<= (f1.exp - f2.exp);
		result.exp = f2.exp;
	}
	else if (f2.exp > f1.exp)
	{
		f2.num <<= (f2.exp - f1.exp);
		result.exp = f1.exp;
	}

	result.num = f1.num - f2.num;
	return result;
}

Real operator*(Real f1, Real f2)
{
	Real result;
	result.num = f1.num*f2.num;
	result.exp = f1.exp + f2.exp;
	return result;
}

Real operator/(Real f1, Real f2)
{
	bool neg = false;
	if (f1.num.sign != 0 && f2.num.sign != 0)
	{
		f1 = -f1;
		f2 = -f2;
	}
	else if (f1.num.sign != 0)
	{
		neg = true;
		f1 = -f1;
	}
	else if (f2.num.sign != 0)
	{
		neg = true;
		f2 = -f2;
	}

	Real quotient;
	quotient.exp = f1.exp - f2.exp;
	quotient.limit = max(f1.limit, f2.limit);
	Integer remainder;
	for (int i = f1.num.data.size()-1; i >= 0; i--)
	{
		// shift the remainder left by 1 place
		remainder.data.push_front(f1.num.data[i]);

		// At this point, I am guaranteed that remainder.data.size() - f2.data.size() <= 1
		// y > x, worst case is if y = x+1
		// (x*2^31 + z)/x = 2^31 + z/x < 2^32
		unsigned long long ll = 0;
		for (int j = remainder.data.size()-1; j >= f2.num.data.size()-1; j--)
		{
			ll <<= 31;
			ll |= remainder.data[j];
		}

		ll /= (f2.num.data.back()+1);
		Integer result(ll);
		remainder -= f2.num*result;
		int k = 0;
		while (remainder >= f2.num)
		{
			remainder -= f2.num;
			result += 1;
			k++;
		}

		quotient.num += (result << (i*31));
	}

	while (remainder != 0 && quotient.num.data.size() < quotient.limit)
	{
		// shift the remainder left by 1 place
		remainder.data.push_front(0);

		// At this point, I am guaranteed that remainder.data.size() - f2.data.size() <= 1
		// y > x, worst case is if y = x+1
		// (x*2^31 + z)/x = 2^31 + z/x < 2^32
		unsigned long long ll = 0;
		for (int j = remainder.data.size()-1; j >= f2.num.data.size()-1; j--)
		{
			ll <<= 31;
			ll |= remainder.data[j];
		}

		ll /= (f2.num.data.back()+1);
		Integer result(ll);
		remainder -= f2.num*result;
		int k = 0;
		while (remainder >= f2.num)
		{
			remainder -= f2.num;
			result += 1;
			k++;
		}

		quotient.num = (quotient.num << 31) + result;
		quotient.exp -= 31;
	}

	if (neg)
		return -quotient;
	else
		return quotient;
}

bool operator==(Real f1, Real f2)
{
	if (f1.exp > f2.exp)
	{
		f1.num <<= (f1.exp - f2.exp);
		f1.exp = f2.exp;
	}
	else if (f2.exp > f1.exp)
	{
		f2.num <<= (f2.exp - f1.exp);
		f2.exp = f1.exp;
	}

	return (f1.num == f2.num);
}

bool operator!=(Real f1, Real f2)
{
	if (f1.exp > f2.exp)
	{
		f1.num <<= (f1.exp - f2.exp);
		f1.exp = f2.exp;
	}
	else if (f2.exp > f1.exp)
	{
		f2.num <<= (f2.exp - f1.exp);
		f2.exp = f1.exp;
	}

	return (f1.num != f2.num);
}

bool operator>(Real f1, Real f2)
{
	if (f1.exp > f2.exp)
	{
		f1.num <<= (f1.exp - f2.exp);
		f1.exp = f2.exp;
	}
	else if (f2.exp > f1.exp)
	{
		f2.num <<= (f2.exp - f1.exp);
		f2.exp = f1.exp;
	}

	return (f1.num > f2.num);
}

bool operator<(Real f1, Real f2)
{
	if (f1.exp > f2.exp)
	{
		f1.num <<= (f1.exp - f2.exp);
		f1.exp = f2.exp;
	}
	else if (f2.exp > f1.exp)
	{
		f2.num <<= (f2.exp - f1.exp);
		f2.exp = f1.exp;
	}

	return (f1.num < f2.num);
}

bool operator>=(Real f1, Real f2)
{
	if (f1.exp > f2.exp)
	{
		f1.num <<= (f1.exp - f2.exp);
		f1.exp = f2.exp;
	}
	else if (f2.exp > f1.exp)
	{
		f2.num <<= (f2.exp - f1.exp);
		f2.exp = f1.exp;
	}

	return (f1.num >= f2.num);
}

bool operator<=(Real f1, Real f2)
{
	if (f1.exp > f2.exp)
	{
		f1.num <<= (f1.exp - f2.exp);
		f1.exp = f2.exp;
	}
	else if (f2.exp > f1.exp)
	{
		f2.num <<= (f2.exp - f1.exp);
		f2.exp = f1.exp;
	}

	return (f1.num <= f2.num);
}

/*Real sqrt(Real f)
{
	Real result(0, f.prec());
	mpf_sqrt(result.value, f.value);
	return result;
}

Real pow(Real f, int p)
{
	Real result(0, f.prec());
	mpf_pow_ui(result.value, f.value, p);
	return result;
}

Real abs(Real f)
{
	Real result(0, f.prec());
	mpf_abs(result.value, f.value);
	return result;
}*/

}