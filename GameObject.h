enum FRESHNESS
{
    LOADED_FROM_FILE,
    NEWLY_CREATED
};

enum VERT_POSITION
{
     ON_GROUND,
     HIGH
};

enum GAME_OBJECT_TYPES
{
    GAME_OBJECT_EMPTY,
    GAME_OBJECT_WALL,
    GAME_OBJECT_SPRITE
};

enum NearestPoint
{
    POINT_A,
    POINT_B,
    WALL_CENTER
};

class GameObject
{
private:
    int type = GAME_OBJECT_EMPTY;
    MyPoint center;
    MyPoint pointA;
    MyPoint pointB;
    bool stopsRays;
    bool transparent;
    bool collisions;
    double height;
    double width;
    bool verticalPosition;
    string texture;
    bool freshness = false;
public:
    void createWall(MyPoint A, MyPoint B, bool inStopsRays, bool inTransparent, bool inCollisions,
                    double inHeight, bool inVerticalPosition, string inTexture, bool fresh = LOADED_FROM_FILE)
    {
        type = GAME_OBJECT_WALL;
        pointA = A;
        pointB = B;
        stopsRays = inStopsRays;
        transparent = inTransparent;
        collisions = inCollisions;
        height = inHeight;
        verticalPosition = inVerticalPosition;
        texture = inTexture;
        freshness = fresh;
    }
    void createSprite(MyPoint inCenter, bool inStopsRays, bool inTransparent, bool inCollisions,
                      double inHeight, double inWidth, bool inVerticalPosition, string inTexture, bool fresh = LOADED_FROM_FILE)
    {
        type = GAME_OBJECT_SPRITE;
        center = inCenter;
        stopsRays = inStopsRays;
        transparent = inTransparent;
        collisions = inCollisions;
        height = inHeight;
        width = inWidth;
        verticalPosition = inVerticalPosition;
        texture = inTexture;
        freshness = fresh;
    }
    int getType()
    {
        return type;
    }
    void draw(SDL_Renderer* renderer, EditorView& view)
    {
        Color color;
        if ( freshness == LOADED_FROM_FILE )
        {
            color = GREEN;
        }
        else
        {
            color = ORANGE;
        }

        if ( type == GAME_OBJECT_SPRITE)
        {
            drawPoint(renderer, AbsToRel( center, view ), 20, color);
        }
        else if (type == GAME_OBJECT_WALL)
        {
            drawLine(renderer, AbsToRel(pointA, view), AbsToRel(pointB, view), color);
            drawPoint(renderer, AbsToRel(pointA, view), 10, color);
            drawPoint(renderer, AbsToRel(pointB, view), 10, color);
        }
    }
    void printInfo()
    {
        if (type == GAME_OBJECT_WALL)
        {
        printf("\n%s %lf %lf %lf %lf %d %d %d %lf %d %s",
               "WALL", pointA.x, pointA.y, pointB.x, pointB.y, stopsRays, transparent, collisions, height, verticalPosition, texture.c_str());

        }
        else if (type == GAME_OBJECT_SPRITE)
        {
        printf("\n%s %lf %lf %d %d %d %lf %lf %d %s",
               "SPRITE", center.x, center.y, stopsRays, transparent, collisions, height, width, verticalPosition, texture.c_str());
        }
    }
    void writeToFile(ofstream& output)
    {
         if (type == GAME_OBJECT_WALL)
        {
        output<<"WALL"<<" "<<pointA.x<<" "<<pointA.y<<" "<<pointB.x<<" "<<pointB.y<<" "<<
        stopsRays<<" "<<transparent<<" "<<collisions<<" "<<height<<" "<<verticalPosition<<" "<<texture;
        }
        else if (type == GAME_OBJECT_SPRITE)
        {
        output<<"SPRITE"<<" "<<center.x<<" "<<center.y<<" "<<
        stopsRays<<" "<<transparent<<" "<<collisions<<" "<<height<<" "<<width<<" "<<verticalPosition<<" "<<texture;
        }
    }
    bool isNewlyAdded()
    {
        return freshness;
    }
    bool updateWall(MyPoint A, MyPoint B)
    {
        pointA = A;
        pointB = B;
    }
    bool updateSprite(MyPoint C)
    {
        center = C;
    }
};
