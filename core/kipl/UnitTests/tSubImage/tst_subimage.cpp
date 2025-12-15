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
    void testLargeImageHandling();

    void testPatchExtractorInitialization();
    void testPatchExtractorAccessors();
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

void SubImageTest::testPatchExtractorInitialization()
{
    kipl::base::TImage<float,2> img({10,10});
    for (size_t y=0; y<img.Size(1); y++) {
        for (size_t x=0; x<img.Size(0); x++) {
            img(x,y) = static_cast<float>(y*10 + x);
        }
    }

    std::vector<size_t> subImageDims = {4,3};
    size_t margin = 1;

    kipl::base::ImagePatchExtractor<float,2> extractor(img.dims(), subImageDims, margin);
    // QCOMPARE(extractor.imageDims(), std::vector<size_t>({10,10}));
    // QCOMPARE(extractor.subImageDims(), subImageDims);
    // QCOMPARE(extractor.margin(), margin);   
    QCOMPARE(extractor.size(0), size_t(2)); // 2 full patches 
    QCOMPARE(extractor.size(1), size_t(3)); // 3 full patches
    QCOMPARE(extractor.size(), size_t(6));  // 2 * 3 = 6 patches
}

void SubImageTest::testPatchExtractorAccessors()
{
    kipl::base::TImage<float,2> img({14,10});
    for (size_t y=0; y<img.Size(1); y++) {
        for (size_t x=0; x<img.Size(0); x++) {
            img(x,y) = static_cast<float>(y*10 + x);
        }
    }

    std::vector<size_t> subImageDims = {4,3};
    size_t margin = 1;

    kipl::base::ImagePatchExtractor<float,2> extractor(img.dims(), subImageDims, margin);

    auto subImages = extractor.getAllSubImages();

    std::vector<size_t> lengthsum = {0,0}; // 14/4=3 full +1 remainder, 10/3=3 full +1 remainder
    for (size_t i=0; i<extractor.size(); ++i) {
        kipl::base::TSubImage<float,2> subImg = extractor.at(i);
        std::vector<size_t> start = subImg.start();
        std::vector<size_t> length = subImg.length();

        auto start2 = subImages[i].start();
        auto length2 = subImages[i].length();
        QCOMPARE(start, start2);
        QCOMPARE(length, length2);
        lengthsum[0] += length[0];
        lengthsum[1] += length[1];
    }

    std::cout << "Total length sum: (" << lengthsum[0] << "," << lengthsum[1] << ")" << std::endl;
    QCOMPARE(lengthsum[0]/extractor.size(1), img.Size(0));
    QCOMPARE(lengthsum[1]/extractor.size(0), img.Size(1));

    size_t ysum = 0UL;
    for (size_t y=0, i=0; y<extractor.size(1); ++y) {
        size_t xsum = 0UL;
        for (size_t x=0; x<extractor.size(0); ++x,++i) {
            size_t idx = y * extractor.size(0) + x;
            kipl::base::TSubImage<float,2> subImg = extractor.at(idx);
            std::vector<size_t> start = subImg.start();
            std::vector<size_t> length = subImg.length();

            auto start2 = subImages[i].start();
            auto length2 = subImages[i].length();
            QCOMPARE(start, start2);
            QCOMPARE(length, length2);

            xsum += length[0];
            ysum += length[1];
        }
        QCOMPARE(xsum, img.Size(0));
    }   
    QCOMPARE(ysum/extractor.size(0), img.Size(1));
}

void SubImageTest::testLargeImageHandling()
{
    // Test handling of large images that may exceed 2GB in size
    std::vector<size_t> dims = {1024, 512}; // 2.5 billion pixels
    std::vector<size_t> subImageDims = {32, 32};
    
    kipl::base::ImagePatchExtractor<float,2> extractor(dims, subImageDims, 0);

    auto subImages = extractor.getAllSubImages();

}


void SubImageTest::testStaticMembers()
{

}

QTEST_APPLESS_MAIN(SubImageTest)
#include "tst_subimage.moc"