/*
 * bits.h
 *
 *  Created on: Jun 20, 2015
 *      Author: nbingham
 */

#pragma once

#include <std/array.h>
#include <std/pair.h>
#include <std/implier.h>

namespace core
{

struct bits : array<unsigned char>
{
	bits();
	~bits();

	using typename array<unsigned char>::type;
	using typename array<unsigned char>::iterator;
	using typename array<unsigned char>::const_iterator;
	
	using array<unsigned char>::begin;
	using array<unsigned char>::end;
	using array<unsigned char>::rbegin;
	using array<unsigned char>::rend;
	using array<unsigned char>::sub;
	using array<unsigned char>::size;

	using array<unsigned char>::operator=;
};

bits &operator<<(bits &str, char v);
bits &operator<<(bits &str, short v);
bits &operator<<(bits &str, int v);
bits &operator<<(bits &str, long v);
bits &operator<<(bits &str, long long v);
bits &operator<<(bits &str, unsigned char v);
bits &operator<<(bits &str, unsigned short v);
bits &operator<<(bits &str, unsigned int v);
bits &operator<<(bits &str, unsigned long v);
bits &operator<<(bits &str, unsigned long long v);
bits &operator<<(bits &str, bool v);
bits &operator<<(bits &str, float v);
bits &operator<<(bits &str, double v);
bits &operator<<(bits &str, const char *v);
bits &operator<<(bits &str, char *v);

template <typename container>
bits &operator<<(bits &str, const container &v)
{
	for (typename container::const_iterator i = v.begin(); i != v.end(); i++)
		str << *i;
	return str;
}

template <typename type0, typename type1>
bits &operator<<(bits &str, const pair<type0, type1> &v)
{
	str << v.first << v.second;
	return str;
}

template <typename type0, typename type1>
bits &operator<<(bits &str, const implier<type0, type1> &v)
{
	str << v.key << v.value;
	return str;
}

template <typename T>
bits &hash_data(bits &str, T value)
{
	str << value;
	return str;
}

template <typename type0, typename type1>
bits &hash_data(bits &str, const implier<type0, type1> &value)
{
	str << value.key;
	return str;
}

}
