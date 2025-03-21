#include "Window.h"

#include <format>

namespace VisualQuadtree
{
    ConsoleWindow::ConsoleWindow(int x, int y, int width, int height, std::string_view title) :
        Fl_Double_Window(x, y, width, height, title.data())
    {
        this->box(FL_THIN_DOWN_BOX);
        this->color(51);

        mPausePressed        = false;
        mClearPressed        = false;
        mFillCirclePressed   = false;
        mShowQuadtreePressed = false;
        mStatisticsPressed   = false;
        mRestitution         = 100;
        mQuadtreeCircleLimit = 20;
        mTick                = 30;
        mCircleType          = CircleType::Custom;
        mRadius              = 5;
        mColor               = fl_rgb_color(0x00, 0x7F, 0xFF);

        this->begin();
        this->createWidgets();
        this->end();
    }

    void ConsoleWindow::linkGraphWindow(std::shared_ptr<GraphWindow> GraphWindow) { mGraphWindow = GraphWindow; }

    void ConsoleWindow::draw()
    {
        this->updateButtons();
        this->updateSliders();
        this->updateCircleType();
        this->updateCircle();
        Fl_Double_Window::draw();
    }

    int ConsoleWindow::handle(int event) { return Fl_Double_Window::handle(event); }

    std::pair<int, Fl_Color> ConsoleWindow::circleInfo() const { return {mRadius, mColor}; }

    void ConsoleWindow::createWidgets()
    {
        mButtonMessage      = std::make_unique<Message>(0.10f, 0.02f, 0.80f, 0.03f, "Visualization", this, true, 20);
        mPauseButton        = std::make_unique<Button>(0.10f, 0.05f, 0.38f, 0.04f, "Pause", this, mPausePressed);
        mClearButton        = std::make_unique<Button>(0.52f, 0.05f, 0.38f, 0.04f, "Clear", this, mClearPressed);
        mFillCircleButton   = std::make_unique<Button>(0.10f, 0.10f, 0.80f, 0.04f, "Outline Circle", this, mFillCirclePressed);
        mShowQuadtreeButton = std::make_unique<Button>(0.10f, 0.15f, 0.80f, 0.04f, "Hide Quadtree", this, mShowQuadtreePressed);
        mStatisticsButton   = std::make_unique<Button>(0.10f, 0.20f, 0.80f, 0.04f, "Hide Statistics", this, mStatisticsPressed);

        mSliderMessage     = std::make_unique<Message>(0.10f, 0.27f, 0.80f, 0.03f, "Physics", this, true, 20);
        mRestitutionSlider = std::make_unique<Slider>(0.10f, 0.30f, 0.80f, 0.04f, "Restitution", this);
        mRestitutionSlider->setValue(0, 100, 1, 100, mRestitution);
        mQuadtreeCircleLimitSlider = std::make_unique<Slider>(0.10f, 0.37f, 0.80f, 0.04f, "Quadtree Circle Limit", this);
        mQuadtreeCircleLimitSlider->setValue(5, 100, 1, 20, mQuadtreeCircleLimit);
        mTickSlider = std::make_unique<Slider>(0.10f, 0.44f, 0.80f, 0.04f, "Target FPS", this);
        mTickSlider->setValue(10, 60, 1, 30, mTick);

        mCircleTypeMessage = std::make_unique<Message>(0.10f, 0.51f, 0.80f, 0.03f, "New", this, true, 20);
        mCircleTypeGroup   = std::make_unique<Fl_Group>(0, 0, this->w(), this->h());
        mCircleTypeGroup->begin();
        mCustomCircleButton = std::make_unique<RadioButton>(0.10f, 0.55f, 0.80f, 0.04f, "Custom Circle", this);
        mRandomCircleButton = std::make_unique<RadioButton>(0.10f, 0.60f, 0.80f, 0.04f, "Random Circle", this);
        mAutoCircleButton   = std::make_unique<RadioButton>(0.10f, 0.65f, 0.80f, 0.04f, "Auto Circle", this);
        mCustomCircleButton->set();
        mCircleTypeGroup->end();

        mCustomMessage = std::make_unique<Message>(0.10f, 0.72f, 0.80f, 0.03f, "Custom", this, true, 20);
        mRadiusSlider  = std::make_unique<Slider>(0.10f, 0.75f, 0.80f, 0.04f, "Radius", this);
        mRadiusSlider->setValue(3, 20, 1, 5, mRadius);
        mColorChooser = std::make_unique<ColorChooser>(0.10f, 0.82f, 0.80f, 0.13f, "Color", this, mColor);
    }

    void ConsoleWindow::updateButtons()
    {
        if (mPausePressed)
        {
            static bool paused = false;
            mPausePressed      = false;
            paused             = !paused;
            if (paused)
                mPauseButton->setLabel("Resume");
            else
                mPauseButton->setLabel("Pause");
            mGraphWindow.lock()->setPause(paused);
        }
        if (mClearPressed)
        {
            mClearPressed = false;
            mGraphWindow.lock()->clear();
        }
        if (mFillCirclePressed)
        {
            static bool fill   = true;
            mFillCirclePressed = false;
            fill               = !fill;
            if (fill)
                mFillCircleButton->setLabel("Outline Circle");
            else
                mFillCircleButton->setLabel("Fill Circle");
            mGraphWindow.lock()->setFillCircle(fill);
        }
        if (mShowQuadtreePressed)
        {
            static bool show     = true;
            mShowQuadtreePressed = false;
            show                 = !show;
            if (show)
                mShowQuadtreeButton->setLabel("Hide Quadtree");
            else
                mShowQuadtreeButton->setLabel("Show Quadtree");
            mGraphWindow.lock()->setShowQuadtree(show);
        }
        if (mStatisticsPressed)
        {
            static bool show   = true;
            mStatisticsPressed = false;
            show               = !show;
            if (show)
                mStatisticsButton->setLabel("Hide Statistics");
            else
                mStatisticsButton->setLabel("Show Statistics");
            mGraphWindow.lock()->setShowFPS(show);
        }
    }

    void ConsoleWindow::updateSliders()
    {
        static int lastRestitution = mRestitution;
        if (mRestitution != lastRestitution)
        {
            lastRestitution = mRestitution;
            mGraphWindow.lock()->setRestitution(mRestitution);
        }
        static int lastQuadtreeCircleLimit = mQuadtreeCircleLimit;
        if (mQuadtreeCircleLimit != lastQuadtreeCircleLimit)
        {
            lastQuadtreeCircleLimit = mQuadtreeCircleLimit;
            mGraphWindow.lock()->setQuadtreeCircleLimit(mQuadtreeCircleLimit);
        }
        static int lastTick = mTick;
        if (mTick != lastTick)
        {
            lastTick = mTick;
            mGraphWindow.lock()->setTickTime(1.0 / mTick);
        }
    }

    void ConsoleWindow::updateCircleType()
    {
        if (mCustomCircleButton->value())
        {
            mCircleType = CircleType::Custom;
            mGraphWindow.lock()->customMode();
        }
        else if (mRandomCircleButton->value())
        {
            mCircleType = CircleType::Random;
            mGraphWindow.lock()->randomMode();
        }
        else if (mAutoCircleButton->value())
        {
            mCircleType = CircleType::Auto;
            mGraphWindow.lock()->autoMode();
        }
    }

    void ConsoleWindow::updateCircle()
    {
        if (mCircleType == CircleType::Custom)
        {
            if (!mCustomMessage->active())
                mCustomMessage->activate();
            if (!mRadiusSlider->active())
                mRadiusSlider->activate();
            if (!mColorChooser->active())
                mColorChooser->activate();
        }
        else
        {
            if (mCustomMessage->active())
                mCustomMessage->deactivate();
            if (mRadiusSlider->active())
                mRadiusSlider->deactivate();
            if (mColorChooser->active())
                mColorChooser->deactivate();
        }
    }

} // namespace VisualQuadtree