#pragma once

#include <memory>
#include <vector>

#include <FL/Fl_Button.H>
#include <FL/Fl_RGB_Image.H>

#include "Polygon.h"

namespace MapAreaCalculator
{
    class Image : public Fl_Button
    {
    public:
        explicit Image(int x, int y, int width, int height);
        virtual ~Image() = default;

        void draw() override;
        int  handle(int event) override;

        double getScaler() const;
        double calcArea() const;

        void loadImage(std::string_view imagePath);
        void importVertices(std::string_view verticesPath);
        void exportResultImage(std::string_view resultImagePath);
        void exportVertices(std::string_view verticesPath);
        void selectScale();

    private:
        std::string                   mImagePath;
        std::unique_ptr<Fl_RGB_Image> mImage;
        std::unique_ptr<Fl_RGB_Image> mOverlayImage;
        std::vector<unsigned char>    mOverlayData;

        double mScaler;
        double mShiftX, mShiftY;

        Polygon mPolygon;

        void calcShift();
        void calcScale();
        void constrainShift();
        void constrainScale();

        void genOverlayImage();
    };

} // namespace MapAreaCalculator