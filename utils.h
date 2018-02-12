
bool equal(double a, double b)
{
    a*=100;
    b*=100;
    a=int(a);
    b=int(b);
    a/=100;
    b/=100;

    return (a==b);
}


double abs(double x)
{
    if (x<0) return -x;
    else return x;
}

using namespace std;

double max3(double a, double b, double c)
{
    if(a > b)
    {
        if(a > c) return a;
        else return c;
    }
    else
    {
        if (b> c) return b;
        else return c;
    }
}
double min3(double a, double b, double c, int& smallestID)
{
    if(a < b)
    {
        if(a < c)
        {
            smallestID = 0;
            return a;
        }
        else
        {
            smallestID = 2;
            return c;
        }
    }
    else
    {
        if (b < c)
        {
            smallestID = 1;
            return b;
        }
        else
        {
            smallestID = 2;
            return c;
        }
    }
}

enum drawingState
{
    STANDARD_SCREEN,
    MINIMAP,
    SCREEN_STATES_NUMBER
};

enum ScreenStates
{
    WINDOW,
    FULLSCREEN,
    SCREEN_STATES_TOTAL
};
enum ViewDirections
{
    FORWARD,
    BACKWARDS,
    LEFT,
    RIGHT
};

class Array2D
{
private:
    int w;
    int h;
    int elementsCount;
    int size;
    unsigned char* array1D;
public:
    void init(int inputW, int inputH, int inputElementsCount)
    {
        w = inputW;
        h = inputH;
        elementsCount = inputElementsCount;
        size = w * h * elementsCount;
        array1D = new unsigned char[size];
        for(int i=0; i<size; i++)
        {
            array1D[i]= 0;
        }
    }
    void set(int x, int y, unsigned char R, unsigned char G, unsigned char B, unsigned char alpha)
    {
        int index = w * y * elementsCount + x * elementsCount;
        array1D[index+0]=alpha;
        array1D[index+1]=B;
        array1D[index+2]=G;
        array1D[index+3]=R;
    }
    void fill(unsigned char value)
    {
     for(int i=0; i<size; i++)
     {
         array1D[i]=value;
     }
    }
    void fillWithColor(unsigned char R, unsigned char G, unsigned char B, unsigned char alpha)
    {
        int index;
        for(int i=0; i<size; i+=elementsCount)
        {
        array1D[index+0]=alpha;
        array1D[index+1]=B;
        array1D[index+2]=G;
        array1D[index+3]=R;
        }
    }

    unsigned char* getPointer()
    {
        return &array1D[0];
    }
    int getW()
    {
        return w;
    }
    int getH()
    {
        return h;
    }
    int getSize()
    {
        return size;
    }
    int getElementsCount()
    {
        return elementsCount;
    }
};

void drawLoadingLogo(string path, int width, int height)
{
    SDL_Window * window;
    SDL_Renderer * renderer;

    window = SDL_CreateWindow(
                              "Loading...",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width,
                              height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS
                              );
    renderer = SDL_CreateRenderer(
                                  window,
                                  NULL,
                                  SDL_RENDERER_ACCELERATED
                                  );
    SDL_Texture * texture;
    SDL_Surface * surface;
    surface = IMG_Load(path.c_str());
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_RenderClear(renderer);
    SDL_Rect rect = {0,0, width, height};
    SDL_RenderCopy(renderer, texture, &rect, &rect);
    SDL_RenderPresent(renderer);
    SDL_Delay(5000);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture);
}

bool init(char screenState, int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Window* myWindow, SDL_Renderer* &myRenderer)
{
     if (0 > SDL_Init(SDL_INIT_EVERYTHING))
     {
      printf("Couldn't start SDL : %s", SDL_GetError());
      return false;
     }
     if(0 > IMG_Init(IMG_INIT_PNG))
     {
      printf("Couldn't start SDL_IMG : %s", IMG_GetError());
     };
     //drawLoadingLogo("logo.png", 900, 631);
     myWindow = SDL_CreateWindow(
                                 "raycast",
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SCREEN_WIDTH,
                                 SCREEN_HEIGHT,
                                 SDL_WINDOW_SHOWN
                                 );
    if(myWindow == NULL)
    {
        printf("Couldn't create window: %s", SDL_GetError());
    }

    myRenderer = SDL_CreateRenderer(
                                    myWindow,
                                    NULL,
                                    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
                                    );
    if( !SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"))
        printf("Cannot set hint texture filtering: %s\n", SDL_GetError());
    if(myRenderer == NULL)
    {
        printf("Couldn't create renderer: %s", SDL_GetError());
    }

    if (0 > SDL_SetRenderDrawBlendMode(myRenderer, SDL_BLENDMODE_BLEND))
    {
        printf("Couldn't set blend mode: %s", SDL_GetError());
    }
    if (0 > Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 6, 2048 ))
    {
        printf("\nCouldn't open Mixer: %s", Mix_GetError());
    }
    return true;
}

class View
{
private:
    MyPoint pointA;
    MyPoint pointB;
    MyPoint observer;
    MyPoint center;

    Wektor frontDirection;
    Wektor sideDirection;
    double deltaAngle;
    double angle;

    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    int OBSERVER_DISTANCE;
protected:
     double radius;
public:
    View(int inSCREEN_WIDTH, int inSCREEN_HEIGHT, int AB_DISTANCE, int inOBSERVER_DISTANCE)
    {
        SCREEN_WIDTH = inSCREEN_WIDTH;
        SCREEN_HEIGHT = inSCREEN_HEIGHT;
        OBSERVER_DISTANCE = inOBSERVER_DISTANCE;
        observer.define(0,0);
        pointA.define(AB_DISTANCE/2, OBSERVER_DISTANCE);
        pointB.define(-AB_DISTANCE/2, OBSERVER_DISTANCE);
        radius = pointPointDistance(pointA, observer);
        deltaAngle =(acos((2*radius*radius - SCREEN_WIDTH*SCREEN_WIDTH)/(2*radius*radius)));
        angle= 0.376991;
    }
    void updateDirection()
    {
        MyPoint middle;
        middle.define( (pointA.x+pointB.x)/2 , (pointA.y+pointB.y)/2 );
        frontDirection.define( (middle.x - observer.x)/OBSERVER_DISTANCE,
                         (middle.y - observer.y)/OBSERVER_DISTANCE);
        frontDirection=
                    {
                     frontDirection.x / frontDirection.getLength(),
                     frontDirection.y / frontDirection.getLength()
                    };
        sideDirection.define( (pointA.x - pointB.x)/SCREEN_WIDTH,
                              (pointA.y - pointB.y)/SCREEN_WIDTH);
        sideDirection=
                    {
                     sideDirection.x / sideDirection.getLength(),
                     sideDirection.y / sideDirection.getLength()
                    };
    }

    void translate(char dir, double howMuch)
    {
        char sign;
        Wektor direction;
       switch(dir)
        {
            case FORWARD : {sign=1; direction=frontDirection;}; break;
            case BACKWARDS : {sign = -1; direction=frontDirection;}; break;
            case LEFT : {sign = 1; direction=sideDirection;}; break;
            case RIGHT : {sign = -1; direction=sideDirection;}; break;
        }

        observer.x+= sign * howMuch * direction.x;
        observer.y+= sign * howMuch * direction.y;
        pointA.x+=  sign * howMuch * direction.x;
        pointA.y+=  sign * howMuch * direction.y;
        pointB.x+=  sign * howMuch * direction.x;
        pointB.y+=  sign * howMuch * direction.y;
    }
    Wektor getTranslationVector(char dir, double howMuch)
    {
        Wektor translationVector;
        char sign;
        Wektor direction;
       switch(dir)
        {
            case FORWARD : {sign=1; direction=frontDirection;}; break;
            case BACKWARDS : {sign = -1; direction=frontDirection;}; break;
            case LEFT : {sign = 1; direction=sideDirection;}; break;
            case RIGHT : {sign = -1; direction=sideDirection;}; break;
        }

        translationVector.x= sign * howMuch * direction.x;
        translationVector.y= sign * howMuch * direction.y;
        return translationVector;
    }
    void rotate(double inputAngle)
    {
        angle+=inputAngle;
        if (angle > 2* PI) angle-= 2* PI;
        if (angle <-2*PI) angle+= 2*PI;


        MyPoint abstractA, abstractB, abstractObserver;
        abstractA.define(pointA.x - observer.x, pointA.y - observer.y);
        abstractB.define(pointA.x - observer.x, pointA.y - observer.y);
        abstractObserver.define(0,0);

        pointA.x=cos(angle)*radius + observer.x;
        pointA.y=sin(angle)*radius + observer.y;
        pointB.x=cos(angle + deltaAngle)*radius + observer.x;
        pointB.y=sin(angle + deltaAngle)*radius + observer.y;
        center = triangleMiddle(pointA, pointB, observer);

    }
    void trans(Wektor move)
    {
        observer.x+= move.x;
        observer.y+= move.y;
        pointA.x+=  move.x;
        pointA.y+=  move.y;
        pointB.x+=  move.x;
        pointB.y+=  move.y;
        center.x+= move.x;
        center.y+= move.y;
    }
    MyPoint getObserver()
    {
        return observer;
    }
    MyPoint getA()
    {
        return pointA;
    }
    MyPoint getB()
    {
        return pointB;
    }
    MyPoint getCenter()
    {
        return center;
    }
    double getAngle()
    {
        return angle;
    }
    double getRadius()
    {
        return radius;
    }
    double getObserverDistance()
    {
        return OBSERVER_DISTANCE;
    }
};

struct Color
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
};

const Color RED = {255, 0, 0, 255};
const Color ORANGE = {255, 128, 0, 255};
const Color GREEN = {0, 255, 0, 255};
const Color BLUE = {0, 0, 255, 255};
const Color LIGHT_BLUE = {0, 0, 255, 100};


void drawPoint(SDL_Renderer* myRenderer, MyPoint point, int size, Color color)
{
   SDL_SetRenderDrawColor(myRenderer, color.R, color.G, color.B, color.A);
   SDL_Rect rect = { int(point.x) - size/2, int(point.y) - size/2,
                    size, size };
   SDL_RenderFillRect(myRenderer, &rect);
}
void drawLine(SDL_Renderer* myRenderer, MyPoint A, MyPoint B, Color color)
{
    SDL_SetRenderDrawColor(myRenderer, color.R, color.G, color.B, color.A);
    SDL_RenderDrawLine(myRenderer, int(A.x), int(A.y), int(B.x), int(B.y) );
}

void drawGood(SDL_Renderer* myRenderer)
{
    SDL_Rect rect = {0,0, 100, 100};
    SDL_SetRenderDrawColor(myRenderer, 0, 255, 0, 255);
    SDL_RenderFillRect(myRenderer, &rect);
}
void drawBad(SDL_Renderer* myRenderer)
{
    SDL_Rect rect = {0,0, 20, 20};
    SDL_SetRenderDrawColor(myRenderer, 255, 0, 0, 255);
    SDL_RenderFillRect(myRenderer, &rect);
}

class MyTexture
{
 private:

        SDL_Rect rect;
        SDL_Texture* texture;
 public:

        void createFromFile(SDL_Renderer* myRenderer, string path)
        {
            SDL_Surface* surface;
            surface = IMG_Load(path.c_str());
            if (surface == NULL)
            {
                printf("\n%s", IMG_GetError());
            }
            rect = {0, 0, surface->w, surface->h};
            texture = SDL_CreateTextureFromSurface(myRenderer, surface);
            if (texture == NULL)
            {
                printf("\n%s", SDL_GetError());
            }
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            SDL_FreeSurface(surface);
        }

        void createBlank(SDL_Renderer * renderer, int w, int h, int Flags=NULL)
        {
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                         Flags, w, h);
            if (texture == NULL)
            {
                printf("\n%s", SDL_GetError());
            }
            rect = {0,0, w, h};
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        }

        void render(SDL_Renderer * renderer, SDL_Rect* rectIn=NULL, SDL_Rect* rectOut=NULL)
        {
            if (rectIn == NULL) rectIn = &rect;
            if (rectOut == NULL) rectOut = &rect;
            SDL_RenderCopy(renderer, texture, rectIn, rectOut);
        }

        unsigned char * getPixelData(SDL_Renderer * renderer)
        {
            unsigned char * pixelData = new unsigned char [rect.w*rect.h*4];
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, &rect, &rect);
            if(0<SDL_RenderReadPixels(renderer, &rect, SDL_PIXELFORMAT_RGBA8888, pixelData, rect.w*4))
            printf("\n%s", SDL_GetError());
            return pixelData;
        }
        void loadPixelData(unsigned char * pixelData)
        {
            void* pixels;
            int pitch = rect.w * 4;
            SDL_LockTexture(texture, &rect, &pixels, &pitch);
            memcpy( pixels, &pixelData[0], rect.w*rect.h*4 );
            SDL_UnlockTexture(texture);
        }
        void setAsRenderTarget(SDL_Renderer * renderer)
        {
            SDL_SetRenderTarget(renderer, texture);
        }
        void setAlpha(unsigned char alpha)
        {
            SDL_SetTextureAlphaMod(texture, alpha);
        }

        int getX()
        {
            return rect.x;
        }
        int getY()
        {
            return rect.y;
        }
        SDL_Rect getRect()
        {
            return rect;
        }
        void setX(int inX)
        {
            rect.x = inX;
        }
        void setY(int inY)
        {
            rect.y = inY;
        }
        SDL_Texture *getTexture()
        {
            return texture;
        }
        int getW()
        {
            return rect.w;
        }
        int getH()
        {
            return rect.h;
        }
        ~MyTexture()
        {
            SDL_DestroyTexture(texture);
        }
};

class AnimatedTexture
{
    private:

        SDL_Rect rect;
        SDL_Texture* texture;
        int frameWidth;
        int frameHeight;
        int currentFrame=0;
        int framesCount;
        int speed;
 public:

        void createFromFile(SDL_Renderer* myRenderer, string path, int inFramesCount, int inSpeed)
        {
            SDL_Surface* surface;
            surface = IMG_Load(path.c_str());
            if (surface == NULL)
            {
                printf("\n%s", IMG_GetError());
            }
            rect = {0, 0, surface->w, surface->h};
            texture = SDL_CreateTextureFromSurface(myRenderer, surface);
            if (texture == NULL)
            {
                printf("\n%s", SDL_GetError());
            }
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            SDL_FreeSurface(surface);

            frameWidth = rect.w;
            frameHeight = rect.h / inFramesCount;
            framesCount = inFramesCount;
            speed = inSpeed;
        }
        void render(SDL_Renderer * renderer, SDL_Rect* rectOut=NULL)
        {
            SDL_Rect rectIn = {0, currentFrame/speed * frameHeight , frameWidth, frameHeight};
            if (rectOut == NULL)
            {
                SDL_Rect rectTmp = {0,0, frameWidth, frameHeight};
                rectOut = &rectTmp;
            }
            SDL_RenderCopy(renderer, texture, &rectIn, rectOut);
            if ( (++currentFrame) / speed  >= framesCount) currentFrame = 0;
        }
        void setAlpha(unsigned char alpha)
        {
            SDL_SetTextureAlphaMod(texture, alpha);
        }

        int getX()
        {
            return rect.x;
        }
        int getY()
        {
            return rect.y;
        }
        SDL_Rect getRect()
        {
            return rect;
        }
        void setX(int inX)
        {
            rect.x = inX;
        }
        void setY(int inY)
        {
            rect.y = inY;
        }
        SDL_Texture *getTexture()
        {
            return texture;
        }
        int getW()
        {
            return rect.w;
        }
        int getH()
        {
            return rect.h;
        }
        int getFrameWidth()
        {
            return frameWidth;
        }
        int getFrameHeight()
        {
            return frameHeight;
        }
        int getFramesCount()
        {
            return framesCount;
        }
        ~AnimatedTexture()
        {
            SDL_DestroyTexture(texture);
        }
};


void quit(SDL_Renderer* renderer, SDL_Window* window)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

struct MyPair
{
    bool x;
    bool y;
};
bool isFileEmpty(string path)
{
    ifstream in;
    in.open(path.c_str(), ios::ate | ios::binary);
    return !in.tellg();
}
