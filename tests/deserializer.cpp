/****************************************************************
 *
 * Copyright (C) 2012 Alessandro Pignotti <alessandro@leaningtech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 ***************************************************************/

#include <stdlib.h>
#include <assert.h>
#include "server.h"

static bool isJSONWhitespace(char c)
{
	return c==0x20 ||
		c==0x09 ||
		c==0x0a ||
		c==0x0d;
}

static void skipJSONWhitespace(char*& data)
{
	while(isJSONWhitespace(*data))
		data++;
}

static bool isJSONDigit(char c)
{
	return c>='0' && c<='9';
}

static char getJSONDigit(char c)
{
	assert(isJSONDigit(c));
	return c-'0';
}

template<>
int deserialize(char*& data)
{
	//JSON decoder for int
	//derived from number, just expect no decimal part
	int ret=0;
	int sign=1;
	//1) Get sign
	skipJSONWhitespace(data);
	if(*data=='-')
	{
		sign=-1;
		data++;
	}
	if(!isJSONDigit(*data))
		throw DeserializationException("Invalid format for int");

	//2) Get int
	while(isJSONDigit(*data))
	{
		ret*=10;
		ret+=getJSONDigit(*data);
		data++;
	}
	ret*=sign;
	//Stop here, no other stuff is allowed, parsing will fail later if there is
	//a fractional part
	return ret;
}

template<>
float deserialize(char*& data)
{
	//JSON decoder for number
	float ret=0;
	int sign=1;
	//1) Get sign
	skipJSONWhitespace(data);
	if(*data=='-')
	{
		sign=-1;
		data++;
	}
	if(!isJSONDigit(*data))
		throw DeserializationException("Invalid format for number");

	//2) Get int
	while(isJSONDigit(*data))
	{
		ret*=10;
		ret+=getJSONDigit(*data);
		data++;
	}

	//3) Get frac
	if(*data=='.')
	{
		data++;
		if(!isJSONDigit(*data))
			throw DeserializationException("Invalid format for number");
		float multiplier=1;
		while(isJSONDigit(*data))
		{
			multiplier*=0.1;
			ret+=getJSONDigit(*data)*multiplier;
			data++;
		}
	}

	//4) Get exp
	if(*data=='e' || *data=='E')
	{
		data++;
		float expsign=10;
		int exp=0;
		if(*data=='-')
		{
			expsign=0.1;
			data++;
		}
		else if(*data=='+')
			data++;

		if(!isJSONDigit(*data))
			throw DeserializationException("Invalid format for number");
		while(isJSONDigit(*data))
		{
			exp*=10;
			exp+=getJSONDigit(*data);
			data++;
		}
		for(int i=0;i<exp;i++)
			ret*=expsign;
	}
	ret*=sign;
	return ret;
}
