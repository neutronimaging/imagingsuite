#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QDebug>

#include <algorithms/sortalg.h>
#include <algorithms/unique.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <set>
#include <string>


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

    void testUniqueEmpty();
    void testUniqueSingle();
    void testUniqueNoDuplicates();
    void testUniqueAllDuplicates();
    void testUniqueMixedUnsorted();
    void testUniqueNegativeValues();
    void testUniqueStrings();
    void testUniqueLargeRandom();

};

AlgorithmsTest::AlgorithmsTest()
{
}


void AlgorithmsTest::testArgSort()
{
    std::vector<int> data = { 40, 10, 5, 20, 30 };
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

void AlgorithmsTest::testUniqueEmpty() {
    std::vector<int> v;
    auto u = kipl::algorithms::unique_values(v);
    QCOMPARE(u.size(), size_t(0));
}

void AlgorithmsTest::testUniqueSingle() {
    std::vector<int> v{42};
    auto u = kipl::algorithms::unique_values(v);
    QCOMPARE(u.size(), size_t(1));
    QCOMPARE(u[0], 42);
}

void AlgorithmsTest::testUniqueNoDuplicates() {
    std::vector<int> v{5,1,7,3};
    auto u = kipl::algorithms::unique_values(v);
    std::vector<int> expected{1,3,5,7};
    QCOMPARE(u, expected);
}

void AlgorithmsTest::testUniqueAllDuplicates() {
    std::vector<int> v{9,9,9,9};
    auto u = kipl::algorithms::unique_values(v);
    QCOMPARE(u.size(), size_t(1));
    QCOMPARE(u[0], 9);
}

void AlgorithmsTest::testUniqueMixedUnsorted() {
    std::vector<int> v{4,2,4,1,2,3,3,4};
    auto u = kipl::algorithms::unique_values(v);
    std::vector<int> expected{1,2,3,4};
    QCOMPARE(u, expected);
}

void AlgorithmsTest::testUniqueNegativeValues() {
    std::vector<int> v{-1,-5,-1,0,-5,2};
    auto u = kipl::algorithms::unique_values(v);
    std::vector<int> expected{-5,-1,0,2};
    QCOMPARE(u, expected);
}

void AlgorithmsTest::testUniqueStrings() {
    std::vector<std::string> v{"b","a","b","c","a"};
    auto u = kipl::algorithms::unique_values(v);
    std::vector<std::string> expected{"a","b","c"};
    QCOMPARE(u, expected);
}

void AlgorithmsTest::testUniqueLargeRandom() {
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(0, 200);
    std::vector<int> v;
    v.reserve(2000);
    for (int i=0;i<2000;++i) v.push_back(dist(rng));
    auto u = kipl::algorithms::unique_values(v);

    // Check sorted
    QVERIFY(std::is_sorted(u.begin(), u.end()));

    // Check all unique
    for (size_t i=1;i<u.size();++i)
        QVERIFY(u[i-1] != u[i]);

    // Cross‑check with std::set
    std::set<int> s(v.begin(), v.end());
    QCOMPARE(u.size(), s.size());
    size_t idx = 0;
    for (int val : s) {
        QCOMPARE(u[idx], val);
        ++idx;
    }
}

QTEST_APPLESS_MAIN(AlgorithmsTest)

#include "tst_algorithms.moc"
