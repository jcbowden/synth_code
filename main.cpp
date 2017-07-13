
#ifdef WIN32
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "OpenGL32.lib")
#endif

//#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
//#endif*/

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <GL/gl.h>
#include <GL/glu.h>


#include <stdio.h>
#include <stdlib.h>

#include "WaveDev.h"
#include "SndSynth.h"

void DrawPianoKeys( void ) ;
void SetupOpengl( int width, int height ) ;
void Enter2D(int winWidth, int winHeight) ;
void Exit2D(void) ;
void PlaySounds(void) ;

int threadFunction(void *) ;

WaveDev  * ds ;
SndSynth * synth ;

char whiteKey[7] = {0,0,0,0,0,0,0} ;
char blackKey[7] = {0,0,0,0,0,0,0} ;
int * notePlayingTable ;  // this has number of elements equal to number of direct sound secondary buffers
char currentOctave = 0 ;

#define MAX_OCTAVE 2 // => 3 octaves


int main(int argc, char **argv)
{

	/* Information about the current video settings. */
    const SDL_VideoInfo* info = NULL;

    /* First, initialize SDL's video subsystem. */
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        /* Failed, exit. */
        fprintf( stderr, "Video initialization failed: %s\n", SDL_GetError( ) );
        SDL_Quit();
				return (1) ;
    }

    /* get video information. */
    info = SDL_GetVideoInfo( );
    if( !info ) {
        /* This should probably never happen. */
        fprintf( stderr, "Video query failed: %s\n",SDL_GetError( ) );
        SDL_Quit();
				return (1) ;
    }


    int width = 800;
    int height = 600;
		/* Color depth in bits of our window. */
    int bpp = info->vfmt->BitsPerPixel;

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );


    /* Set the video mode */
    if( SDL_SetVideoMode( width, height, bpp, SDL_OPENGL ) == 0 ) {
        fprintf( stderr, "Video mode set failed: %s\n",
             SDL_GetError( ) );
        SDL_Quit();
				return (1) ;
    }


		
		vector<xy> _txy ;
		
		ds = new WaveDev() ;
		ds->GetDSDevOutputCaps(0) ;	// needed
		ds->SelectDSDriver(1,0) ;		// needed

		synth = new SndSynth(SINE, 440, _txy) ;
		notePlayingTable =  (int *) calloc(NUMBEROFOCTAVES * NOTESPEROCTAVE, 4) ;

		Frequency freq ;
		synth->SetUpHeader(1.0,1,_22k,16) ;

		for (int i = 0; i < NUMBEROFOCTAVES ; i++)
		{
			for (int j = 0; j < NOTESPEROCTAVE ; j++)
			{
				freq._freq = synth->noteTable[i][j] ;
			//	synth->ConstFreqWaveform(freq._freq) ;
			//	synth->ConstFreqFeedbackWaveform(freq._freq) ;
			//	synth->LinearFreqRamp(freq._freq,freq._freq*2,1.0,2.0) ;
				synth->SineOscilatingSineWaveform(freq._freq,freq._freq+0.2,freq._freq-0.2) ;
			//	synth->_wavFile.ReverseData() ;
			//	synth->ReverseData() ;
			//	synth->LinearFreqRampMultiplesHilltop(freq._freq-(freq._freq*0.25),freq._freq,200) ;
				ds->LoadSecondaryBuffFromWav("test",&(synth->_wavFile)) ;
			}
		}

/*		synth->ConstFreqFeedbackWaveform(freq._freq) ;
		synth->_wavFile.ReverseData() ;
		ds->LoadSecondaryBuffFromWav("test",&(synth->_wavFile)) ;
		ds->LoadSecondaryBuffFromWav("test",&(synth->_wavFile)) ;
		ds->LoadSecondaryBuffFromWav("test",&(synth->_wavFile)) ;
		ds->LoadSecondaryBuffFromWav("test",&(synth->_wavFile)) ;*/

    /*
     * At this point, we should have a properly setup
     * double-buffered window for use with OpenGL.
     */
    SetupOpengl( width, height );
		Enter2D(width, height) ;
		DrawPianoKeys( ) ;

		SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
		SDL_Event event;
		while (event.type != SDL_QUIT)
		{

			SDL_WaitEvent(&event);

			switch (event.type) {
					case SDL_ACTIVEEVENT: 
            if ( event.active.state & SDL_APPACTIVE ) 
						{
							if ( event.active.gain ) 
							{
									printf("App activated\n");
									DrawPianoKeys( ) ;
							} 
							else {
									printf("App iconified\n");
							}
            }

					case SDL_KEYDOWN:
							
					//		printf("The %s key was pressed!\n", SDL_GetKeyName(event.key.keysym.sym));
					//		printf("The %d key was pressed!\n", event.key.keysym.sym);
						if (event.key.keysym.sym==SDLK_d) { whiteKey[0] = 1 ; notePlayingTable[(currentOctave*12)+3] = 1 ; }
						else if (event.key.keysym.sym==SDLK_f) { whiteKey[1] = 1 ; notePlayingTable[(currentOctave*12)+5] = 1 ;		}
						else if (event.key.keysym.sym==SDLK_g) { whiteKey[2] = 1 ; notePlayingTable[(currentOctave*12)+7] = 1 ;		}
						else if (event.key.keysym.sym==SDLK_h) { whiteKey[3] = 1 ; notePlayingTable[(currentOctave*12)+8] = 1 ;		}
						else if (event.key.keysym.sym==SDLK_j) { whiteKey[4] = 1 ; notePlayingTable[(currentOctave*12)+10] = 1 ;	}
						else if (event.key.keysym.sym==SDLK_k) { whiteKey[5] = 1 ; notePlayingTable[(currentOctave*12)+12] = 1 ;	}
						else if (event.key.keysym.sym==SDLK_l) { whiteKey[6] = 1 ; notePlayingTable[(currentOctave*12)+14] = 1 ;	}
						else if (event.key.keysym.sym==SDLK_r) { blackKey[0] = 1 ; notePlayingTable[(currentOctave*12)+4] = 1 ;		}
						else if (event.key.keysym.sym==SDLK_t) { blackKey[1] = 1 ; notePlayingTable[(currentOctave*12)+6] = 1 ;		}
						else if (event.key.keysym.sym==SDLK_u) { blackKey[3] = 1 ; notePlayingTable[(currentOctave*12)+9] = 1 ;		}
						else if (event.key.keysym.sym==SDLK_i) { blackKey[4] = 1 ; notePlayingTable[(currentOctave*12)+11] = 1 ;	}
						else if (event.key.keysym.sym==SDLK_o) { blackKey[5] = 1 ; notePlayingTable[(currentOctave*12)+13] = 1 ;	}
						else if (event.key.keysym.sym==SDLK_COMMA && currentOctave > 0 ) {
							currentOctave -= 1 ;
						//	whiteKey[6] = 1 ;
						}
						else if (event.key.keysym.sym==SDLK_PERIOD && currentOctave < MAX_OCTAVE ) {
						//	whiteKey[0] = 1 ;
							currentOctave += 1 ;
						}
						else if (event.key.keysym.sym==SDLK_ESCAPE)
							exit(0) ;

				//			printf("currentOctave  = %d\n", currentOctave );
							PlaySounds() ;
							DrawPianoKeys( ) ;
							break;
					case SDL_KEYUP:
					//		printf("The %s key was released!\n", SDL_GetKeyName(event.key.keysym.sym));
						if (event.key.keysym.sym==SDLK_d) { whiteKey[0] = 0 ;  notePlayingTable[(currentOctave*12)+3] = 0 ;	}
							else if (event.key.keysym.sym==SDLK_f) { whiteKey[1] = 0 ; notePlayingTable[(currentOctave*12)+5] = 0 ;		}
							else if (event.key.keysym.sym==SDLK_g) { whiteKey[2] = 0 ; notePlayingTable[(currentOctave*12)+7] = 0 ;		}
							else if (event.key.keysym.sym==SDLK_h) { whiteKey[3] = 0 ; notePlayingTable[(currentOctave*12)+8] = 0 ;		}
							else if (event.key.keysym.sym==SDLK_j) { whiteKey[4] = 0 ; notePlayingTable[(currentOctave*12)+10] = 0 ;	}
							else if (event.key.keysym.sym==SDLK_k) { whiteKey[5] = 0 ; notePlayingTable[(currentOctave*12)+12] = 0 ;	}
							else if (event.key.keysym.sym==SDLK_l) { whiteKey[6] = 0 ; notePlayingTable[(currentOctave*12)+14] = 0 ;	}
							else if (event.key.keysym.sym==SDLK_r) { blackKey[0] = 0 ; notePlayingTable[(currentOctave*12)+4] = 0 ;		}
							else if (event.key.keysym.sym==SDLK_t) { blackKey[1] = 0 ; notePlayingTable[(currentOctave*12)+6] = 0 ;		}
							else if (event.key.keysym.sym==SDLK_u) { blackKey[3] = 0 ; notePlayingTable[(currentOctave*12)+9] = 0 ;		}
							else if (event.key.keysym.sym==SDLK_i) { blackKey[4] = 0 ; notePlayingTable[(currentOctave*12)+11] = 0 ;	}
							else if (event.key.keysym.sym==SDLK_o) { blackKey[5] = 0 ; notePlayingTable[(currentOctave*12)+13] = 0 ;	}
							else if (event.key.keysym.sym==SDLK_COMMA && currentOctave >= 0 ) {
							//	whiteKey[6] = 0 ;
							}
							else if (event.key.keysym.sym==SDLK_PERIOD && currentOctave <= MAX_OCTAVE ) {
							//	whiteKey[0] = 0 ;
							}

							PlaySounds() ;
							DrawPianoKeys( ) ;
							break ;


					case SDL_QUIT:
							break ;
			}
		}


		delete synth ;
		delete ds ;
		return (0) ;
}


void PlaySounds(void) 
{
	for (int j = 0 ; j < NUMBEROFOCTAVES * NOTESPEROCTAVE ; j++)
	{
		if ( notePlayingTable[j] == 1 ) 
			ds->_DSBVect[j]->pDSBuff->Play(0,0,DSBPLAY_LOOPING) ;
		else
		{
			ds->_DSBVect[j]->pDSBuff->Stop() ;
			ds->_DSBVect[j]->pDSBuff->SetCurrentPosition(0) ;
		}
	}
}



int threadFunction(void *)
{

	SDL_Event event ;
//	SDL_KeyboardEvent key ;

	event.key.type = SDL_KEYDOWN ;
  event.key.state = SDL_PRESSED ;
  event.key.keysym.scancode = 97 ;
  event.key.keysym.sym = SDLK_a ;
  event.key.keysym.mod = KMOD_NONE ;
  event.key.keysym.unicode = 0 ;


	while (1)
	{
		SDL_PushEvent(&event);
		Sleep(3000) ;
	}

	return 1 ;
}



static void DrawPianoKeys( void )
{
	float dest_x = 30.0f ;
	float dest_y = 290.0f ;
	GLubyte white[]  = { 255, 255, 255, 255 };
	GLubyte black[]  = { 0, 0, 0, 255 };
	GLubyte red[]  = { 255, 0, 0, 255 };


	GLfloat wk0[] = { 0.0f,0.0f, 1.0f };
  GLfloat wk1[] = { 30.0f, 0.0f,  1.0f };
  GLfloat wk2[] = { 30.0f,  160.0f,  1.0f };
  GLfloat wk3[] = { 0.0f,  160.0f,  1.0f };

	GLfloat bk0[] = { 0.0f,0.0f, 1.0f };
  GLfloat bk1[] = { 22.0f, 0.0f,  1.0f };
  GLfloat bk2[] = { 22.0f,  90.0f,  1.0f };
  GLfloat bk3[] = { 0.0f,  90.0f,  1.0f };

	/* Clear the color and depth buffers. */
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  /* We don't want to modify the projection matrix. */
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );

	glScalef(0.5,0.5,0.5) ;
	

	for (int j = 0 ; j < MAX_OCTAVE+1 ; j++)
	{
		glLoadIdentity( );
	//	glScalef(0.5,0.5,0.5) ;
		dest_x = (j*231) + 30.0f ;
		dest_y = 290.0f ;
		glTranslatef( dest_x, dest_y, 0.0 );
		for (int i = 0 ; i < 7; i++)
		{
	//		fprintf(stderr, "white dest.x : %f\n", dest_x );
			glBegin( GL_QUADS  );
			if (whiteKey[i] == 1 && currentOctave == j)
				glColor4ubv( red );
			else
					glColor4ubv( white );
				glVertex3fv( wk0 );
				glVertex3fv( wk1 );
				glVertex3fv( wk2 );
				glVertex3fv( wk3 );
			glEnd() ;
			dest_x += 33.0f;
			glLoadIdentity( );
	//		glScalef(0.5,0.5,0.5) ;
			glTranslatef( dest_x, dest_y, 0.0 );
		}

		dest_x = (j*231) + 51.0f ;
		dest_y = 360.0f ;
		glLoadIdentity( );
	//	glScalef(0.5,0.5,0.5) ;
		glTranslatef( dest_x, dest_y, 0.0 );

		for (i = 0 ; i < 6; i++)
		{
	//		fprintf(stderr, "black dest.x : %f\n", dest_x );
			if (i != 2 )
			{
				glBegin( GL_QUADS  );
					if (blackKey[i] == 1 && currentOctave == j)
						glColor4ubv( red );
					else
						glColor4ubv( black );
					glVertex3fv( bk0 );
					glVertex3fv( bk1 );
					glVertex3fv( bk2 );
					glVertex3fv( bk3 );
				glEnd() ;
			}
			dest_x += 33.0f;
			glLoadIdentity( );
	//		glScalef(0.5,0.5,0.5) ;
			glTranslatef( dest_x, dest_y, 0.0 );
		}
	}



	SDL_GL_SwapBuffers( );
}


static void SetupOpengl( int width, int height )
{
    /* Our shading model--Gouraud (smooth). */
    glShadeModel( GL_SMOOTH );

    /* Culling. */
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );

    /* Set the clear color. */
    glClearColor( 0, 0, 0, 0 );

    /* Setup our viewport. */
    glViewport( 0,  0, width, height );

    /*
     * Change to the projection matrix and set
     * our viewing volume.
     */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
}


void Enter2D(int winWidth, int winHeight) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, winWidth, 0, winHeight, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void Exit2D(void) {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}