#include <QQmlEngine>
#include <QtQml>
#include "BigInteger.hh"
#include "bigintegerobject.h"
#include "BigIntegerUtils.hh"
#include "bigintegermath.h"

BigIntegerObject::BigIntegerObject(QObject *parent) : QObject(parent)
{

}

QString BigIntegerObject::add(const QString &a, const QString &b) const
{
    BigInteger v1,v2,r;

    v1 = stringToBigInteger(a.toStdString());
    v2 = stringToBigInteger(b.toStdString());
    r = v1 + v2;

    return QString::fromStdString(bigIntegerToString(r));
}

QString BigIntegerObject::minus(const QString &a, const QString &b) const
{
    BigInteger v1,v2,r;

    v1 = stringToBigInteger(a.toStdString());
    v2 = stringToBigInteger(b.toStdString());
    r = v1 - v2;

    return QString::fromStdString(bigIntegerToString(r));
}

QString BigIntegerObject::divide(const QString &a, const QString &b) const
{
    BigInteger v1,v2,r;

    v1 = stringToBigInteger(a.toStdString());
    v2 = stringToBigInteger(b.toStdString());
    r = v1 / v2;

    return QString::fromStdString(bigIntegerToString(r));
}

QString BigIntegerObject::abs(const QString &value) const
{
    if (isNegative(value)) {
        BigInteger v = stringToBigInteger(value.toStdString());
        v.flipSign();
        return QString::fromStdString(bigIntegerToString(v));
    } else {
        return  value;
    }
}

bool BigIntegerObject::isNegative(const QString &value) const
{
    return value.size() > 0 && value[0] == QChar('-');
}

int BigIntegerObject::compare(const QString &a, const QString &b) const
{
    BigInteger v1,v2;

    v1 = stringToBigInteger(a.toStdString());
    v2 = stringToBigInteger(b.toStdString());

    return v1.compareTo(v2);
}

bool BigIntegerObject::greater(const QString &a, const QString &b) const
{
    BigInteger v1,v2;

    v1 = stringToBigInteger(a.toStdString());
    v2 = stringToBigInteger(b.toStdString());

    return v1 > v2;
}

bool BigIntegerObject::greaterOrEquals(const QString &a, const QString &b) const
{
    BigInteger v1,v2;

    v1 = stringToBigInteger(a.toStdString());
    v2 = stringToBigInteger(b.toStdString());

    return v1 >= v2;
}

bool BigIntegerObject::lesser(const QString &a, const QString &b) const
{
    BigInteger v1,v2;

    v1 = stringToBigInteger(a.toStdString());
    v2 = stringToBigInteger(b.toStdString());

    return v1 < v2;
}

bool BigIntegerObject::lesserOrEquals(const QString &a, const QString &b) const
{
    BigInteger v1,v2;

    v1 = stringToBigInteger(a.toStdString());
    v2 = stringToBigInteger(b.toStdString());

    return v1 <= v2;
}

bool BigIntegerObject::equals(const QString &a, const QString &b) const
{
    return a == b;
}

QStringList BigIntegerObject::fraction(qreal input) const
{
    BigInteger numerator, denominator;
    QStringList res;

    BigIntegerMath::fraction(input,numerator, denominator);

    res << QString::fromStdString(bigIntegerToString(numerator))
        << QString::fromStdString(bigIntegerToString(denominator));

    return res;
}

QString BigIntegerObject::gcd(const QString &a, const QString &b) const
{
    BigInteger v1,v2;

    v1 = stringToBigInteger(a.toStdString());
    v2 = stringToBigInteger(b.toStdString());

    BigInteger result = BigIntegerMath::gcd(v1,v2);

    return QString::fromStdString(bigIntegerToString(result));
}

QString BigIntegerObject::pow(const QString &a, int b) const
{
    BigInteger v1;

    v1 = stringToBigInteger(a.toStdString());

    BigInteger result = BigIntegerMath::pow(v1,b);

    return QString::fromStdString(bigIntegerToString(result));

}

QString BigIntegerObject::multiply(const QString &a, const QString &b) const
{
    BigInteger v1,v2,r;

    v1 = stringToBigInteger(a.toStdString());
    v2 = stringToBigInteger(b.toStdString());
    r = v1 * v2;

    return QString::fromStdString(bigIntegerToString(r));
}

QString BigIntegerObject::multiply(const QString &a, const QStringList &fraction) const
{
    if (fraction.size() != 2) {
        qWarning() << "BigInteger::multiply(a, fraction) - Invalid fraction";
        return a;
    }

    BigInteger numerator,denominator,value;

    value = stringToBigInteger(a.toStdString());
    numerator = stringToBigInteger(fraction.at(0).toStdString());
    denominator = stringToBigInteger(fraction.at(1).toStdString());

    value *= numerator;
    value /= denominator;

    return QString::fromStdString(bigIntegerToString(value));
}

QString BigIntegerObject::multiply(const QString &a, qreal b) const
{
    BigInteger numerator, denominator,v;
    QString res;

    BigIntegerMath::fraction(b,numerator, denominator);

    v = stringToBigInteger(a.toStdString());

    v *= numerator;
    v /= denominator;

    res = QString::fromStdString(bigIntegerToString(v));

    return res;
}

static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    BigIntegerObject* object = new BigIntegerObject();
    return object;
}

class BigIntegerWrapperRegistrationHelper {
public:
    BigIntegerWrapperRegistrationHelper() {
        qmlRegisterSingletonType<BigIntegerObject>("BigInteger", 1, 0, "BigInteger", provider);
    }
};

static BigIntegerWrapperRegistrationHelper registerHelper;
