#include <map>
#include <QString>
#include <QtTest>
#include <QMap>

#include <qmarker.h>
#include <qglyphs.h>

class TestQtAddons : public QObject
{
    Q_OBJECT

public:
    TestQtAddons();

private Q_SLOTS:
    void testQMarker();
};

TestQtAddons::TestQtAddons()
{
}

void TestQtAddons::testQMarker()
{
    QtAddons::QMarker m1;

    QVERIFY(m1.getGlyph()==nullptr);

    QtAddons::QMarker m2(QtAddons::PlotGlyph_Cross,QPointF(10,10),QColor("red"),15);
    QVERIFY(m2.getGlyph()!=nullptr);
    QtAddons::ePlotGlyph g=m2.getGlyph()->GlyphType();
    QVERIFY(g==QtAddons::PlotGlyph_Cross);

    QtAddons::QMarker m3(m2);
    QVERIFY(m3.getGlyph()!=nullptr);

    QVERIFY(m3.getGlyph()->GlyphType()==m2.getGlyph()->GlyphType());

    m1=m3;
    QVERIFY(m1.getGlyph()!=nullptr);
    QVERIFY(m1.getGlyph()!=m3.getGlyph());

    QVERIFY(m1.getGlyph()->GlyphType()==m3.getGlyph()->GlyphType());

    QMap<int,QtAddons::QMarker> q;

    q.insert(1,m1);

    QVERIFY(q[1].getGlyph()->GlyphType()==m1.getGlyph()->GlyphType());

}


QTEST_APPLESS_MAIN(TestQtAddons)

#include "tst_qtaddons.moc"
