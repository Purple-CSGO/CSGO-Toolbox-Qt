#include <QRegExp>
#include <QtCore>
#include <QString>
#include <QRegExp>
#include <QRegularExpressionMatch>
#include <QRegularExpression>
#include "bigintegermath.h"
#include "BigIntegerUtils.hh"

BigInteger BigIntegerMath::pow(const BigInteger &base, int exponent)
{
    BigInteger result;

    if (exponent < 1) {
        // It don't support floating point
        result = 1;
        return result;
    } else if (exponent == 1) {
        return base;
    }

    BigInteger x = base;
    BigInteger y = 1;
    int n = exponent;

    while (n > 1) {
        if ((n & 1) == 0) { // even number
            x = x * x;
            n = n / 2;
        } else {
            y = x * y;
            x = x * x;
            n = ( n - 1) / 2;
        }
    }

    result = x * y;
    return result;
}

static BigInteger _gcd(const BigInteger &a, const BigInteger &b) {
    if (a == 0) {
        return b;
    }

    if (b == 0) {
        return a;
    }

    if (a > b) {
        return _gcd(b , a % b);
    } else {
        return _gcd(a , b % a);
    }
}

BigInteger BigIntegerMath::gcd(const BigInteger &a, const BigInteger &b)
{
    BigInteger v1,v2;
    v1 = a;
    v2 = b;
    if (v1 < 0) {
        v1 = -v1;
    }
    if (v2 < 0) {
        v2 = -v2;
    }

    return _gcd(v1,v2);
}

void BigIntegerMath::fraction(double input, BigInteger &numerator, BigInteger &denominator)
{
    static QRegularExpression re("\\.[0-9]+$");

    QString str = QString::number(input);

    QRegularExpressionMatch matcher;

    str.indexOf(re, 0, &matcher);

    if (!matcher.hasMatch()) {
        numerator = qRound(input);
        denominator = 1;
    } else {

        QString decimal = str.mid(matcher.capturedStart() + 1, matcher.capturedEnd());
        QString integerPart = str.mid(0, matcher.capturedStart());
        denominator = pow(BigInteger(10), decimal.size());

        if (integerPart == "0") {
            integerPart = "";
        }

        for (int i = 0 ; i < decimal.size() ; i++) {
            integerPart += decimal.at(i);
        }

        numerator = stringToBigInteger(integerPart.toStdString());
    }

    BigInteger factor = gcd(numerator, denominator);
    if (factor != 1) {
        numerator /= factor;
        denominator /= factor;
    }

}


QDebug operator<<(QDebug d, const BigInteger &bigInteger)
{
    d << QString::fromStdString(bigIntegerToString(bigInteger));

    return d;
}
