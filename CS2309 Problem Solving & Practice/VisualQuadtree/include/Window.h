#pragma once

#include <array>
#include <memory>
#include <string>

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>

#include "Button.h"
#include "ColorChooser.h"
#include "Message.h"
#include "Quadtree.h"
#include "RadioButton.h"
#include "Slider.h"

namespace VisualQuadtree
{
    class Window;
    class MainWindow;
    class ConsoleWindow;
    class GraphWindow;

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
        std::shared_ptr<GraphWindow>   mGraphWindow;
    };

    class ConsoleWindow : public Fl_Double_Window
    {
    public:
        explicit ConsoleWindow(int x, int y, int width, int height, std::string_view title);
        virtual ~ConsoleWindow() = default;
        void linkGraphWindow(std::shared_ptr<GraphWindow> GraphWindow);

        virtual void draw() override;
        virtual int  handle(int event) override;

        std::pair<int, Fl_Color> circleInfo() const;

    private:
        std::weak_ptr<GraphWindow> mGraphWindow;

        void createWidgets();

        std::unique_ptr<Message> mButtonMessage;
        std::unique_ptr<Button>  mPauseButton;
        bool                     mPausePressed;
        std::unique_ptr<Button>  mClearButton;
        bool                     mClearPressed;
        std::unique_ptr<Button>  mFillCircleButton;
        bool                     mFillCirclePressed;
        std::unique_ptr<Button>  mShowQuadtreeButton;
        bool                     mShowQuadtreePressed;
        std::unique_ptr<Button>  mStatisticsButton;
        bool                     mStatisticsPressed;
        void                     updateButtons();

        std::unique_ptr<Message> mSliderMessage;
        std::unique_ptr<Slider>  mRestitutionSlider;
        int                      mRestitution;
        std::unique_ptr<Slider>  mQuadtreeCircleLimitSlider;
        int                      mQuadtreeCircleLimit;
        std::unique_ptr<Slider>  mTickSlider;
        int                      mTick;
        void                     updateSliders();

        std::unique_ptr<Message>     mCircleTypeMessage;
        std::unique_ptr<Fl_Group>    mCircleTypeGroup;
        std::unique_ptr<RadioButton> mCustomCircleButton;
        std::unique_ptr<RadioButton> mRandomCircleButton;
        std::unique_ptr<RadioButton> mAutoCircleButton;
        enum class CircleType { Custom, Random, Auto };
        CircleType mCircleType;
        void       updateCircleType();

        std::unique_ptr<Message>      mCustomMessage;
        std::unique_ptr<Slider>       mRadiusSlider;
        int                           mRadius;
        std::unique_ptr<ColorChooser> mColorChooser;
        Fl_Color                      mColor;
        void                          updateCircle();
    };

    class GraphWindow : public Fl_Double_Window
    {
    public:
        explicit GraphWindow(int x, int y, int width, int height, std::string_view title);
        virtual ~GraphWindow() = default;
        void linkConsoleWindow(std::shared_ptr<ConsoleWindow> consoleWindow);

        virtual void draw() override;
        virtual int  handle(int event) override;

        void   setTickTime(double tickTime);
        double getTickTime() const;

        void setPause(bool pause);
        void setFillCircle(bool fillCircle);
        void setShowQuadtree(bool showQuadtree);
        void setShowFPS(bool showFPS);

        void setRestitution(int restitution);
        void setQuadtreeCircleLimit(int quadtreeCircleLimit);

        void customMode();
        void randomMode();
        void autoMode();

        void clear();

        void logicUpdate();
        bool insert(const std::shared_ptr<Circle> &circle);

    private:
        std::weak_ptr<ConsoleWindow> mConsoleWindow;
        int                          mWidth;
        int                          mHeight;

        Quadtree                             mQuadtree;
        std::vector<std::shared_ptr<Circle>> mCircles;

        bool mFillCircle;
        bool mShowQuadtree;
        bool mPause;

        enum class CircleType { Custom, Random, Auto };
        CircleType mCircleType;

        struct CustomCircle
        {
            bool     mCenterSet = false;
            Vec2     mMouse;
            Vec2     mCenter;
            Vec2     mVelocity;
            int      mRadius;
            Fl_Color mColor;
        };
        CustomCircle mCustomCircle;

        inline static double sTickTime = 1.0 / 30.0;
        bool                 mShowFPS;
        float                mFrameTime;
        int                  mLogicTime;
        int                  mRenderTime;
    };

} // namespace VisualQuadtree