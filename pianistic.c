/*
 * To enable record extension in Xorg/XFree86, add the following  line in
 * Section "Module"
 *     Load         "record"
 */
// gcc pianistic.c -o pianistic -g `sdl-config --cflags --libs` -lSDL_mixer -lX11

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlibint.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/record.h>
#include "SDL.h"
#include "SDL_mixer.h"
// #include <X11/extensions/XTest.h>

/* for this struct, refer to libxnee */
typedef union {
  unsigned char    type ;
  xEvent           event ;
  xResourceReq     req   ;
  xGenericReply    reply ;
  xError           error ;
  xConnSetupPrefix setup;
} XRecordDatum;

/*
 * FIXME: We need define a private struct for callback function,
 * to store cur_x, cur_y, data_disp, ctrl_disp etc.
 */
Display *data_disp = NULL;
Display *ctrl_disp = NULL;

/* stop flag */
int stop = 0;

void event_callback (XPointer, XRecordInterceptData*);
void play (int index);
Mix_Music* sounds[26];

void loadSounds(){
  char path[256];
  int letter = 0;
  for (;letter < 12; letter ++) {
    sprintf(path, "audio/%c.wav", letter + 'a');
    sounds[letter] = Mix_LoadMUS(path);
  }
}

void play(int index) {
  Mix_PlayMusic(sounds[index%12], 0);
}

int main (int argc, char **argv)
{
  // SDL
  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
  int audio_channels = 2;
  int audio_buffers = 4096;

  SDL_Init(SDL_INIT_AUDIO);

  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
    printf("Unable to open audio!\n");
    exit(1);
  }
  loadSounds();

  // RECORD
  ctrl_disp = XOpenDisplay (NULL);
  data_disp = XOpenDisplay (NULL);


  if (!ctrl_disp || !data_disp) {
    fprintf (stderr, "Error to open local display!\n");
    exit (1);
  }

  /*
   * we must set the ctrl_disp to sync mode, or, when we the enalbe
   * context in data_disp, there will be a fatal X error !!!
   */
  XSynchronize(ctrl_disp,True);

  int major, minor;
  if (!XRecordQueryVersion (ctrl_disp, &major, &minor)) {
    fprintf (stderr, "RECORD extension not supported on this X server!\n");
    exit (2);
  }
 
  printf ("RECORD extension for local server is version is %d.%d\n", major, minor);

  XRecordRange  *rr;
  XRecordClientSpec  rcs;
  XRecordContext   rc;

  rr = XRecordAllocRange ();
  if (!rr) {
    fprintf (stderr, "Could not alloc record range object!\n");
    exit (3);
  }

  rr->device_events.first = KeyPress;
  rr->device_events.last = MotionNotify;
  rcs = XRecordAllClients;

  rc = XRecordCreateContext (ctrl_disp, 0, &rcs, 1, &rr, 1);
  if (!rc) {
    fprintf (stderr, "Could not create a record context!\n");
    exit (4);
  }

  if (!XRecordEnableContextAsync (data_disp, rc, event_callback, NULL)) {
    fprintf (stderr, "Cound not enable the record context!\n");
    exit (5);
  }

  while (stop != 1) {
    XRecordProcessReplies (data_disp);
  }

  // SDL
  Mix_CloseAudio();
  SDL_Quit();

  // RECORD
  XRecordDisableContext (ctrl_disp, rc);
  XRecordFreeContext (ctrl_disp, rc);
  XFree (rr);
 
  XCloseDisplay (data_disp);
  XCloseDisplay (ctrl_disp);
  return 0;
}

void event_callback(XPointer priv, XRecordInterceptData *hook)
{
  if (hook->category != XRecordFromServer) {
    XRecordFreeData (hook);
    return;
  }

  XRecordDatum *data = (XRecordDatum*) hook->data;

  int event_type = data->type;

  BYTE keycode;
  keycode = data->event.u.u.detail;
  switch (event_type) {
  case KeyPress:
    /* if escape is pressed, stop the loop and clean up, then exit */
    if (keycode == 9) stop = 1;

    play(keycode);
    // printf ("KeyPress: \t%s", XKeysymToString(XKeycodeToKeysym(ctrl_disp, keycode, 0)));
    break;
  default:
    break;
  }
  XRecordFreeData (hook);
}
