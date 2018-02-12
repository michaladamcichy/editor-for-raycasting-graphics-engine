class Stripe
{
    private:
    int texture;
    int objectType;
    SDL_Rect rectIn;
    SDL_Rect rectOut;
    bool transparent;
    unsigned char alpha = SDL_ALPHA_OPAQUE;
    public:
    void create(GameObject& object, int stripeIndex, int stripePositionInTexture,
                          int stripeWidth, int objectVisualWidth, int stripeVisualHeight, unsigned char inAlpha, MyPoint screenPoint, MyPoint crossing, Player& player,
                          int texH)
    {
        objectType = object.getType();
        alpha = inAlpha;
        texture = object.getTextureIndex();
        transparent = object.isTransparent();
        if ( objectType == GAME_OBJECT_WALL )
        {
        rectIn = {stripePositionInTexture-stripeWidth/2, 0, stripeWidth, texH};
        rectOut = { stripeIndex, SCREEN_HEIGHT/2 - stripeVisualHeight/2, 1, stripeVisualHeight};
        }
        else if ( objectType == GAME_OBJECT_SPRITE )
        {
        AlternativeLine OC, view;
        OC.define(player.getObserver(), crossing);
        view.define(player.getA(), player.getB());
        MyPoint temp;
        cramer( OC.A, OC.B, -OC.C, view.A, view.B, -view.C,  temp.x, temp.y);
        int actual  = pointPointDistance(temp, player.getA());
        rectIn = {0,0, 1000, 1000};
        rectOut = {actual - objectVisualWidth/2, SCREEN_HEIGHT/2 - stripeVisualHeight/2 , objectVisualWidth, stripeVisualHeight};
        }
    }
    void render(MyTexture* textures)
    {
             if (!transparent)
               {
                   SDL_RenderFillRect(myRenderer, &rectOut);
               }
               textures[texture].setAlpha(alpha);
               textures[texture].render(myRenderer, &rectIn, &rectOut);
    }
};

class Screen
{
private:
    Stripe** stripes;
    int stripesCount[NUMBER_OF_THREADS] = {0}; //Uwaga!
    int skyTranslation = 0;
    bool blur = false;
    bool minimapVisible = false;
    MyTexture* renderedImages;

    Player& player;
    GameWorld& gameWorld;

    int renderedImage=0;
    MyTexture sky;
    MyTexture snow;
    MyTexture floor;
    MyTexture gunStandard;
    AnimatedTexture* shiningGunStandard;
    AnimatedTexture gunRunning;
    AnimatedTexture fire;
    MyTexture gunAimed;
    MyTexture gunAimedFire;
    MyTexture object;
    MyTexture gunStandardFire;

    MyTexture* objectTextures;
    int objectTexturesCount;

   void prepareSnow()
   {
       for(int y = SCREEN_HEIGHT/2; y<SCREEN_HEIGHT; y++)
       {
           SDL_SetRenderDrawColor(myRenderer, 183, 166, 135, double(y-SCREEN_HEIGHT/2)/(SCREEN_WIDTH/2)*SDL_ALPHA_OPAQUE*1.5);
           SDL_RenderDrawLine(myRenderer, 0, y, SCREEN_WIDTH, y);
       }
   }
   void loadTextures()
   {
       objectTexturesCount = gameWorld.getTextures().size();
       objectTextures = new MyTexture [objectTexturesCount];
       string path;
       for(int i=0; i<objectTexturesCount; i++)
       {
           path = gameWorld.getTextures()[i] + ".png";
           objectTextures[i].createFromFile(myRenderer, path);
       }
       sky.createFromFile(myRenderer, "sky.png");
       sky.setAlpha(100);
       gunStandard.createFromFile(myRenderer, "standard.png");
       gunRunning.createFromFile(myRenderer, "running.png", 8, 3);
       gunAimed.createFromFile(myRenderer, "aimed.png");
       gunAimedFire.createFromFile(myRenderer, "aimedfireblurred.png");
       gunStandardFire.createFromFile(myRenderer, "firestandardblow.png");
       fire.createFromFile(myRenderer, "fire.png", 3, 1);
       object.createFromFile(myRenderer, "metal.png");
       floor.createFromFile(myRenderer, "soil1.png");
       floor.setAlpha(100);
       shiningGunStandard = new AnimatedTexture [player.getShiningGunStandardAnimationCount()];
       for(int i=0; i<player.getShiningGunStandardAnimationCount(); i++)
       {
           path="";
           path = "shine";
           path += char(48+i);
           path += ".png";
           shiningGunStandard[i].createFromFile(myRenderer, path.c_str(), player.getShiningGunStandardFramesCount(), 5);
       }
       snow.createBlank(myRenderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_TEXTUREACCESS_TARGET);
       snow.setAsRenderTarget(myRenderer);
       SDL_SetRenderDrawColor(myRenderer, 0, 0, 0, 255);
       SDL_RenderClear(myRenderer);
       prepareSnow();
       SDL_SetRenderTarget(myRenderer, NULL);
   }

public:
   Screen(GameWorld &gameWorldRef, Player& playerRef) : gameWorld(gameWorldRef), player(playerRef)
   {
       player = playerRef;
       gameWorld = gameWorldRef;

       loadTextures();
       player.prepareTextureRects(&gunStandard, &gunAimed, &gunRunning, &fire, SCREEN_WIDTH, SCREEN_HEIGHT);
       player.setFavorizedAnimation(4);

       stripes = new Stripe*[NUMBER_OF_THREADS];
       for(int i=0; i<NUMBER_OF_THREADS; i++)
       {
           stripes[i] = new Stripe[STRIPES_FOR_THREAD_LIMIT];  //UWAGA!
       }

       renderedImages = new MyTexture [RENDERED_IMAGES_LIMIT];
       for(int i=0; i < RENDERED_IMAGES_LIMIT; i++)
       {
           renderedImages[i].createBlank(myRenderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_TEXTUREACCESS_TARGET);
       }
   }

    void raycastThread(int partOfScreen, int threadsNumber)
   {
    stripesCount[partOfScreen] = 0;
    MyPoint currentScreenPoint, crossing, currentObserver;
    currentObserver=player.getObserver();
    Wektor screenStep;
    AlternativeLine viewLine;
    viewLine.define(player.getA(), player.getB());
    screenStep.defineFromPoints(player.getA(), player.getB());
    screenStep = screenStep.getVersor();
    Wektor initialTrans;
    initialTrans.defineFromPoints(player.getA(), player.getB());
    initialTrans.scale(double(1)/NUMBER_OF_THREADS);
    currentScreenPoint.define(player.getA().x+partOfScreen*initialTrans.x, player.getA().y+partOfScreen*initialTrans.y);
    bool existence=false;
    Stripe currentStripe;
    GameObject object;
    //
    double fullDistance;
    double percentageOfRealSize;
    double stripeVisualHeight;
    double objectVisualWidth;
    double stripePositionInTexture;
    double stripeWidth;
    int alpha;
    //
    int crossingFound = false;

    char continueRaycasting  = 1;
    for(int stripeIndex=partOfScreen*SCREEN_WIDTH/threadsNumber; stripeIndex < (partOfScreen+1)*SCREEN_WIDTH/threadsNumber; stripeIndex++)
    {
        continueRaycasting = 1;

        for(int objectIndex=0; objectIndex < gameWorld.getObjectsCount(); objectIndex++)
        {

            if (stripesCount[partOfScreen] == STRIPES_FOR_THREAD_LIMIT)
            {
                objectIndex = gameWorld.getObjectsCount();
                stripeIndex = (partOfScreen+1)*SCREEN_WIDTH/threadsNumber;
            }
            else
            {
                object = gameWorld.getObject(objectIndex);
                        if( object.rayCrossing(player, currentScreenPoint, crossing) &&
                            !object.alreadyNoticed )
                        {
                            object.alreadyNoticed = true;
                            fullDistance = perpendicularDistance(viewLine, crossing) + OBSERVER_DISTANCE;
                            percentageOfRealSize = (SCREEN_HEIGHT) /
                                                    ( (SCREEN_HEIGHT / OBSERVER_DISTANCE) *fullDistance );
                            stripeVisualHeight = percentageOfRealSize * SCREEN_HEIGHT * object_HEIGHT_MULTIPLIER;
                            objectVisualWidth = percentageOfRealSize * object.getWidth();
                            stripePositionInTexture = pointPointDistance(crossing, object.getPointA())/object.getWidth() *
                                                                                        objectTextures[object.getTextureIndex()].getW();
                            stripeWidth = object.getWidth() / objectVisualWidth;
                            alpha = percentageOfRealSize * SDL_ALPHA_OPAQUE * 1.5;
                            if (alpha > SDL_ALPHA_OPAQUE )
                            {
                                alpha = SDL_ALPHA_OPAQUE;
                            }

                            currentStripe.create(object, stripeIndex,
                                                 stripePositionInTexture, stripeWidth,
                                                 objectVisualWidth, stripeVisualHeight, alpha, currentScreenPoint, crossing, player,
                                                 objectTextures[object.getTextureIndex()].getH());
                            stripes[partOfScreen][stripesCount[partOfScreen]++]=currentStripe;

                            if ( !object.isTransparent())
                            {
                                continueRaycasting--;
                                if (!continueRaycasting)
                                {
                                objectIndex = gameWorld.getObjectsCount();
                                }
                            }
                            //cout<<"*";
                        }
            }
        }
        currentScreenPoint.x+=screenStep.x;
        currentScreenPoint.y+=screenStep.y;
    }
    //cout<<endl;
   }

   void raycast(int threadsNumber)
   {
        MyPoint currentObserver = player.getObserver();
        player.collision = false;
          for(int objectIndex=0; objectIndex < gameWorld.getObjectsCount(); objectIndex++)
            {
                gameWorld.getObject(objectIndex).calcDistanceFromObserverAndHandleCollisions(player);
            }
            timeBeg = SDL_GetTicks();
            gameWorld.sortObjects();
            timeEnd = SDL_GetTicks();
            sortingFromRaycast+=double(timeEnd-timeBeg)/1000;

       thread * threads;
       threads = new thread [NUMBER_OF_THREADS];

       for(int i=0; i<NUMBER_OF_THREADS; i++)
       {
           threads[i] = thread( [=] { raycastThread(i, NUMBER_OF_THREADS); } );
       }
       for(int i=0; i<NUMBER_OF_THREADS; i++)
       {



           threads[i].join();
       }
   }


   void drawobjects()
   {
    SDL_SetRenderDrawColor(myRenderer,0, 0, 0, 255);
    for(int thread =0; thread < NUMBER_OF_THREADS; thread++)
       {
           for(int i=stripesCount[thread]-1; i>=0; i--)
           {
             stripes[thread][i].render(objectTextures);
           }
       }

   }
   void drawSky()
   {
       if (skyTranslation<0)
       {
           skyTranslation = SCREEN_WIDTH+skyTranslation;
       }
       skyTranslation = skyTranslation%SCREEN_WIDTH;
       SDL_Rect rectIn = {skyTranslation,0,SCREEN_WIDTH-skyTranslation, SCREEN_HEIGHT/2};
       SDL_Rect rectOut = {0,0, SCREEN_WIDTH-skyTranslation, SCREEN_HEIGHT/2};
       sky.render(myRenderer, &rectIn, &rectOut);

       rectIn = {0,0,skyTranslation, SCREEN_HEIGHT/2};
       rectOut = {SCREEN_WIDTH-skyTranslation,0, skyTranslation, SCREEN_HEIGHT/2};
       sky.render(myRenderer, &rectIn, &rectOut);
   }
   void handleSkyTranslation(double degrees, int mouseTransX, int mouseSensitivity)
   {
       skyTranslation+=int(degrees/(2*PI)*SCREEN_WIDTH)*mouseTransX/mouseSensitivity;
   }
   void drawFloor()
   {
       SDL_Rect rect = {0, SCREEN_HEIGHT/4, floor.getW(), floor.getH()};
       floor.render(myRenderer, NULL, &rect);
   }

   void drawSnow()
   {
       SDL_Rect rect = {0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2};
       SDL_RenderCopy(myRenderer, snow.getTexture(), &rect, &rect);
   }

   void drawGun()
   {
       if (player.getMovement() == RUNNING)
       {
           gunRunning.render(myRenderer, player.getRunning());
       }
       else
       {
           switch(player.getPosition())
           {
            case STANDARD:
                {
                        if (player.isFiring())
                        {
                          fire.render(myRenderer, player.getFiring());
                          gunStandardFire.render(myRenderer, NULL, player.getStandard());
                        }
                        else if(player.getMovement() != MOVEMENT_NONE)
                        {
                          shiningGunStandard[player.getCurrentAnimation()].render(myRenderer, player.getStandard());
                          player.handleAnimationSwitching();
                        }
                        else
                        {
                          gunStandard.render(myRenderer, NULL, player.getStandard());
                        }
                }
                break;
            case AIMED:
                {
                        if(player.isFiring())
                        {
                         fire.render(myRenderer, player.getFiringZoom());
                         gunAimedFire.render(myRenderer, NULL, player.getAimed());
                        }
                        else
                        {
                            gunAimed.render(myRenderer, NULL, player.getAimed());
                        }
                }
                break;
            default : break;
           }
       }
   }

    /*void drawMinimap()
    {
        for(int i=0; i<gameWorld.getObjectsCount(); i++)
        {
            SDL_SetRenderDrawColor(myRenderer, 255, 0, 0, 255);
            SDL_RenderDrawLine(myRenderer, gameWorld.objects[i].getPointA().x*MINIMAP_SCALE, gameWorld.objects[i].getPointA().y*MINIMAP_SCALE,
                               gameWorld.objects[i].getPointB().x*MINIMAP_SCALE, gameWorld.objects[i].getPointB().y*MINIMAP_SCALE);
        }
        drawLine(myRenderer, player.getObserver(), player.getA());
        drawLine(myRenderer, player.getA(), player.getB());
        drawLine(myRenderer, player.getObserver(), player.getB());

        MyPoint middle = { (player.getA().x + player.getB().x) /2, (player.getA().y + player.getB().y) /2};
        Wektor wektorek;
        wektorek.defineFromPoints(player.getObserver(), middle);
        wektorek.setLength(player.getCollisionRadius());
        MyPoint avangard = player.getObserver();
        avangard.trans(wektorek.x, wektorek.y);
        drawPoint(myRenderer, avangard);

        wektorek.defineFromPoints(player.getObserver(), player.getA());
        wektorek.setLength(player.getCollisionRadius());
        avangard = player.getObserver();
        avangard.trans(wektorek.x, wektorek.y);
        drawPoint(myRenderer, avangard);

        wektorek.defineFromPoints(player.getObserver(), player.getB());
        wektorek.setLength(player.getCollisionRadius());
        avangard = player.getObserver();
        avangard.trans(wektorek.x, wektorek.y);
        drawPoint(myRenderer, avangard);
    }*/

    void prepareImage()
   {
       //renderedImages[renderedImage].setAsRenderTarget(myRenderer);
       SDL_SetRenderDrawColor(myRenderer, 0, 0, 0, 255);
       SDL_RenderClear(myRenderer);
       drawSky();
       drawSnow();
       drawobjects();
       //SDL_SetRenderTarget(myRenderer, NULL);
   }
   void render(GameWorld gameWorld)
   {
    timeBeg = SDL_GetTicks();
    prepareImage();
    /*if (blur)
    {
        renderedImages[renderedImage].setAlpha(SDL_ALPHA_OPAQUE);
        renderedImages[renderedImage].render(myRenderer);
     for(int i=0; i<RENDERED_IMAGES_LIMIT; i++)
     {
         renderedImages[i].setAlpha(SDL_ALPHA_OPAQUE/RENDERED_IMAGES_LIMIT);
         renderedImages[i].render(myRenderer);
     }
    }
    else
    {
        renderedImages[renderedImage].setAlpha(SDL_ALPHA_OPAQUE);
        renderedImages[renderedImage].render(myRenderer);
    }*/
    drawGun();
   /*if ( minimapVisible )
   {
    drawMinimap();
   }*/
   if (player.collision)
   {
    drawGood(myRenderer);
   }
   timeEnd = SDL_GetTicks();
   rendering+= double(timeEnd-timeBeg)/1000;
   SDL_RenderPresent(myRenderer);

   renderedImage = (renderedImage + 1) % RENDERED_IMAGES_LIMIT;
   }

   void minimapSwitch()
   {
       minimapVisible = !minimapVisible;
   }

   void enableBlur()
   {
       blur = true;
   }
   void disableBlur()
   {
       blur = false;
   }
};
