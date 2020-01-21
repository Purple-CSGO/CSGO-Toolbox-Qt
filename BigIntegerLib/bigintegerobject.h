#pragma once
#include <QObject>
#include <QJSValue>

// QML Wrapper Object for BigInteger manipulation

class BigIntegerObject : public QObject
{
    Q_OBJECT
public:
    explicit BigIntegerObject(QObject *parent = 0);

signals:

public slots:

    QString multiply(const QString& a, const QString& b) const;

    QString multiply(const QString&a , const QStringList& fraction) const;

    QString multiply(const QString& a, qreal b) const;

    QString add(const QString& a, const QString& b) const;

    QString minus(const QString& a, const QString& b) const;

    QString divide(const QString& a, const QString& b) const;

    QString abs(const QString &value) const;

    bool isNegative(const QString &value) const;

    int compare(const QString& a, const QString& b) const;

    bool greater(const QString& a, const QString& b) const;

    bool greaterOrEquals(const QString& a, const QString& b) const;

    bool lesser(const QString& a, const QString& b) const;

    bool lesserOrEquals(const QString& a, const QString& b) const;

    bool equals(const QString& a, const QString& b) const;

    QStringList fraction(qreal input) const;

    QString gcd(const QString& a, const QString& b) const;

    QString pow(const QString& a, int b) const;

};
