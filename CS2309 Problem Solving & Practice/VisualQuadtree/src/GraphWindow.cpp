#include "Window.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

static std::random_device                    sRandomDevice;
static std::mt19937                          sRandomEngine(sRandomDevice());
static std::uniform_real_distribution<float> sCenterDistributionX(0.0f, 1280.0f);
static std::uniform_real_distribution<float> sCenterDistributionY(0.0f, 720.0f);
static std::uniform_real_distribution<float> sVelocityDistribution(-100.0f, 100.0f);
static std::uniform_real_distribution<float> sRadiusDistribution(3.0f, 10.0f);
static std::uniform_int_distribution<int>    sColorDistribution(0, 255);

static void timerCallback(void *data)
{
    auto *window = reinterpret_cast<VisualQuadtree::GraphWindow *>(data);
    window->logicUpdate();
    window->redraw();
    Fl::repeat_timeout(window->getTickTime(), timerCallback, data);
}

static std::shared_ptr<VisualQuadtree::Circle> getRandomCircle()
{
    std::shared_ptr<VisualQuadtree::Circle> circle;
    circle = std::make_shared<VisualQuadtree::Circle>(
        VisualQuadtree::Vec2{sCenterDistributionX(sRandomEngine), sCenterDistributionY(sRandomEngine)},
        VisualQuadtree::Vec2{sVelocityDistribution(sRandomEngine), sVelocityDistribution(sRandomEngine)},
        sRadiusDistribution(sRandomEngine),
        fl_rgb_color(sColorDistribution(sRandomEngine), sColorDistribution(sRandomEngine), sColorDistribution(sRandomEngine)));
    return circle;
}

static std::shared_ptr<VisualQuadtree::Circle> getRandomCircle(VisualQuadtree::Vec2 center)
{
    std::shared_ptr<VisualQuadtree::Circle> circle;
    circle = std::make_shared<VisualQuadtree::Circle>(
        center, VisualQuadtree::Vec2{sVelocityDistribution(sRandomEngine), sVelocityDistribution(sRandomEngine)},
        sRadiusDistribution(sRandomEngine),
        fl_rgb_color(sColorDistribution(sRandomEngine), sColorDistribution(sRandomEngine), sColorDistribution(sRandomEngine)));
    return circle;
}

namespace VisualQuadtree
{
    GraphWindow::GraphWindow(int x, int y, int width, int height, std::string_view title) :
        Fl_Double_Window(x, y, width, height, title.data()), mQuadtree(width, height)
    {
        this->box(FL_THIN_DOWN_BOX);
        this->color(47);

        mWidth        = width;
        mHeight       = height;
        mPause        = false;
        mFillCircle   = true;
        mShowQuadtree = true;
        mCircleType   = CircleType::Custom;
        mShowFPS      = true;

        this->begin();
        Fl::add_timeout(sTickTime, timerCallback, this);
        this->end();
    }

    void GraphWindow::linkConsoleWindow(std::shared_ptr<ConsoleWindow> consoleWindow) { mConsoleWindow = consoleWindow; }

    void GraphWindow::draw()
    {
        auto now = std::chrono::high_resolution_clock::now();

        Fl_Double_Window::draw();
        for (const auto &circle : mCircles)
            circle->draw(mFillCircle);
        if (mShowQuadtree)
            mQuadtree.draw();

        if (mCustomCircle.mCenterSet)
        {
            fl_color(mCustomCircle.mColor);
            fl_circle(mCustomCircle.mCenter.x, mCustomCircle.mCenter.y, mCustomCircle.mRadius);
            fl_color(FL_WHITE);
            fl_line_style(FL_DASHDOT, 2.0f);
            fl_line(mCustomCircle.mCenter.x, mCustomCircle.mCenter.y, 2 * mCustomCircle.mCenter.x - mCustomCircle.mMouse.x,
                    2 * mCustomCircle.mCenter.y - mCustomCircle.mMouse.y);
        }

        if (mShowFPS)
        {
            fl_color(FL_WHITE);
            fl_font(FL_HELVETICA, 20);
            fl_draw(std::format("FPS: {:.2f}", mFrameTime).c_str(), 10, 30);
            fl_draw(std::format("Logic Time: {} ms", mLogicTime).c_str(), 10, 60);
            fl_draw(std::format("Render Time: {} ms", mRenderTime).c_str(), 10, 90);
            fl_draw(std::format("Total Circles: {}", mCircles.size()).c_str(), 10, 120);
        }

        auto end    = std::chrono::high_resolution_clock::now();
        mRenderTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - now).count();
    }

    int GraphWindow::handle(int event)
    {
        if (mCircleType == CircleType::Random && event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE)
            this->insert(getRandomCircle({static_cast<float>(Fl::event_x()), static_cast<float>(Fl::event_y())}));
        else if (mCircleType == CircleType::Custom && event == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE)
        {
            if (!mCustomCircle.mCenterSet)
            {
                mCustomCircle.mCenterSet = true;
                mCustomCircle.mCenter    = {static_cast<float>(Fl::event_x()), static_cast<float>(Fl::event_y())};
                auto [radius, color]     = mConsoleWindow.lock()->circleInfo();
                mCustomCircle.mRadius    = radius;
                mCustomCircle.mColor     = color;
            }
            else
            {
                mCustomCircle.mVelocity =
                    mCustomCircle.mCenter - Vec2{static_cast<float>(Fl::event_x()), static_cast<float>(Fl::event_y())};
                auto circle =
                    std::make_shared<Circle>(mCustomCircle.mCenter, mCustomCircle.mVelocity, mCustomCircle.mRadius, mCustomCircle.mColor);
                this->insert(circle);
                mCustomCircle.mCenterSet = false;
            }
        }
        if (mCircleType == CircleType::Custom)
            mCustomCircle.mMouse = {static_cast<float>(Fl::event_x()), static_cast<float>(Fl::event_y())};
        return Fl_Double_Window::handle(event);
    }

    void GraphWindow::setTickTime(double tickTime)
    {
#ifdef DEBUG
        if (tickTime <= 0.0f)
            throw std::runtime_error("Tick time must be positive");
#endif
        sTickTime = tickTime;
    }

    double GraphWindow::getTickTime() const { return sTickTime; }

    void GraphWindow::setPause(bool pause) { mPause = pause; }

    void GraphWindow::setFillCircle(bool fillCircle) { mFillCircle = fillCircle; }

    void GraphWindow::setShowQuadtree(bool showQuadtree) { mShowQuadtree = showQuadtree; }

    void GraphWindow::setShowFPS(bool showFPS) { mShowFPS = showFPS; }

    void GraphWindow::setRestitution(int restitution) { Circle::setRestitution(restitution / 100.0f); }

    void GraphWindow::setQuadtreeCircleLimit(int quadtreeCircleLimit) { mQuadtree.setCircleLimit(quadtreeCircleLimit); }

    void GraphWindow::customMode() { mCircleType = CircleType::Custom; }

    void GraphWindow::randomMode() { mCircleType = CircleType::Random; }

    void GraphWindow::autoMode() { mCircleType = CircleType::Auto; }

    void GraphWindow::clear()
    {
        mCircles.clear();
        mQuadtree.clear();
    }

    void GraphWindow::logicUpdate()
    {
        static auto      lastTime = std::chrono::high_resolution_clock::now();
        static int       count    = 0;
        static long long sum      = 0;
        auto             now      = std::chrono::high_resolution_clock::now();
        auto             duration = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count();
        if (count < 10)
        {
            sum += duration;
            ++count;
        }
        else
        {
            mFrameTime = 1e6 / (sum / 10.0);
            sum        = 0;
            count      = 0;
        }
        lastTime = now;

        auto start = std::chrono::high_resolution_clock::now();

        if (mPause)
            return;
        for (auto &circle : mCircles)
            circle->update(sTickTime);
        mQuadtree.update();

        if (mCircleType == CircleType::Auto)
            this->insert(getRandomCircle());

        auto end   = std::chrono::high_resolution_clock::now();
        mLogicTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    bool GraphWindow::insert(const std::shared_ptr<Circle> &circle)
    {
        if (!mQuadtree.collideCheck(circle) && mQuadtree.insert(circle))
        {
            mCircles.push_back(circle);
            return true;
        }
        return false;
    }

} // namespace VisualQuadtree