#include <QString>
#include <QtTest>

class FritzingpartTest : public QObject
{
    Q_OBJECT

public:
    FritzingpartTest();

private Q_SLOTS:
    void testCase1_data();
    void testCase1();
};

FritzingpartTest::FritzingpartTest()
{
}

void FritzingpartTest::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void FritzingpartTest::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(FritzingpartTest)

#include "tst_fritzingparttest.moc"
