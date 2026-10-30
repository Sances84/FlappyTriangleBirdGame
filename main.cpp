/**************************
 * Includes
 *
 **************************/

#include <gl/gl.h>
#include <SDL3/SDL.h>
#include <math.h>
#include <cstdlib>


/**************************
 * Function Declarations
 *
 **************************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);

/**************************
 * WinMain
 *
 **************************/
class statics {
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
bool statics::pipeDir = false;
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
int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int iCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;        
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "FlappyTriangle";
    RegisterClass (&wc);

    /* create main window */
    hWnd = CreateWindow (
      "FlappyTriangle", "Flappy Triangle Bird Game", 
      WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
      0, 0, 512, 512,
      NULL, NULL, hInstance, NULL);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
        }
        else
        {
   	     	float pipeLeft   = statics::pipe_pos_x + 0.25f;
    		float pipeRight  = pipeLeft + 0.125f;
    		float pipeTop    = statics::pipeDir ? -statics::pipe_pos_y / 4.0f : statics::pipe_pos_y / 4.0f;
    		float pipeBottom = statics::pipeDir ? pipeTop + 10.0f / 4.0f : pipeTop - 10.0f / 4.0f;
            /* OpenGL animation code goes here */

            glClearColor (0.0f, 0.1f, 0.8f, 0.0f);
            glClear (GL_COLOR_BUFFER_BIT);

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
            
            glPopMatrix ();

            SwapBuffers (hDC);

            theta += 1.0f;
        if (!statics::mainmenu)
		{
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
            {
                float birdLeft   = statics::pos_x - (0.2175f / 1.4f);
			    float birdRight  = statics::pos_x + (0.2175f / 1.4f);
	            float birdBottom = statics::pos_y - (0.125f / 1.3f);
			    float birdTop    = statics::pos_y + (0.25f / 1.3f);

			    if (birdRight > pipeLeft &&
       		        birdLeft < pipeRight &&
   			        birdTop > pipeBottom &&
   			        birdBottom < pipeTop)
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
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}
/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
        return 0;
    
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        case VK_UP:
        	statics::mainmenu = false;
        	if (statics::jumpProg < 0.01f && !statics::gameover)
        	{
			 	statics::jumpProg = 1;
			}
        }
        return 0;

    default:
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
}

/*******************
 * Enable OpenGL
 *
 *******************/

void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC (hWnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );

}


/******************
 * Disable OpenGL
 *
 ******************/

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}
