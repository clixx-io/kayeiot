#include <QString>
#include <QtTest>
#include <QCoreApplication>

class TestsTest : public QObject
{
    Q_OBJECT

public:
    TestsTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1_data();
    void testCase1();
};

TestsTest::TestsTest()
{
}

void TestsTest::initTestCase()
{
}

void TestsTest::cleanupTestCase()
{
}

void TestsTest::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void TestsTest::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

QTEST_MAIN(TestsTest)

#include "tst_teststest.moc"
