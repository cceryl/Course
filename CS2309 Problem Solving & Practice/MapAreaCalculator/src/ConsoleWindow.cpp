#include "Window.h"

#include <format>

#include "Platform.h"

namespace MapAreaCalculator
{
    ConsoleWindow::ConsoleWindow(int x, int y, int width, int height, std::string_view title) :
        Fl_Double_Window(x, y, width, height, title.data())
    {
        this->box(FL_THIN_DOWN_BOX);
        this->color(51);

        mImportImage    = false;
        mImportVertices = false;
        mExportImage    = false;
        mExportVertices = false;
        mSelectScale    = false;
        mEnter          = false;

        mInputValue = -1.0;

        this->begin();
        this->createWidgets();
        this->end();
    }

    void ConsoleWindow::linkImageWindow(std::shared_ptr<ImageWindow> imageWindow) { mImageWindow = imageWindow; }

    void ConsoleWindow::draw()
    {
        if (mImportImage)
        {
            mImportImage = false;
            mImageWindow.lock()->importImage();
        }
        if (mImportVertices)
        {
            mImportVertices = false;
            mImageWindow.lock()->importVertices();
        }
        if (mSelectScale)
        {
            mSelectScale = false;
            mImageWindow.lock()->selectScale();
        }
        if (mEnter)
        {
            mEnter = false;
            if (mImageWindow.lock()->getScaler() <= 0.0)
            {
                errorMessage("No map scale selected.");
                return;
            }
            if (mInput.empty())
            {
                errorMessage("No real distance input.");
                return;
            }

            try
            {
                std::size_t pos = 0;
                mInputValue     = std::stod(mInput, &pos);
                if (mInputValue <= 0.0)
                    throw std::invalid_argument("Input value must be positive");
                if (pos != mInput.size())
                    throw std::invalid_argument("Input value must be a number");
            }
            catch (const std::exception &exception)
            {
                errorMessage(std::format("Real distance input error: {}", exception.what()));
                mInputValue = 1.0;
            }
        }
        if (mExportImage)
        {
            mExportImage = false;
            mImageWindow.lock()->exportResultImage();
        }
        if (mExportVertices)
        {
            mExportVertices = false;
            mImageWindow.lock()->exportVertices();
        }

        double scalerValue    = mImageWindow.lock()->getScaler();
        double pixelAreaValue = mImageWindow.lock()->calcArea();
        double scale          = mInputValue / scalerValue;

        if (mInputValue > 0.0 && scalerValue > 0.0)
            mMapScale->updateMessage(std::format("Map Scale: {:.3f} m/pixel", mInputValue / scalerValue));
        else
            mMapScale->updateMessage("Map Scale: N/A");

        if (pixelAreaValue > 0.0)
            mPixelArea->updateMessage(std::format("Pixel Area: {:.1f} squared pixels", pixelAreaValue));
        else
            mPixelArea->updateMessage("Pixel Area: N/A");

        if (mInputValue > 0.0 && scalerValue > 0.0 && pixelAreaValue > 0.0)
            mRealArea->updateMessage(std::format("Real Area: {:.12g} squared meters", pixelAreaValue * scale * scale));
        else
            mRealArea->updateMessage("Real Area: N/A");

        Fl_Double_Window::draw();
    }

    int ConsoleWindow::handle(int event) { return Fl_Double_Window::handle(event); }

    void ConsoleWindow::createWidgets()
    {
        descriptions[0]       = std::make_unique<Message>(0.10, 0.05, 0.80, 0.05, "Import", this, true, 22);
        mImportImageButton    = std::make_unique<Button>(0.10, 0.10, 0.80, 0.05, "@fileopen    Import Image", this, mImportImage);
        mImportVerticesButton = std::make_unique<Button>(0.10, 0.16, 0.80, 0.05, "@fileopen    Import Vertices", this, mImportVertices);

        descriptions[1]    = std::make_unique<Message>(0.10, 0.25, 0.80, 0.05, "Map Scale", this, true, 22);
        mSelectScaleButton = std::make_unique<Button>(0.10, 0.30, 0.51, 0.05, "Select Scale", this, mSelectScale);
        mEnterButton       = std::make_unique<Button>(0.65, 0.30, 0.25, 0.05, "Enter", this, mEnter);
        descriptions[2]    = std::make_unique<Message>(0.10, 0.36, 0.30, 0.05, "Real Distance: ", this, false, 15);
        mInputBox          = std::make_unique<InputBox>(0.44, 0.365, 0.40, 0.04, this, "m", mInput);
        mMapScale          = std::make_unique<Message>(0.10, 0.40, 0.80, 0.05, "Map Scale: N/A", this, false, 15);

        descriptions[3] = std::make_unique<Message>(0.10, 0.50, 0.80, 0.05, "Result", this, true, 22);
        mPixelArea      = std::make_unique<Message>(0.10, 0.55, 0.80, 0.05, "Pixel Area: N/A", this, false, 15);
        mRealArea       = std::make_unique<Message>(0.10, 0.59, 0.80, 0.05, "Real Area: N/A", this, false, 15);

        descriptions[4]       = std::make_unique<Message>(0.10, 0.70, 0.80, 0.05, "Export", this, true, 22);
        mExportImageButton    = std::make_unique<Button>(0.10, 0.75, 0.80, 0.05, "@filesave    Export Result Image", this, mExportImage);
        mExportVerticesButton = std::make_unique<Button>(0.10, 0.81, 0.80, 0.05, "@filesave    Export Vertices", this, mExportVertices);
    }

} // namespace MapAreaCalculator