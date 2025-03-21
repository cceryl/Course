#pragma once

#include <array>
#include <memory>
#include <string>

#include <FL/Fl_Double_Window.H>

#include "Button.h"
#include "Image.h"
#include "InputBox.h"
#include "Message.h"

namespace MapAreaCalculator
{
    class Window;
    class MainWindow;
    class ConsoleWindow;
    class ImageWindow;

    class Window
    {
    public:
        explicit Window(int width, int height, std::string_view title);
        virtual ~Window()                 = default;
        Window(const Window &)            = delete;
        Window &operator=(const Window &) = delete;
        Window(Window &&)                 = delete;
        Window &operator=(Window &&)      = delete;

    private:
        std::unique_ptr<MainWindow> mWindow;
    };

    class MainWindow : public Fl_Double_Window
    {
    public:
        explicit MainWindow(int width, int height, std::string_view title);
        virtual ~MainWindow() = default;

        void draw() override;
        int  handle(int event) override;

    private:
        std::shared_ptr<ConsoleWindow> mConsoleWindow;
        std::shared_ptr<ImageWindow>   mImageWindow;
    };

    class ConsoleWindow : public Fl_Double_Window
    {
    public:
        explicit ConsoleWindow(int x, int y, int width, int height, std::string_view title);
        virtual ~ConsoleWindow() = default;
        void linkImageWindow(std::shared_ptr<ImageWindow> imageWindow);

        virtual void draw() override;
        virtual int  handle(int event) override;

    private:
        std::weak_ptr<ImageWindow> mImageWindow;

        std::array<std::unique_ptr<Message>, 5> descriptions;

        std::unique_ptr<Button> mImportImageButton;
        bool                    mImportImage;
        std::unique_ptr<Button> mImportVerticesButton;
        bool                    mImportVertices;

        std::unique_ptr<Button>   mSelectScaleButton;
        bool                      mSelectScale;
        std::unique_ptr<Button>   mEnterButton;
        bool                      mEnter;
        std::unique_ptr<InputBox> mInputBox;
        std::string               mInput;
        double                    mInputValue;
        std::unique_ptr<Message>  mMapScale;

        std::unique_ptr<Message> mPixelArea;
        std::unique_ptr<Message> mRealArea;

        std::unique_ptr<Button> mExportImageButton;
        bool                    mExportImage;
        std::unique_ptr<Button> mExportVerticesButton;
        bool                    mExportVertices;

        void createWidgets();
    };

    class ImageWindow : public Fl_Double_Window
    {
    public:
        explicit ImageWindow(int x, int y, int width, int height, std::string_view title);
        virtual ~ImageWindow() = default;
        void linkConsoleWindow(std::shared_ptr<ConsoleWindow> consoleWindow);

        virtual void draw() override;
        virtual int  handle(int event) override;

        void importImage();
        void importVertices();

        void exportResultImage();
        void exportVertices();

        void selectScale();

        double getScaler() const;
        double calcArea() const;

    private:
        std::weak_ptr<ConsoleWindow> mConsoleWindow;

        std::unique_ptr<Image> mImage;
    };

} // namespace MapAreaCalculator