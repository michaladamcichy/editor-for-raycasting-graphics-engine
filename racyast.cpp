#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<iostream>
#include<cstdio>
#include<vector>
#include<cstdlib>
#include<ctime>
#include<cmath>
#include<myMath.h>
#include<thread>
#include<myGeometry.h>
#include<fstream>
#include<algorithm>
#include<utility>
#include<utils.h>
#include<input.h>
#include<time.h>
#include<config.h>
#include<editor.h>
#include<GameObject.h>
#include<userInfo.h>
#include<GameWorld.h>

using namespace std;

SDL_Renderer* renderer;
SDL_Window* window;

int main(int argc, char* argv[])
{
   UserInfo userInfo;
   userInfo.getInfoFromUser();

   int windowMode = FULLSCREEN;
   if (!init(windowMode, SCREEN_WIDTH, SCREEN_HEIGHT, window, renderer))
   {
       printf("\nERROR");
       SDL_Delay(4000);
   }
   else
   {
    GameWorld gameWorld;
    if ( userInfo.append() )
    {
    gameWorld.loadObjects("objects.txt");
    gameWorld.printObjectsData();
    }

    EditorView view(SCREEN_WIDTH, SCREEN_HEIGHT, 64, 10);
    if (windowMode == FULLSCREEN) view.toggleMouseScrolling();
    Mouse mouse;
    Keyboard keyboard;
    SDL_Event event;
    MyPoint cursor;
    bool end = false;

    while(!end)
    {
      while( SDL_PollEvent(&event) )
      {
       if (event.type == SDL_QUIT)
       {
           end = true;
       }
       else if ( event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
       {
           keyboard.handleInput( event );
       }
       else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL)
       {
           mouse.handleButtons(event);
       }
      }

      if (keyboard.up || (mouse.getCurrentPos().y < SCREEN_HEIGHT * 2/10 && view.mouseScrollingEnabled()) )
      {
          view.addTrans( { 0, -view.getScrollingSpeed()} );
      }
      if (keyboard.left || (mouse.getCurrentPos().x < SCREEN_WIDTH * 2/10 && view.mouseScrollingEnabled()) )
      {
          view.addTrans( { -view.getScrollingSpeed(), 0} );
      }
      if (keyboard.down || (mouse.getCurrentPos().y > SCREEN_HEIGHT * 8/10 && view.mouseScrollingEnabled()) )
      {
          view.addTrans( { 0, view.getScrollingSpeed()} );
      }
      if (keyboard.right || (mouse.getCurrentPos().x > SCREEN_WIDTH * 8/10 && view.mouseScrollingEnabled()) )
      {
          view.addTrans( { view.getScrollingSpeed(), 0} );
      }
      if (mouse.wheelUp)
      {
        view.scaleView( 2 , mouse.getCurrentPos());
        mouse.wheelUp = false;
      }
      else if (mouse.wheelDown)
      {
        if (view.getScale() > double(1)/1024)
        {
         view.scaleView( double(1)/2, mouse.getCurrentPos());
        }
        mouse.wheelDown = false;
      }
      if (mouse.left)
      {
          mouse.left = false;
          gameWorld.handleClick(mouse.getCurrentPos(), userInfo, view);
      }

      mouse.recordMovement(window, 0,0);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
      view.drawCoordSys(renderer);
      gameWorld.drawObjects(renderer, view);
      SDL_RenderPresent(renderer);
      cout<<endl<<view.getScale();
    }
    quit(renderer, window);
    gameWorld.printObjectsData();
    gameWorld.saveObjects("objects.txt");
    cout<<"\n\n\nI have the pleasure to inform you, that the scene was SUCCESSFULLY SAVED!";
    SDL_Delay(5000);
   }
    return 0;
}
