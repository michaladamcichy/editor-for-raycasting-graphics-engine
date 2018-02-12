
class EditorView
{
private:
    Wektor trans;
    double scale;
    int width;
    int height;
    int precision;
    bool mouseScrollingEnabledFlag = false;
    double scrollingSpeed;
public:
    EditorView(int w, int h, int initialPrecision, double initialScrollingSpeed)
    {
      trans = {0,0};
      width = w;
      height = h;
      scale = 1;
      precision = initialPrecision;
      scrollingSpeed = initialScrollingSpeed;
    }
    Wektor getTrans()
    {
        return trans;
    }
    void setTrans(Wektor inWektor)
    {
        trans = inWektor;
    }
    void addTrans(Wektor inWektor)
    {
        trans.x += inWektor.x;
        trans.y += inWektor.y;
    }
    void scaleView(double multiplier, MyPoint mouse)
    {
        mouse = RelToAbs(mouse);

        if (multiplier > 0)
        {
        scale *= multiplier;
        width *= multiplier;
        height *= multiplier;
        scrollingSpeed /= multiplier;
        }

        MyPoint screenCenter = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
        screenCenter = RelToAbs(screenCenter);
        Wektor toCursor;
        toCursor.defineFromPoints(screenCenter, mouse);
        addTrans(toCursor);
    }
    void drawCoordSys(SDL_Renderer * renderer)
    {
        MyPoint start = AbsToRel({0, 0});
        drawPoint(renderer, start, 20, BLUE);

        int prec = precision;

        for(int i=int(start.x)%prec; i < SCREEN_WIDTH; i+= prec)
        {
            drawLine(renderer, {i,0}, {i, SCREEN_WIDTH}, LIGHT_BLUE);
        }
        for(int i=int(start.y)%prec; i < SCREEN_HEIGHT; i+= prec)
        {
            drawLine(renderer, {0,i}, {SCREEN_WIDTH, i}, LIGHT_BLUE);
        }
    }
    int getW()
    {
        return width;
    }
    int getH()
    {
        return height;
    }
    double getScale()
    {
        return scale;
    }
    int getPrecision()
    {
        return precision;
    }
    void toggleMouseScrolling()
    {
        mouseScrollingEnabledFlag = !mouseScrollingEnabledFlag;
    }
    bool mouseScrollingEnabled()
    {
        return mouseScrollingEnabledFlag;
    }
    double getScrollingSpeed()
    {
        return scrollingSpeed;
    }
    MyPoint AbsToRel(MyPoint abs)
    {
        return { (abs.x - trans.x) * scale, (abs.y - trans.y) * scale};
    }
    MyPoint RelToAbs(MyPoint rel)
    {
        return { rel.x / scale + trans.x, rel.y / scale + trans.y };
    }
};
    MyPoint AbsToRel(MyPoint abs, EditorView& view)
    {
        return { (abs.x - view.getTrans().x) * view.getScale(), (abs.y - view.getTrans().y) * view.getScale()};
    }
    MyPoint RelToAbs(MyPoint rel, EditorView view)
    {
        return { rel.x / view.getScale() + view.getTrans().x, rel.y / view.getScale() + view.getTrans().y };
    }
    MyPoint roundToGrid(MyPoint point, EditorView view)
    {
        int precision = view.getPrecision() / view.getScale();
        return {  round(point.x / precision) * precision, round(point.y/precision) * precision };
    }
