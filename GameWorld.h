class GameWorld
{
private:
    vector <GameObject> objects;
    vector <MyPoint> clickedPoints;
public:

void loadObjects(string path)
{
    ifstream input;
    input.open(path.c_str());

    if ( !input.good() )
    {
        printf("Failed to open file with objects");
    }
    else
    {
        if( !isFileEmpty(path))
           {
             GameObject object;
             string type;
             MyPoint A,B;
             MyPoint center;
             bool flags[3];
             bool vertPos;
             double h, w;
             string texture;
            while( !input.eof() )
            {
             input>>type;

             if ( type == "WALL")
             {
             input>>A.x>>A.y>>B.x>>B.y>>flags[0]>>flags[1]>>flags[2]>>h>>vertPos>>texture;
             object.createWall(A, B, flags[0], flags[1], flags[2], h, vertPos, texture);
             }
             else if ( type == "SPRITE")
             {
             input>>center.x>>center.y>>flags[0]>>flags[1]>>flags[2]>>h>>w>>vertPos>>texture;
             object.createSprite(center, flags[0], flags[1], flags[2], h, w, vertPos,texture);
             }
             else break;
             objects.push_back( object );
            }
           }
    }
}
void saveObjects(string path)
{
    ofstream output;
    output.open(path.c_str());

    if ( !output.good() )
    {
        printf("Failed to open file with objects");
    }
    else
    {
            bool first = true;
            for(GameObject& object : objects)
            {
            if (!first)
            {
                output<<endl;
            }
            else
            {
                first = false;
            }
             object.writeToFile(output);
            }
    }
}
void drawObjects(SDL_Renderer* renderer, EditorView& view)
{
    for(GameObject& object : objects)
    {
        object.draw(renderer, view);
    }
    if ( clickedPoints.end() - clickedPoints.begin() == 1)
    {
        drawPoint(renderer, AbsToRel(clickedPoints[0], view), 10, ORANGE);
    }
}
void appendObject(GameObject object)
{
     objects.push_back(object);
}
void printObjectsData()
{
    cout<< endl << "OBJECTS DATA:\n";
    for( GameObject& object : objects )
    {
        object.printInfo();
    }
}
void handleClick(MyPoint currentPos, UserInfo& userInfo, EditorView& view)
    {
        if ( clickedPoints.empty() )
          {
              clickedPoints.push_back( roundToGrid( RelToAbs( currentPos, view), view) );

              if (userInfo.getType() == GAME_OBJECT_SPRITE)
              {
                  GameObject object = userInfo.getObject();
                  object.updateSprite(clickedPoints[0]);
                  appendObject(object);
                  clickedPoints.clear();
              }
          }
          else
          {
              clickedPoints.push_back( roundToGrid( RelToAbs( currentPos, view), view) );
              GameObject object = userInfo.getObject();
              object.updateWall(clickedPoints[0], clickedPoints[1]);
              appendObject(object);
              clickedPoints.clear();
          }
    }
};
