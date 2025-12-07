#include <QString>
#include <QtTest>
#include <QDebug>

#include <base/timage.h>
#include <base/tsubimage.h>
#include <iostream>


class SubImageTest : public QObject
{
    Q_OBJECT

public:
    SubImageTest();

private Q_SLOTS:
    void testInitialization();
    void testCopying();
    void testExtractInsert2D();
    void testEdgeCases2D();
    void testCornerCases2D();
    void testExtractInsert3D();
    void testStaticMembers();
};

SubImageTest::SubImageTest()
{
}

void SubImageTest::testInitialization()
{
    std::vector<size_t> start = {2,2};
    std::vector<size_t> length = {4,4};
    size_t margin = 1;

    kipl::base::TSubImage<float,2> subImg(start,length,margin);

    QCOMPARE(subImg.start(), start);
    QCOMPARE(subImg.length(), length);

    std::vector<size_t> estart = {1,1};
    std::vector<size_t> elength = {6,6};

    QCOMPARE(subImg.estart(), estart);
    QCOMPARE(subImg.elength(), elength);
}   

void SubImageTest::testCopying()
{
    std::vector<size_t> start = {2,2};
    std::vector<size_t> length = {4,4};
    size_t margin = 1;

    kipl::base::TSubImage<float,2> subImg1(start,length,margin);
    kipl::base::TSubImage<float,2> subImg2 = subImg1;

    QCOMPARE(subImg2.start(), start);
    QCOMPARE(subImg2.length(), length);

    std::vector<size_t> estart = {1,1};
    std::vector<size_t> elength = {6,6};

    QCOMPARE(subImg2.estart(), estart);
    QCOMPARE(subImg2.elength(), elength);

    kipl::base::TSubImage<float,2> subImg3;

    subImg3 = subImg1;

    QCOMPARE(subImg3.start(), start);
    QCOMPARE(subImg3.length(), length);
    QCOMPARE(subImg3.estart(), estart);
    QCOMPARE(subImg3.elength(), elength);

}   

void SubImageTest::testExtractInsert2D()
{
    kipl::base::TImage<float,2> img({10,10});
    for (size_t y=0; y<img.Size(1); y++) {
        for (size_t x=0; x<img.Size(0); x++) {
            img(x,y) = static_cast<float>(y*10 + x);
        }
    }

    std::vector<size_t> start = {2,2};
    std::vector<size_t> length = {4,5};
    size_t margin = 1;

    kipl::base::TSubImage<float,2> subImg(start,length,margin);

    kipl::base::TImage<float,2> extracted = subImg.extract(img);

    QCOMPARE(extracted.Size(0), length[0]+2*margin);
    QCOMPARE(extracted.Size(1), length[1]+2*margin);

    // Check center values
    for (size_t y=0; y<length[1]; y++) {
        for (size_t x=0; x<length[0]; x++) {
            float val = extracted(x+margin,y+margin);
            float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
            QCOMPARE(val, expected);
        }
    }

    // std::cout << img << std::endl;
    // std::cout << extracted << std::endl;

    // Now insert back into a new image
    kipl::base::TImage<float,2> destImg({10,10});
    subImg.insert(extracted, destImg);

    // std::cout << destImg << std::endl;

    // Check that the inserted values match the original image
    for (size_t y=0; y<length[1]; y++) {
        for (size_t x=0; x<length[0]; x++) {
            float val = destImg(x+start[0],y+start[1]);
            float expected = img(x+start[0],y+start[1]);
            QCOMPARE(val, expected);
        }
    }

    kipl::base::TImage<float,2> destImg2({10,10});
    subImg.insert(extracted, destImg2, true);

    // cout << destImg2 << std::endl;
    // Check that the inserted values match the original image including margin
    for (size_t y=0; y<length[1]+2*margin; y++) {
        for (size_t x=0; x<length[0]+2*margin; x++) {
            float val = destImg2(x+start[0]-margin,y+start[1]-margin);
            float expected = img(x+start[0]-margin,y+start[1]-margin);
            QCOMPARE(val, expected);    
        }
    }
}

void SubImageTest::testEdgeCases2D()
{
    kipl::base::TImage<float,2> img({10,10});
    for (size_t y=0; y<img.Size(1); y++) {
        for (size_t x=0; x<img.Size(0); x++) {
            img(x,y) = static_cast<float>(y*10 + x);
        }
    }

    // Left margin
    {
        std::vector<size_t> start = {0,2};
        std::vector<size_t> length = {3,3};
        size_t margin = 2;

        kipl::base::TSubImage<float,2> subImg(start,length,margin);

        kipl::base::TImage<float,2> extracted = subImg.extract(img);

        QCOMPARE(extracted.Size(0), length[0]+2*margin);
        QCOMPARE(extracted.Size(1), length[1]+2*margin);

        // std::cout << "Left Margin:" << std::endl;
        // std::cout << img << std::endl;
        // std::cout << extracted << std::endl;
        // Check center values
        for (size_t y=0; y<length[1]; y++) {
            for (size_t x=0; x<length[0]; x++) {
                float val = extracted(x+margin,y+margin);
                float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
                QCOMPARE(val, expected);
            }
        }
    }

    // Top margin
    {
        std::vector<size_t> start = {2,0};
        std::vector<size_t> length = {3,3};
        size_t margin = 2;

        kipl::base::TSubImage<float,2> subImg(start,length,margin);

        kipl::base::TImage<float,2> extracted = subImg.extract(img);

        QCOMPARE(extracted.Size(0), length[0]+2*margin);
        QCOMPARE(extracted.Size(1), length[1]+2*margin);

        // std::cout << "Top Margin:" << std::endl;
        // std::cout << img << std::endl;
        // std::cout << extracted << std::endl;
        // Check center values
        for (size_t y=0; y<length[1]; y++) {
            for (size_t x=0; x<length[0]; x++) {
                float val = extracted(x+margin,y+margin);
                float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
                QCOMPARE(val, expected);
            }
        }
    }

    // Right margin
    {
        std::vector<size_t> length = {3,3};
        std::vector<size_t> start = {img.Size(0)-length[0],2};
        size_t margin = 2;

        kipl::base::TSubImage<float,2> subImg(start,length,margin);

        kipl::base::TImage<float,2> extracted = subImg.extract(img);

        QCOMPARE(extracted.Size(0), length[0]+2*margin);
        QCOMPARE(extracted.Size(1), length[1]+2*margin);

        // std::cout << "Right Margin:" << std::endl;
        // std::cout << img << std::endl;
        // std::cout << extracted << std::endl;
        // Check center values
        for (size_t y=0; y<length[1]; y++) {
            for (size_t x=0; x<length[0]; x++) {
                float val = extracted(x+margin,y+margin);
                float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
                QCOMPARE(val, expected);
            }
        }
    }

    // Bottom margin
    {
        std::vector<size_t> length = {3,3};
        std::vector<size_t> start = {2,img.Size(1)-length[1]};
        size_t margin = 2;

        kipl::base::TSubImage<float,2> subImg(start,length,margin);

        kipl::base::TImage<float,2> extracted = subImg.extract(img);

        QCOMPARE(extracted.Size(0), length[0]+2*margin);
        QCOMPARE(extracted.Size(1), length[1]+2*margin);

        // std::cout << "Bottom Margin:" << std::endl;
        // std::cout << img << std::endl;
        // std::cout << extracted << std::endl;
        // Check center values
        for (size_t y=0; y<length[1]; y++) {
            for (size_t x=0; x<length[0]; x++) {
                float val = extracted(x+margin,y+margin);
                float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
                QCOMPARE(val, expected);
            }
        }
    }

    // Extreme right margin
    {
        std::vector<size_t> start = {img.Size(0)-1,2};
        std::vector<size_t> length = {3,3};
        size_t margin = 2;

        kipl::base::TSubImage<float,2> subImg(start,length,margin);

        kipl::base::TImage<float,2> extracted = subImg.extract(img);


        QCOMPARE(extracted.Size(0), length[0]+2*margin);
        QCOMPARE(extracted.Size(1), length[1]+2*margin);

        // std::cout << "Extreme Right Margin:" << std::endl;
        // std::cout << img << std::endl;
        // std::cout << extracted << std::endl;
        // Check center values
        // for (size_t y=0; y<length[1]; y++) {
        //     for (size_t x=0; x<length[0]; x++) {
        //         float val = extracted(x+margin,y+margin);
        //         float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
        //         QCOMPARE(val, expected);
        //     }
        // }
    }

    // Extreme bottom margin
    {
        std::vector<size_t> start = {2,img.Size(1)-1};
        std::vector<size_t> length = {3,3};
        size_t margin = 2;

        kipl::base::TSubImage<float,2> subImg(start,length,margin);

        kipl::base::TImage<float,2> extracted = subImg.extract(img);

        QCOMPARE(extracted.Size(0), length[0]+2*margin);
        QCOMPARE(extracted.Size(1), length[1]+2*margin);

        // std::cout << "Extreme Bottom Margin:" << std::endl;
        // std::cout << img << std::endl;
        // std::cout << extracted << std::endl;
        // Check center values
        // for (size_t y=0; y<length[1]; y++) {
        //     for (size_t x=0; x<length[0]; x++) {
        //         float val = extracted(x+margin,y+margin);
        //         float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
        //         QCOMPARE(val, expected);
        //     }
        // }
    }
}

void SubImageTest::testCornerCases2D()
{
    kipl::base::TImage<float,2> img({10,10});
    for (size_t y=0; y<img.Size(1); y++) {
        for (size_t x=0; x<img.Size(0); x++) {
            img(x,y) = static_cast<float>(y*10 + x);
        }
    }

    // Top-left corner
    {
        std::vector<size_t> start = {0,0};
        std::vector<size_t> length = {3,3};
        size_t margin = 2;

        kipl::base::TSubImage<float,2> subImg(start,length,margin);

        kipl::base::TImage<float,2> extracted = subImg.extract(img);

        QCOMPARE(extracted.Size(0), length[0]+2*margin);
        QCOMPARE(extracted.Size(1), length[1]+2*margin);

        std::cout << "Top-Left Corner:" << std::endl;
        std::cout << img << std::endl;
        std::cout << extracted << std::endl;
        // Check center values
        for (size_t y=0; y<length[1]; y++) {
            for (size_t x=0; x<length[0]; x++) {
                float val = extracted(x+margin,y+margin);
                float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
                QCOMPARE(val, expected);
            }
        }
    }

    // Top-right corner
    {
        std::vector<size_t> length = {3,3};
        std::vector<size_t> start = {img.Size(0)-length[0],0};

        size_t margin = 2;

        kipl::base::TSubImage<float,2> subImg(start,length,margin);

        kipl::base::TImage<float,2> extracted = subImg.extract(img);

        QCOMPARE(extracted.Size(0), length[0]+2*margin);
        QCOMPARE(extracted.Size(1), length[1]+2*margin);

        std::cout << "Top-Right Corner:" << std::endl;
        std::cout << img << std::endl;
        std::cout << extracted << std::endl;
        // Check center values
        for (size_t y=0; y<length[1]; y++) {
            for (size_t x=0; x<length[0]; x++) {
                float val = extracted(x+margin,y+margin);
                float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
                QCOMPARE(val, expected);
            }
        }
    }

    // Bottom-left corner
    {
        std::vector<size_t> length = {3,3};
        std::vector<size_t> start = {0,img.Size(1)-length[1]};
        size_t margin = 2;

        kipl::base::TSubImage<float,2> subImg(start,length,margin);

        kipl::base::TImage<float,2> extracted = subImg.extract(img);

        QCOMPARE(extracted.Size(0), length[0]+2*margin);
        QCOMPARE(extracted.Size(1), length[1]+2*margin);

        std::cout << "Bottom-Left Corner:" << std::endl;
        std::cout << img << std::endl;
        std::cout << extracted << std::endl;
        // Check center values
        for (size_t y=0; y<length[1]; y++) {
            for (size_t x=0; x<length[0]; x++) {
                float val = extracted(x+margin,y+margin);
                float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
                QCOMPARE(val, expected);
            }
        }
    }

    // Bottom-right corner
    {
        std::vector<size_t> length = {3,3};
        std::vector<size_t> start = {img.Size(0)-length[0],img.Size(1)-length[1]};
        size_t margin = 2;

        kipl::base::TSubImage<float,2> subImg(start,length,margin);

        kipl::base::TImage<float,2> extracted = subImg.extract(img);

        QCOMPARE(extracted.Size(0), length[0]+2*margin);
        QCOMPARE(extracted.Size(1), length[1]+2*margin);

        std::cout << "Bottom-Right Corner:" << std::endl;
        std::cout << img << std::endl;
        std::cout << extracted << std::endl;
        // Check center values
        for (size_t y=0; y<length[1]; y++) {
            for (size_t x=0; x<length[0]; x++) {
                float val = extracted(x+margin,y+margin);
                float expected = static_cast<float>((y+start[1])*10 + (x+start[0]));
                QCOMPARE(val, expected);
            }
        }
    }
}

void SubImageTest::testExtractInsert3D()
{
    kipl::base::TImage<float,3> img({10,10,10});
    for (size_t z=0; z<img.Size(2); z++) {
        for (size_t y=0; y<img.Size(1); y++) {
            for (size_t x=0; x<img.Size(0); x++) {
                img(x,y,z) = static_cast<float>(z*100 + y*10 + x);
            }
        }
    }

    std::vector<size_t> start = {2,2,2};
    std::vector<size_t> length = {4,5,3};
    size_t margin = 1;

    kipl::base::TSubImage<float,3> subImg(start,length,margin);

    kipl::base::TImage<float,3> extracted = subImg.extract(img);

    QCOMPARE(extracted.Size(0), length[0]+2*margin);
    QCOMPARE(extracted.Size(1), length[1]+2*margin);
    QCOMPARE(extracted.Size(2), length[2]+2*margin);

    // Check center values
    for (size_t z=0; z<length[2]; z++) {
        for (size_t y=0; y<length[1]; y++) {
            for (size_t x=0; x<length[0]; x++) {
                float val = extracted(x+margin,y+margin,z+margin);
                float expected = static_cast<float>((z+start[2])*100 + (y+start[1])*10 + (x+start[0]));
                QCOMPARE(val, expected);
            }
        }
    }

    // Now insert back into a new image
    kipl::base::TImage<float,3> destImg({10,10,10});
    subImg.insert(extracted, destImg);

    // Check that the inserted values match the original image
    for (size_t z=0; z<length[2]; z++) {
        for (size_t y=0; y<length[1]; y++) {
            for (size_t x=0; x<length[0]; x++) {
                float val = destImg(x+start[0],y+start[1],z+start[2]);
                float expected = img(x+start[0],y+start[1],z+start[2]);
                QCOMPARE(val, expected);
            }
        }
    }
}

void SubImageTest::testStaticMembers()
{

}

QTEST_APPLESS_MAIN(SubImageTest)
#include "tst_subimage.moc"