/*
    Flappy Triangle Bird Game
    Copyright (C) 2026 Sances84

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
// Some of comments that are with slash and lot of stars are from an template of Dev C++, however the commentary is mine -main developer of game
// thanks to https://gist.github.com/stavrossk/5004111 for helping in drawing texts but GPT-Luna 6 improve it
// +Uses modern SDL3 without old SDL2 from SDL3-devel-3.4.18-mingw.zip and SDL3_ttf-devel-3.2.2-mingw.zip files
/**************************
 * Includes
 *
 **************************/

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>
#include <cstdlib>
#include <stdio.h>

class statics { // Thanks to the GeeksForGeeks to learned to make static variables infrastructure
public:
    static int score;
    static float pos_x;
    static float pos_y;
    static float uptime;
    static float jumpProg;
    static float pipe_pos_x;
    static float pipe_pos_y;
    static bool pipeDir;
    static bool gameover;
    static float gameoverProg;
    static bool mainmenu;
    static bool debug;
    static float resW;
    static float resH;
    static float resTextW;
    static float resTextH;
    statics(){
        // Do nothing
    };
};
float statics::pos_x = 0;
float statics::pos_y = 0;
float statics::pipe_pos_x = 1;
float statics::pipe_pos_y = 0.9f;
int statics::score = 0;
float statics::uptime = 0;
float statics::jumpProg = 0;
bool statics::gameover = false;
float statics::gameoverProg = 0;
bool statics::mainmenu = true;
bool statics::debug = false;
bool statics::pipeDir = false;
float statics::resW = 812;
float statics::resH = 812;
float statics::resTextW = statics::resW / 2.0f;
float statics::resTextH = statics::resH / 2.0f;
bool collision(float dotX,float dotY, float zoneX1, float zoneX2, float zoneY1, float zoneY2)
{
	return dotX > zoneX1 && dotX < zoneX2 && dotY > zoneY1 && dotY < zoneY2;
}
float randomValue() { // GPT-Luna 6 helped me too much to give the randomValue thingy for RNG of the game engine in this game
    return static_cast<float>(rand()) / RAND_MAX;
}
bool randomBool() {
	return randomValue() < 0.5f ? true : false;
}
void restart()
{
    statics::pos_x = 0;
	statics::pos_y = 0;
	statics::pipe_pos_x = 1;
	statics::pipe_pos_y = 0.9f;
	statics::score = 0;
	statics::uptime = 0;
	statics::jumpProg = 0;
	statics::gameover = false;
	statics::gameoverProg = 0;
	statics::pipeDir = false;
}
void drawText(const char* text, int size, float x, float y)
{
    TTF_Font* font = TTF_OpenFont("ARIAL.TTF", size);
    if (!font) {
        SDL_Log("TTF_OpenFont failed: %s", SDL_GetError());
        return;
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, white);
    TTF_CloseFont(font);

if (!surface) {
    SDL_Log("Text render failed: %s", SDL_GetError());
    TTF_CloseFont(font);
    return;
}
SDL_Log("Text surface: %d x %d", surface->w, surface->h);

    SDL_Surface* rgba = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);

    if (!rgba) {
        SDL_Log("SDL_ConvertSurface failed: %s", SDL_GetError());
        return;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgba->w, rgba->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, rgba->pixels);

    float w = 2.0f * rgba->w / statics::resTextW;
    float h = 2.0f * rgba->h / statics::resTextH;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1, 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(x,     y);
        glTexCoord2f(1, 0); glVertex2f(x + w, y);
        glTexCoord2f(1, 1); glVertex2f(x + w, y - h);
        glTexCoord2f(0, 1); glVertex2f(x,     y - h);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glDeleteTextures(1, &texture);
    SDL_DestroySurface(rgba);
}
void setViewport(int windowW, int windowH) { // Provided by GPT-6 Luna
    if (windowW <= 0 || windowH <= 0) return;

    float gameAspect = 1.0f; // 512 x 512
    float windowAspect = static_cast<float>(windowW) / windowH;

    int viewportW, viewportH;
    if (windowAspect > gameAspect) {
        viewportH = windowH;
        viewportW = static_cast<int>(windowH * gameAspect);
    } else {
        viewportW = windowW;
        viewportH = static_cast<int>(windowW / gameAspect);
    }

    int x = (windowW - viewportW) / 2;
    int y = (windowH - viewportH) / 2;

    statics::resW = static_cast<float>(viewportW);
    statics::resH = static_cast<float>(viewportH);
    statics::resTextW = statics::resW; // half-width in pixels for your text formula
    statics::resTextH = statics::resH;

    glViewport(x, y, viewportW, viewportH);
}
int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // Your existing glBegin/glEnd drawing uses older OpenGL features.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_Window* window = SDL_CreateWindow(
        "Flappy Triangle Bird Game",
        statics::resW, statics::resH,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
    );

    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_Log("OpenGL context creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;
    float theta = 0.0f;
    TTF_Init();

	// Pass zero for width and height to draw the whole surface
	SDL_Rect textLocation = { 100, 100, 0, 0 };
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            
			if (event.type == SDL_EVENT_WINDOW_RESIZED) {
         	   setViewport(event.window.data1, event.window.data2);
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                } else if (event.key.key == SDLK_UP) {
                    statics::mainmenu = false;
                    if (statics::jumpProg < 0.01f && !statics::gameover) {
                        statics::jumpProg = 1;
                    }
                }
            }
        }
         	float birdLeft   = statics::pos_x - (0.2175f / 1.4f);
			float birdRight  = statics::pos_x + (0.2175f / 1.4f);
   			float birdBottom = statics::pos_y - (0.125f / 1.3f);
			float birdTop    = statics::pos_y + (0.25f / 1.3f);
   	     	float pipeLeft   = statics::pipe_pos_x + 0.25f;
    		float pipeRight  = pipeLeft + 0.125f;
    		float pipeTop    = statics::pipeDir ? -statics::pipe_pos_y / 4.0f : statics::pipe_pos_y / 4.0f;
    		float pipeBottom = statics::pipeDir ? pipeTop + 10.0f / 4.0f : pipeTop - 10.0f / 4.0f;
    		float pipeTopCol    = statics::pipeDir ? pipeTop + 10.0f / 4.0f : statics::pipe_pos_y / 4.0f;
    		float pipeBottomCol = statics::pipeDir ? -statics::pipe_pos_y / 4.0f : pipeTop - 10.0f / 4.0f;
            /* OpenGL animation code goes here */

            glClearColor (0.0f, 0.1f, 0.8f, 0.0f);
            glClear (GL_COLOR_BUFFER_BIT);
			/*
			   to make debug player collision hitbox visual fixed and patched, i did use this glEnable(GL_BLEND); code.
               source of this one-liner snippet was came from https://www.opengl.org/archives/resources/code/samples/advanced/advanced98/notes/node146.html
            */
            glEnable(GL_BLEND);
            glPushMatrix ();
            // glRotatef (statics::jumpProg * 20, 0.0f, 0.0f, 1.0f);
            glBegin (GL_TRIANGLES); // player bird body rendering start
            glColor3f (0.8f, 0.9f, 0.0f);   glVertex2f (statics::pos_x + 0.0f / 4, statics::pos_y + 1.0f / 4);
            glColor3f (1.0f, 1.0f, 0.0f);   glVertex2f (statics::pos_x + 0.87f / 4, statics::pos_y + -0.5f / 4);
            glColor3f (0.8f, 0.7f, 0.0f);   glVertex2f (statics::pos_x + -0.87f / 4, statics::pos_y + -0.5f / 4);
            glEnd (); // player bird body rendering start
			glBegin (GL_TRIANGLES); // player bird eye rendering start
            glColor3f (1.0f, 1.0f, 1.0f);   glVertex2f (statics::pos_x + (0.0f + 0.1f) / 4, statics::pos_y + 0.7f / 4);
            glColor3f (1.0f, 1.0f, 1.0f);   glVertex2f (statics::pos_x + (0.27f + 0.1f) / 4, statics::pos_y + 0.25f / 4);
            glColor3f (1.0f, 1.0f, 1.0f);   glVertex2f (statics::pos_x + (-0.27f + 0.1f) / 4, statics::pos_y + 0.25f / 4);
            glEnd (); // player bird eye rendering end
			glBegin (GL_TRIANGLES); // player bird eye pupil rendering start
            glColor3f (0.0f, 0.0f, 0.0f);   glVertex2f (statics::pos_x + (0.0f + 0.1f) / 4, statics::pos_y + 0.6f / 4);
            glColor3f (0.0f, 0.0f, 0.0f);   glVertex2f (statics::pos_x + (0.17f + 0.1f) / 4, statics::pos_y + 0.3f / 4);
            glColor3f (0.0f, 0.0f, 0.0f);   glVertex2f (statics::pos_x + (-0.17f + 0.1f) / 4, statics::pos_y + 0.3f / 4);
            glEnd (); // player bird eye pupil rendering end
			glBegin (GL_TRIANGLES); // player bird beak rendering start
            glColor3f (1.0f, 0.5f, 0.0f);   glVertex2f (statics::pos_x + 0.5f / 4, statics::pos_y + 0.1f / 4);
            glColor3f (1.0f, 0.5f, 0.0f);   glVertex2f (statics::pos_x + 0.19f / 4, statics::pos_y + 0.0f / 4);
            glColor3f (1.0f, 0.5f, 0.0f);   glVertex2f (statics::pos_x + 0.19f / 4, statics::pos_y + 0.2f / 4);
            glEnd (); // player bird beak rendering end
   	        glBegin (GL_TRIANGLES); // player bird wing rendering start
            glColor3f (0.9f, 1.0f, 0.0f);   glVertex2f (statics::pos_x - 0.1f / 4, statics::pos_y + (sinf(statics::uptime * 222) / 2) / 4);
            glColor3f (1.0f, 0.9f, 0.0f);   glVertex2f (statics::pos_x + 0.33f / 4, statics::pos_y + -0.25f / 4);
            glColor3f (1.0f, 0.8f, 0.0f);   glVertex2f (statics::pos_x + -0.33f / 4, statics::pos_y + -0.25f / 4);
            glEnd (); // player bird wing rendering end
            glBegin (GL_POLYGON); // pipe rendering start
            glColor3f  (0.0f,1.0f,0.0f); glVertex2f (pipeLeft,pipeTop);
            glColor3f  (0.0f,1.0f,0.0f); glVertex2f (pipeRight,pipeTop);
            glColor3f  (0.0f,1.0f,0.0f); glVertex2f (pipeRight,pipeBottom);
            glColor3f  (0.0f,1.0f,0.0f); glVertex2f (pipeLeft,pipeBottom);
			glEnd (); // pipe rendering end
  			float pipeLeft2   = statics::pipe_pos_x - (0.125f / 3) + 0.25f; // coordinates to the pipe cap before rendering
    		float pipeRight2  = pipeLeft + ((0.125f / 3) * 4);
    		float pipeTop2    = statics::pipeDir ? -statics::pipe_pos_y / 4.0f : statics::pipe_pos_y / 4.0f;
    		float pipeBottom2 = pipeTop - 0.5f / 4.0f;
			glBegin (GL_POLYGON); // pipe cap rendering start
            glColor3f  (0.0f,0.6f,0.0f); glVertex2f (pipeLeft2,pipeTop2);
            glColor3f  (0.0f,0.6f,0.0f); glVertex2f (pipeRight2,pipeTop2);
            glColor3f  (0.0f,0.6f,0.0f); glVertex2f (pipeRight2,pipeBottom2);
            glColor3f  (0.0f,0.6f,0.0f); glVertex2f (pipeLeft2,pipeBottom2);
			glEnd (); // pipe cap rendering end
			if (statics::debug)
			{
			   glBegin (GL_POLYGON); // player hitbox debug rendering start
               glColor4f  (1.0f,0.0f,0.0f,0.5f); glVertex2f (birdLeft,birdTop);
               glColor4f  (1.0f,0.0f,0.0f,0.5f); glVertex2f (birdRight,birdTop);
               glColor4f  (1.0f,0.0f,0.0f,0.5f); glVertex2f (birdRight,birdBottom);
               glColor4f  (1.0f,0.0f,0.0f,0.5f); glVertex2f (birdLeft,birdBottom);
			   glEnd (); // player hitbox debug rendering end
        	}
            glPopMatrix ();

            theta += 1.0f;
        if (!statics::mainmenu)
		{
			char text[32];
			snprintf(text, sizeof text, "Score: %d", statics::score);
			drawText(text, 24, -0.9f, 0.9f);
			if (statics::debug)
			{
			   char text[48];
			   snprintf(text, sizeof text, "pos_y: %f", static_cast<double>(statics::pos_y));
			   drawText(text, 24, -0.9f, 0.8f);
			}
            if (!statics::gameover)
            {
            	statics::pos_y -= 0.01f;
			}
			else
			{
				statics::jumpProg = 0;
				statics::pos_y -= 0.06f;
				statics::gameoverProg += 0.05f;
			}
			if (statics::gameoverProg > 1)
			{
		 		restart();
		 		continue;
			}
            statics::uptime = theta / 1000;
            if (statics::jumpProg > 0)
            {
            	statics::jumpProg -= 0.05f;
            	statics::pos_y += 0.04f;
			}
			statics::pipe_pos_x -= 0.01f + (0.01f * (statics::score / 8));
			if (statics::pipe_pos_x < -1.4)
			{
				statics::pipe_pos_x = 1.1;
				statics::score = statics::score + 1;
				statics::pipe_pos_y = randomValue() - 1.0f * 4.0f + (randomValue() * 8.0f / 2.0f) + (0.04f * (statics::score / 2));
				statics::pipeDir = randomBool();
				if (statics::pipe_pos_y < 0.2f)
				{
					statics::pipe_pos_y = statics::pipe_pos_y + 0.29f;
				}
			}
			if (statics::pos_y > 1)
            {
            	statics::pos_y = 1;
            }
 		    if (statics::pos_y < -1 && !statics::gameover)
            {
            	statics::gameover = true;
            	statics::gameoverProg = 0.9f;
            }
            if (!statics::gameover)
            { // colliders system implemented by GPT-6 Luna so i wanna to do own collision functions but isn't work for me
    			if (birdRight > pipeLeft &&
       		        birdLeft < pipeRight &&
   			 		birdTop > pipeBottomCol &&
   			        birdBottom < pipeTopCol)
                {
  		  		    statics::gameover = true;
				}
    			if (birdRight > pipeLeft2 &&
       		        birdLeft < pipeRight2 &&
   			        birdTop > pipeBottom2 &&
   			        birdBottom < pipeTop2)
                {
  		  		    statics::gameover = true;
				}
			}
		}
		else
		{
			drawText("Flappy Triangle Bird Game", 24, -0.9f, 0.9f);
			drawText("PRESS UP TO START", 24, -0.9f, 0.7f);
		}

        SDL_GL_SwapWindow(window);
        SDL_DelayPrecise(1000000);
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
	TTF_Quit();
    SDL_Quit();
    return 0;
}
