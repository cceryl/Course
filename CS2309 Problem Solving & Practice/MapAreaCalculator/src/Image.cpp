#include "Image.h"

#include <algorithm>
#include <chrono>
#include <unordered_map>

#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Image_Surface.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_PNM_Image.H>
#include <FL/fl_draw.H>

#include "Platform.h"

namespace MapAreaCalculator
{
    Image::Image(int x, int y, int width, int height) : Fl_Button(x, y, width, height)
    {
        this->align(FL_ALIGN_CENTER);
        this->box(FL_NO_BOX);
        this->labelfont(FL_TIMES_BOLD);
        this->labelsize(22);

        mScaler = 1.0;
        mShiftX = 0.0;
        mShiftY = 0.0;
    }

    void Image::draw()
    {
        if (mImage == nullptr)
        {
            this->label("No Available Image");
            this->labelfont(FL_TIMES_BOLD);
            this->labelsize(22);
        }
        else if (mImage->fail())
        {
            this->label("Failed to Load Image");
            this->labelfont(FL_TIMES_BOLD);
            this->labelsize(22);
        }
        else
        {
            this->label(nullptr);
            this->constrainScale();
            mImage->scale(mImage->data_w() * mScaler, mImage->data_h() * mScaler, true, true);
            mImage->draw(0, 0, mImage->w(), mImage->h(), -mShiftX, -mShiftY);
            if (mOverlayImage != nullptr)
            {
                mOverlayImage->scale(mOverlayImage->data_w() * mScaler, mOverlayImage->data_h() * mScaler, true, true);
                mOverlayImage->draw(0, 0, mOverlayImage->w(), mOverlayImage->h(), -mShiftX, -mShiftY);
            }
        }
        Fl_Group *parent = this->parent();
        this->resize(0, 0, parent->w(), parent->h());
        Fl_Button::draw();
    }

    int Image::handle(int event)
    {
        if (Fl::event_inside(this) && mImage != nullptr && !mImage->fail())
        {
            if (Fl::event_button() == FL_LEFT_MOUSE && event == FL_PUSH)
            {
                mPolygon.push((Fl::event_x() - mShiftX) / mScaler, (Fl::event_y() - mShiftY) / mScaler);
                this->genOverlayImage();
            }
            else if (Fl::event_button() == FL_MIDDLE_MOUSE && event == FL_DRAG)
                this->calcShift();
            else if (Fl::event_button() == FL_RIGHT_MOUSE && event == FL_PUSH)
            {
                mPolygon.pop();
                this->genOverlayImage();
            }
            else if (event == FL_MOUSEWHEEL)
            {
                static bool imageWindowCall = false;
                imageWindowCall             = !imageWindowCall;
                if (imageWindowCall)
                    this->calcScale();
            }
            this->redraw();
        }
        return Fl_Button::handle(event);
    }

    double Image::getScaler() const { return mPolygon.getScaler(); }

    double Image::calcArea() const { return mPolygon.calcArea(); }

    void Image::loadImage(std::string_view imagePath)
    {
        this->redraw();
        this->redraw_label();

        if (mImagePath == imagePath)
            return;

        mImage.reset();
        mOverlayImage.reset();
        mOverlayData.clear();
        mPolygon.clear();
        mImagePath = imagePath.data();

        if (imagePath.empty())
            return;

        enum Extension : unsigned char { BMP, JPG, JPEG, PNG, PNM, PBM, PGM, PPM };
        static const std::unordered_map<std::string, Extension> extensionMap = {
            { ".bmp",  BMP},
            { ".jpg",  JPG},
            {".jpeg", JPEG},
            { ".png",  PNG},
            { ".pnm",  PNM},
            { ".pbm",  PBM},
            { ".pgm",  PGM},
            { ".ppm",  PPM}
        };

        std::string extension = imagePath.substr(imagePath.find_last_of('.')).data();
        Extension   type;
        if (extensionMap.contains(extension))
            type = extensionMap.at(extension);
        else
        {
            mImage = std::make_unique<Fl_RGB_Image>(nullptr, 0, 0);
            return;
        }

        switch (type)
        {
        case BMP:  mImage = std::make_unique<Fl_BMP_Image>(imagePath.data()); break;
        case JPG:
        case JPEG: mImage = std::make_unique<Fl_JPEG_Image>(imagePath.data()); break;
        case PNG:  mImage = std::make_unique<Fl_PNG_Image>(imagePath.data()); break;
        case PNM:
        case PBM:
        case PGM:
        case PPM:  mImage = std::make_unique<Fl_PNM_Image>(imagePath.data()); break;
        default:   mImage = std::make_unique<Fl_RGB_Image>(nullptr, 0, 0); break;
        }

        this->constrainScale();
    }

    void Image::importVertices(std::string_view verticesPath)
    {
        if (mImage == nullptr || mImage->fail())
        {
            errorMessage("No Available Image");
            return;
        }
        mPolygon.importVertices(verticesPath, mImage->data_w(), mImage->data_h());
        this->genOverlayImage();
        this->redraw();
    }

    void Image::exportResultImage(std::string_view resultImagePath)
    {
        if (mImage == nullptr || mImage->fail())
        {
            errorMessage("No Available Image");
            return;
        }

        Fl_Image_Surface resultImageSurface(mImage->data_w(), mImage->data_h());
        Fl_Image_Surface::push_current(&resultImageSurface);
        mImage->scale(mImage->data_w(), mImage->data_h(), true, true);
        mImage->draw(0, 0, mImage->data_w(), mImage->data_h());
        mImage->scale(mImage->data_w() * mScaler, mImage->data_h() * mScaler, true, true);
        if (mOverlayImage != nullptr)
        {
            mOverlayImage->scale(mOverlayImage->data_w(), mOverlayImage->data_h(), true, true);
            mOverlayImage->draw(0, 0, mImage->data_w(), mImage->data_h());
            mOverlayImage->scale(mOverlayImage->data_w() * mScaler, mOverlayImage->data_h() * mScaler, true, true);
        }
        Fl_Image_Surface::pop_current();

        std::unique_ptr<Fl_RGB_Image> resultImage;
        resultImage.reset(resultImageSurface.image());
        fl_write_png(resultImagePath.data(), resultImage.get());
    }

    void Image::exportVertices(std::string_view verticesPath)
    {
        if (mImage == nullptr || mImage->fail())
        {
            errorMessage("No Available Image");
            return;
        }

        mPolygon.exportVertices(verticesPath);
    }

    void Image::selectScale()
    {
        if (mImage == nullptr || mImage->fail())
        {
            errorMessage("No Available Image");
            return;
        }
        mPolygon.selectScale();
    }

    void Image::calcShift()
    {
        static int  lastX, lastY;
        static auto lastDragTime = std::chrono::high_resolution_clock::now() - std::chrono::milliseconds(1000);
        auto        currentTime  = std::chrono::high_resolution_clock::now();
        auto        deltaTime    = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastDragTime);
        if (deltaTime > std::chrono::milliseconds(100))
        {
            lastX = Fl::event_x();
            lastY = Fl::event_y();
        }
        mShiftX      += Fl::event_x() - lastX;
        mShiftY      += Fl::event_y() - lastY;
        lastX         = Fl::event_x();
        lastY         = Fl::event_y();
        lastDragTime  = currentTime;
        lastDragTime  = std::chrono::high_resolution_clock::now();
        this->constrainShift();
    }

    void Image::calcScale()
    {
        double cursorX    = Fl::event_x();
        double cursorY    = Fl::event_y();
        double imagePosX  = (cursorX - mShiftX) / mScaler;
        double imagePosY  = (cursorY - mShiftY) / mScaler;
        mScaler          *= (Fl::event_dy() < 0 ? 1.1 : 0.9);
        this->constrainScale();

        double newCursorX  = imagePosX * mScaler + mShiftX;
        double newCursorY  = imagePosY * mScaler + mShiftY;
        mShiftX           += cursorX - newCursorX;
        mShiftY           += cursorY - newCursorY;
        this->constrainShift();
    }

    void Image::constrainShift()
    {
        mShiftX = std::clamp(mShiftX, static_cast<double>(this->w()) - mImage->data_w() * mScaler, 0.0);
        mShiftY = std::clamp(mShiftY, static_cast<double>(this->h()) - mImage->data_h() * mScaler, 0.0);
    }

    void Image::constrainScale()
    {
        mScaler = std::min(mScaler, 5.0);
        mScaler = std::max(mScaler, static_cast<double>(this->w()) / mImage->data_w());
        mScaler = std::max(mScaler, static_cast<double>(this->h()) / mImage->data_h());
        this->constrainShift();
    }

    void Image::genOverlayImage()
    {
        Fl_Image_Surface overlayImageSurface(mImage->data_w(), mImage->data_h());
        Fl_Image_Surface::push_current(&overlayImageSurface);
        fl_color(fl_rgb_color(255, 255, 255));
        fl_rectf(0, 0, mImage->data_w(), mImage->data_h());
        mPolygon.draw();
        Fl_Image_Surface::pop_current();

        std::unique_ptr<Fl_RGB_Image> rawImage;
        rawImage.reset(overlayImageSurface.image());
        mOverlayData.resize(mImage->data_w() * mImage->data_h() * 4);
        const char    *rawIter  = *rawImage->data();
        unsigned char *dataIter = mOverlayData.data();
        for (std::size_t i = 0; i < mImage->data_w() * mImage->data_h(); ++i)
            if (*(rawIter) == -1 && *(rawIter + 1) == -1 && *(rawIter + 2) == -1)
            {
                *dataIter++  = 0x00;
                *dataIter++  = 0x00;
                *dataIter++  = 0x00;
                *dataIter++  = 0x00;
                rawIter     += 3;
            }
            else
            {
                *dataIter++ = *rawIter++;
                *dataIter++ = *rawIter++;
                *dataIter++ = *rawIter++;
                *dataIter++ = 0x7F;
            }

        mOverlayImage = std::make_unique<Fl_RGB_Image>(mOverlayData.data(), mImage->data_w(), mImage->data_h(), 4);
    }

} // namespace MapAreaCalculator