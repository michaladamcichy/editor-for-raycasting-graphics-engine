
class Mouse
{
    private:
    int x, y;
    Wektor transXY;

    public:
    int REPEAT = 2;
    bool left = false;
    bool right  = false;

    bool leftReleased= true;
    int leftRepeat=REPEAT;
    int rightRepeat=REPEAT;

    bool wheelUp = false;
    bool wheelDown = false;

    Mouse()
    {
        SDL_GetMouseState(&x, &y);
    }

    void recordMovement(SDL_Window * window, int initialX, int initialY)
    {
        SDL_GetMouseState(&x, &y);
        transXY = {x - initialX, y - initialY};
       //SDL_WarpMouseInWindow(window, initialX, initialY);
    }
    Wektor getTransXY()
    {
        return transXY;
    }

    void handleButtons(SDL_Event event)
    {
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                {
                    left = true;
                }
                break;
            case SDL_BUTTON_RIGHT:
                {
                    right = true;
                }
                break;
            default: break;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                {
                    left = false;
                }
                break;
            case SDL_BUTTON_RIGHT:
                {
                    right = false;
                }
                break;
            default: break;
            }
        }
        else if (event.wheel.y != 0)
        {
            if (event.wheel.y == 1)
            {
             wheelUp = true;
             wheelDown = false;
            }
            else if (event.wheel.y == -1)
            {
             wheelDown = true;
             wheelUp = false;
            }
        }
        else
        {
            wheelUp = false;
            wheelDown = false;
        }
    }
    MyPoint getCurrentPos()
    {
     return {x, y};
    }
};


class Keyboard
{
    private:
    public:

    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    bool space = false;
    bool leftCtrl = false;
    bool leftShift = false;
    bool tab = false;

    void handleInput(SDL_Event event)
    {
        if(event.type == SDL_KEYDOWN)
                        {
                            switch(event.key.keysym.scancode)
                            {
                                case SDL_SCANCODE_UP:
                                    {
                                        up = true; break;
                                    }
                                case SDL_SCANCODE_W:
                                    {
                                        up = true; break;
                                    }
                                case SDL_SCANCODE_DOWN:
                                    {
                                        down = true; break;
                                    }
                                case SDL_SCANCODE_S:
                                    {
                                        down = true; break;
                                    }
                                case SDL_SCANCODE_LEFT:
                                    {
                                        left = true; break;
                                    }
                                case SDL_SCANCODE_A:
                                {
                                    left = true; break;
                                }
                                case SDL_SCANCODE_RIGHT:
                                    {
                                        right = true; break;
                                    }
                                case SDL_SCANCODE_D:
                                    {
                                        right = true; break;
                                    }
                                case SDL_SCANCODE_SPACE:
                                    {
                                        space = true; break;
                                    }
                                case SDL_SCANCODE_LCTRL:
                                    {
                                        leftCtrl = true; break;
                                    }
                                case SDL_SCANCODE_LSHIFT:
                                    {
                                        leftShift = true; break;
                                    }
                                case SDL_SCANCODE_TAB:
                                    {
                                        tab = true; break;
                                    }
                                default: break;
                            }


                        }
                        else if(event.type == SDL_KEYUP)
                        {
                               switch(event.key.keysym.scancode)
                            {
                                case SDL_SCANCODE_UP:
                                    {
                                        up = false; break;
                                    }
                                case SDL_SCANCODE_W:
                                    {
                                        up = false; break;
                                    }
                                case SDL_SCANCODE_DOWN:
                                    {
                                        down = false; break;
                                    }
                                case SDL_SCANCODE_S:
                                    {
                                        down = false; break;
                                    }
                                case SDL_SCANCODE_LEFT:
                                    {
                                        left = false; break;
                                    }
                                case SDL_SCANCODE_A:
                                    {
                                        left = false; break;
                                    }
                                case SDL_SCANCODE_RIGHT:
                                    {
                                        right = false; break;
                                    }
                                case SDL_SCANCODE_D:
                                    {
                                        right = false; break;
                                    }
                                case SDL_SCANCODE_SPACE:
                                    {
                                        space = false; break;
                                    }
                                case SDL_SCANCODE_LCTRL:
                                    {
                                        leftCtrl = false; break;
                                    }
                                case SDL_SCANCODE_LSHIFT:
                                    {
                                        leftShift = false; break;
                                    }
                                case SDL_SCANCODE_TAB:
                                    {
                                        tab = false; break;
                                    }
                                default: break;
                            }
                        }
    }
};
