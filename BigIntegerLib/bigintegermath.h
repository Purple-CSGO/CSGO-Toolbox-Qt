#ifndef BIGINTEGERMATH_H
#define BIGINTEGERMATH_H

#include <QtCore>
#include "BigInteger.hh"

/// Math utility

class BigIntegerMath
{
public:

    static BigInteger pow(const BigInteger &base,int exponent);

    static BigInteger gcd(const BigInteger &a,const BigInteger &b);

    static void fraction(double decimal, BigInteger& numerator, BigInteger& denominator);
};

QDebug operator<< (QDebug d, const BigInteger &bigInteger);

#endif // BIGINTEGERMATH_H
