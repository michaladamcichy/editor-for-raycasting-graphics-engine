class UserInfo
{
private:
    GameObject object;
    int type;
    bool appending;
public:
    void getInfoFromUser()
    {
     bool wall;
     MyPoint A,B;
     MyPoint center;
     bool flags[3];
     bool vertPos;
     double h, w;
     string texture;

     cout<<"Welcome in the scene editor\nAnswer the questions below with boolean values: 0 or 1";
     cout<<"\nDo you want to edit existing scene? -- ";
     cin>> appending;
     cout<<"\nSpecify the kind of objects you are going to add to the scene in this session:";
     cout<<"\nIs it wall? -- ";
     cin>>wall;
     if ( wall )
     {
         type = GAME_OBJECT_WALL;
         cout<<"\nWALL attributes:";
         cout<<"\nStops rays? -- ";
         cin>>flags[0];
         cout<<"\nTransparent? -- ";
         cin>>flags[1];
         cout<<"\nCollisions? -- ";
         cin>>flags[2];
         cout<<"\nHeight (float) --";
         cin>>h;
         cout<<"\nVertical position (0 or 1, on the ground or int the air) -- ";
         cin>>vertPos;
         cout<<"\nTexture name -- ";
         cin>>texture;
         object.createWall(A, B, flags[0], flags[1], flags[2], h, vertPos, texture, NEWLY_CREATED);
     }
     else
     {
         type = GAME_OBJECT_SPRITE;
         cout<<"\nA SPRITE then.\nSPRITE attributes:";
         cout<<"\nStops rays? -- ";
         cin>>flags[0];
         cout<<"\nTransparent? -- ";
         cin>>flags[1];
         cout<<"\nCollisions? -- ";
         cin>>flags[2];
         cout<<"\nHeight (float) -- ";
         cin>>h;
         cout<<"\nWidth (int) -- ";
         cin>>w;
         cout<<"\nVertical position (0 or 1, on the ground or int the air) -- ";
         cin>>vertPos;
         cout<<"\nTexture name -- ";
         cin>>texture;
         object.createSprite(center, flags[0], flags[1], flags[2], h, w, vertPos,texture, NEWLY_CREATED);
     }
    }
    bool append()
    {
        return appending;
    }
    GameObject getObject()
    {
        return object;
    }
    int getType()
    {
        return type;
    }
};
