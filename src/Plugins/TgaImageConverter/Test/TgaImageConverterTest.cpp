/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <tuple>
#include <TestSuite/Tester.h>
#include <Utility/Directory.h>
#include <Image.h>
#include <Trade/ImageData.h>
#include <TgaImageConverter/TgaImageConverter.h>
#include <TgaImporter/TgaImporter.h>

#include "configure.h"

using Corrade::Utility::Directory;

namespace Magnum { namespace Trade { namespace TgaImageConverter { namespace Test {

class TgaImageConverterTest: public Corrade::TestSuite::Tester {
    public:
        explicit TgaImageConverterTest();

        void wrongFormat();
        void wrongType();

        void data();
        void file();
};

namespace {
    const Image2D original({2, 3}, Image2D::Format::BGR, Image2D::Type::UnsignedByte, new char[18] {
        1, 2, 3, 2, 3, 4,
        3, 4, 5, 4, 5, 6,
        5, 6, 7, 6, 7, 8
    });
}

TgaImageConverterTest::TgaImageConverterTest() {
    addTests({&TgaImageConverterTest::wrongFormat,
              &TgaImageConverterTest::wrongType,

              &TgaImageConverterTest::data,
              &TgaImageConverterTest::file});
}

void TgaImageConverterTest::wrongFormat() {
    Image2D image({}, Image2D::Format::RG, Image2D::Type::UnsignedByte, nullptr);

    std::ostringstream out;
    Error::setOutput(&out);

    const unsigned char* data;
    std::tie(data, std::ignore) = TgaImageConverter().convertToData(&image);
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(out.str(), "Trade::TgaImageConverter::TgaImageConverter::convertToData(): unsupported image format AbstractImage::Format::RG\n");
}

void TgaImageConverterTest::wrongType() {
    Image2D image({}, Image2D::Format::Red, Image2D::Type::Float, nullptr);

    std::ostringstream out;
    Error::setOutput(&out);

    const unsigned char* data;
    std::tie(data, std::ignore) = TgaImageConverter().convertToData(&image);
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(out.str(), "Trade::TgaImageConverter::TgaImageConverter::convertToData(): unsupported image type AbstractImage::Type::Float\n");
}

void TgaImageConverterTest::data() {
    const unsigned char* data;
    std::size_t size;
    std::tie(data, size) = TgaImageConverter().convertToData(&original);

    TgaImporter::TgaImporter importer;
    CORRADE_VERIFY(importer.openData(data, size));
    Trade::ImageData2D* converted = importer.image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), Trade::ImageData2D::Format::BGR);
    CORRADE_COMPARE(converted->type(), Trade::ImageData2D::Type::UnsignedByte);
    CORRADE_COMPARE(std::string(reinterpret_cast<const char*>(converted->data()), 2*3*3),
                    std::string(reinterpret_cast<const char*>(original.data()), 2*3*3));
}

void TgaImageConverterTest::file() {
    const std::string filename = Directory::join(TGAIMAGECONVERTER_TEST_DIR, "file.tga");
    Directory::rm(filename);
    CORRADE_VERIFY(TgaImageConverter().convertToFile(&original, filename));

    TgaImporter::TgaImporter importer;
    CORRADE_VERIFY(importer.openFile(filename));
    Trade::ImageData2D* converted = importer.image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), Trade::ImageData2D::Format::BGR);
    CORRADE_COMPARE(converted->type(), Trade::ImageData2D::Type::UnsignedByte);
    CORRADE_COMPARE(std::string(reinterpret_cast<const char*>(converted->data()), 2*3*3),
                    std::string(reinterpret_cast<const char*>(original.data()), 2*3*3));

    Directory::rm(filename);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::TgaImageConverter::Test::TgaImageConverterTest)
