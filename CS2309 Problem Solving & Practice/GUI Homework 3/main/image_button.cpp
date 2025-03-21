#include "image_button.h"

#include <format>
#include <optional>
#include <random>

#include <windows.h>

#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_PNM_Image.H>
#include <FL/fl_draw.H>

static std::optional<std::string> importImage()
{
    static bool called = false;
    if (called)
        return std::nullopt;
    called = true;

    OPENFILENAME ofn;
    char         file[MAX_PATH] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = NULL;
    ofn.lpstrFilter = "Picture\0*.bmp;*.jpg;*.jpeg;*.png;*.pnm;*.pbm;*.pgm;*.ppm\0";
    ofn.lpstrFile   = file;
    ofn.nMaxFile    = sizeof(file);
    ofn.Flags       = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    GetOpenFileName(&ofn);

    called = false;
    return file;
}

static Fl_RGB_Image *loadImage(std::string_view imagePath)
{
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
    if (auto result = extensionMap.find(extension); result != extensionMap.cend())
        type = result->second;
    else
        return new Fl_RGB_Image(0, 0, 0);
    switch (type)
    {
    case BMP:  return new Fl_BMP_Image(imagePath.data());
    case JPG:
    case JPEG: return new Fl_JPEG_Image(imagePath.data());
    case PNG:  return new Fl_PNG_Image(imagePath.data());
    case PNM:
    case PBM:
    case PGM:
    case PPM:  return new Fl_PNM_Image(imagePath.data());
    default:   return new Fl_RGB_Image(0, 0, 0);
    }
}

ImageButtonWindow::ImageButtonWindow(int width, int height, std::string_view name) : Fl_Double_Window(width, height, name.data())
{
    this->border(true);
    this->box(FL_NO_BOX);
    this->clip_children(true);
    this->color(FL_BLACK);
    this->resizable(this);
    this->size_range(800, 600);
    this->begin();

    mGraphWindow = new GraphWindow(this->w() * 0.25, 0, this->w() * 0.75, this->h(), this);
    this->setupGraph();

    mConsoleWindow = new ConsoleWindow(0, 0, this->w() * 0.25, this->h(), this);
    this->setupConsole();

    this->end();
}

void ImageButtonWindow::setupConsole()
{
    mConsoleWindow->begin();

    new Button(0.1, 0.3, 0.8, 0.1, mConsoleWindow, "Import Image", &mConsoleWindow->mImportImage);

    (new Slider(0.1, 0.6, 0.8, 0.1, mConsoleWindow, "Scaler"))->setValue(10, 100, 1, 30, mGraphWindow->mImageButton->getScaler());

    mConsoleWindow->end();
}

void ImageButtonWindow::setupGraph()
{
    mGraphWindow->begin();

    mGraphWindow->mImageButton = new ImageButton(0.35, 0.35, 0.3, 0.3, mGraphWindow, "Image Button", &mGraphWindow->mRandom);

    mGraphWindow->end();
}

ImageButtonWindow::ConsoleWindow::ConsoleWindow(int x, int y, int width, int height, ImageButtonWindow *mainWindow) :
    Fl_Double_Window(x, y, width, height), mMainWindow(mainWindow)
{
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_GRAY);

    mImportImage = false;
    mImagePath   = "";
}

void ImageButtonWindow::ConsoleWindow::draw()
{
    if (mImportImage)
    {
        mImportImage   = false;
        auto imagePath = importImage();
        mImagePath     = imagePath.value_or("");
        mMainWindow->mGraphWindow->mImageButton->redraw();
    }
    Fl_Double_Window::draw();
}

ImageButtonWindow::GraphWindow::GraphWindow(int x, int y, int width, int height, ImageButtonWindow *mainWindow) :
    Fl_Double_Window(x, y, width, height), mMainWindow(mainWindow)
{
    this->box(FL_THIN_DOWN_BOX);
    this->color(FL_BLACK);

    mRandom = false;
}

void ImageButtonWindow::GraphWindow::draw() { Fl_Double_Window::draw(); }

ImageButtonWindow::Button::Button(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label,
                                  bool *data) :
    Fl_Button(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_GTK_DOWN_BOX);
    this->labelfont(FL_TIMES);
    this->align(FL_ALIGN_CENTER);

    mX      = x;
    mY      = y;
    mWidth  = width;
    mHeight = height;

    this->user_data(data);
    this->callback(
        [](Fl_Widget *button, void *data)
        {
            *reinterpret_cast<bool *>(data) = true;
            dynamic_cast<Fl_Button *>(button)->parent()->redraw();
        });
}

void ImageButtonWindow::Button::draw()
{
    fl_line_style(FL_SOLID, 1);
    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    Fl_Button::draw();
}

ImageButtonWindow::ImageButton::ImageButton(double x, double y, double width, double height, GraphWindow *window, std::string_view label,
                                            bool *data) :
    Fl_Button(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_THIN_DOWN_BOX);
    this->label("No image");
    this->labelfont(FL_TIMES);
    this->align(FL_ALIGN_CENTER);

    mWindow = window;
    mX      = x;
    mY      = y;
    mWidth  = width;
    mHeight = height;

    mImagePath = "";
    mImage     = nullptr;
    mScaler    = 30;

    this->user_data(data);
    this->callback(
        [](Fl_Widget *button, void *data)
        {
            *reinterpret_cast<bool *>(data) = true;
            dynamic_cast<Fl_Button *>(button)->parent()->redraw();
        });
}

void ImageButtonWindow::ImageButton::draw()
{
    if (mImagePath != mWindow->mMainWindow->mConsoleWindow->mImagePath)
    {
        mImagePath = mWindow->mMainWindow->mConsoleWindow->mImagePath;
        if (!mImagePath.empty())
            mImage = loadImage(mImagePath);
    }
    if (mImagePath.empty())
    {
        this->label("No image");
        this->image(nullptr);
        this->bind_image(nullptr);
    }
    else if (mImage->fail())
    {
        this->label("Failed to load image");
        this->image(nullptr);
        this->bind_image(nullptr);
    }
    else
    {
        this->label(nullptr);
        this->image(mImage);
        this->bind_image(mImage);
    }

    mWidth  = mScaler / 100.0;
    mHeight = mScaler / 100.0;

    if (mWindow->mRandom)
    {
        mWindow->mRandom = false;
        static std::random_device          seeder;
        static std::mt19937                generator(seeder());
        std::uniform_int_distribution<int> distributorX(1, 1000);
        std::uniform_int_distribution<int> distributorY(1, 1000);
        mX = distributorX(generator) / 1000.0 * (1.0 - mWidth);
        mY = distributorY(generator) / 1000.0 * (1.0 - mHeight);
    }

    mX = std::min(mX, 1.0 - mWidth);
    mY = std::min(mY, 1.0 - mHeight);

    fl_line_style(FL_SOLID, 1);
    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    if (this->image() != nullptr)
        this->image()->scale(this->w(), this->h(), false, true);
    Fl_Button::draw();
}

ImageButtonWindow::Slider::Slider(double x, double y, double width, double height, ConsoleWindow *window, std::string_view label) :
    Fl_Hor_Slider(x * window->w(), y * window->h(), width * window->w(), height * window->h(), label.data())
{
    this->box(FL_GTK_DOWN_BOX);
    this->labelfont(FL_TIMES);

    mName   = label.data();
    mX      = x;
    mY      = y;
    mWidth  = width;
    mHeight = height;
}

void ImageButtonWindow::Slider::draw()
{
    static std::string label;

    this->resize(mX * this->parent()->w(), mY * this->parent()->h(), mWidth * this->parent()->w(), mHeight * this->parent()->h());
    label = std::format("{} = {}%", mName, this->value());
    this->label(label.c_str());

    Fl_Hor_Slider::draw();
}

void ImageButtonWindow::Slider::setValue(int min, int max, int step, int init, int *value)
{
    this->range(min, max);
    this->step(step);
    this->value(init);

    this->user_data(value);
    this->callback(
        [](Fl_Widget *slider, void *value)
        {
            dynamic_cast<Fl_Slider *>(slider)->redraw();
            dynamic_cast<Fl_Slider *>(slider)->redraw_label();
            *reinterpret_cast<int *>(value) = (int)dynamic_cast<Fl_Slider *>(slider)->value();
            slider->redraw();
            auto graphWindow = dynamic_cast<ImageButtonWindow *>(slider->parent()->parent())->mGraphWindow;
            graphWindow->redraw();
            graphWindow->mImageButton->redraw();
        });
}