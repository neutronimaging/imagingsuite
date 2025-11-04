#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QDebug>

#include <algorithms/sortalg.h>

#include <iostream>
#include <fstream>
#include <algorithm>


class AlgorithmsTest : public QObject
{
    Q_OBJECT
    
public:
    AlgorithmsTest();

private Q_SLOTS:
    void testArgSort();
    void testEmptyInput();
    void testSingleElement();
    void testAlreadySorted();
    void testReverseSorted();
    void testDuplicatesPreserveSortedValues();
    void testPointerIterator();

};

AlgorithmsTest::AlgorithmsTest()
{
}


void AlgorithmsTest::testArgSort()
{
    std::vector<int> data = { 40, 10, 5,20, 30 };
    std::vector<size_t> sorted_indices = kipl::algorithms::arg_sort(data.begin(), data.end());

    std::vector<size_t> expected_indices = { 2, 1, 3, 4, 0 }; // Indices that would sort the array

    QCOMPARE(sorted_indices.size(), expected_indices.size());
    for (size_t i = 0; i < sorted_indices.size(); ++i) {
        QCOMPARE(sorted_indices[i], expected_indices[i]);
    }
}

void AlgorithmsTest::testEmptyInput()
{
    std::vector<int> data;
    auto idx = kipl::algorithms::arg_sort(data.begin(), data.end());
    QCOMPARE(idx.size(), static_cast<size_t>(0));
}

void AlgorithmsTest::testSingleElement()
{
    std::vector<int> data = {42};
    auto idx = kipl::algorithms::arg_sort(data.begin(), data.end());
    std::vector<size_t> expected = {0};
    QCOMPARE(idx, expected);
}

void AlgorithmsTest::testAlreadySorted()
{
    std::vector<int> data = {1,2,3,4,5};
    auto idx = kipl::algorithms::arg_sort(data.begin(), data.end());
    std::vector<int> sorted_values(idx.size());
    for (size_t i = 0; i < idx.size(); ++i) sorted_values[i] = data[idx[i]];
    std::vector<int> expected = data;
    QCOMPARE(sorted_values, expected);
}

void AlgorithmsTest::testReverseSorted()
{
    std::vector<int> data = {5,4,3,2,1};
    auto idx = kipl::algorithms::arg_sort(data.begin(), data.end());
    std::vector<int> sorted_values(idx.size());
    for (size_t i = 0; i < idx.size(); ++i) sorted_values[i] = data[idx[i]];
    std::vector<int> expected = {1,2,3,4,5};
    QCOMPARE(sorted_values, expected);
}

void AlgorithmsTest::testDuplicatesPreserveSortedValues()
{
    std::vector<int> data = {40, 10, 5, 20, 30, 10, 5};
    auto idx = kipl::algorithms::arg_sort(data.begin(), data.end());

    // Verify size and that applying indices yields a non-decreasing sequence
    QVERIFY(idx.size() == data.size());
    std::vector<int> sorted_values(idx.size());
    for (size_t i = 0; i < idx.size(); ++i) sorted_values[i] = data[idx[i]];

    // Compare with std::sort result (value-wise correctness)
    std::vector<int> expected = data;
    std::sort(expected.begin(), expected.end());
    QCOMPARE(sorted_values, expected);
}

void AlgorithmsTest::testPointerIterator()
{
    double arr[] = {3.5, 1.2, 4.8, 2.0};
    auto idx = kipl::algorithms::arg_sort(std::begin(arr), std::end(arr));
    std::vector<double> sorted_values(idx.size());
    for (size_t i = 0; i < idx.size(); ++i) sorted_values[i] = arr[idx[i]];

    std::vector<double> expected(std::begin(arr), std::end(arr));
    std::sort(expected.begin(), expected.end());
    QCOMPARE(sorted_values, expected);
}

QTEST_APPLESS_MAIN(AlgorithmsTest)

#include "tst_algorithms.moc"
