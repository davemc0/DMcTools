//////////////////////////////////////////////////////////////////////
// auint.h - Arbitrary precision unsigned integer.
//
// Copyright David K. McAllister, Feb. 2000.

#ifndef _auint_h
#define _auint_h

#include "toolconfig.h"

#include <stdio.h>

#include <vector>
using namespace std;


class auint
{
  vector<unsigned char> val;

public:
  inline auint()
    {
      val.resize(1);
      val[0] = 0;
    }

  inline auint(int x)
    {
      val.clear();

      char num[100];
      sprintf(num, "%d", x);
      int len = strlen(num);
      cerr << "x"<<num << " " << len << endl;

      for(int i=0; i<len; i++)
	val.push_back(num[i] - '0');
    }

  inline auint &operator+=(const auint &b)
    {
	  unsigned char carry = 0;
	  for(int i=b.val.size()-1,j=val.size()-1; i>=0 || j>=0; i--,j--)
	    {
	      unsigned char av = (j>=0)?val[j]:0;
	      unsigned char bv = (i>=0)?val[i]:0;
	      unsigned char mul = av + bv + carry;
	      unsigned char dig = mul % 10;
	      if(j>=0)
		val[j] = dig;
	      else
		val.insert(val.begin(), dig);

	      carry = mul / 10;
	    }
	  if(carry>0)
	    val.insert(val.begin(), carry);

	  return *this;
    }

  inline auint operator*(const auint &b) const
    {
      auint r;
      r.val.clear();

      if(b.val.size() == 1)
	{
	  // Multiply this long number by a single-digit number.
	  unsigned char bv = b.val[0];
	  unsigned char carry = 0;
	  for(int i=val.size()-1; i>=0; i--)
	    {
	      unsigned char mul = val[i] * bv + carry;
	      unsigned char dig = mul % 10;
	      r.val.insert(r.val.begin(), dig);
	      carry = mul / 10;
	    }
	  if(carry > 0)
	    r.val.insert(r.val.begin(), carry);
	}
      else
	{
	  for(int cnt=0,i=b.val.size()-1; i>=0; i--,cnt++)
	    { 
	      cerr << "hi\n";
	      auint t = *this * b.val[i];
	      // Left shift this intermediate result.
	      for(int j=0; j<cnt; j++)
		t.val.push_back((unsigned char)0);
	      r += t;
	    }
	}
 
      return r;
    }

  friend ostream& operator<<(ostream& os, const auint& v);
};

inline ostream& operator<<(ostream& os, const auint& v)
{
  for(int i=0; i< v.val.size(); i++)
    os << char(v.val[i] + '0');
  return os;
}

#endif
