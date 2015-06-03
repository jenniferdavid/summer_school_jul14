#ifdef USE_X11

#include "GUI/Window.h"

#include     <stdexcept>

#include     <cstring>
#include     <X11/Xlib.h>

#include "Thread/thread.h"
#include "Thread/mutex.h"

#include <map>

// XImage *CreateTrueColorImage(Display *display, Visual *visual, unsigned char *image, int width, int height)
// {
//     int i, j;
//     unsigned char *image32=(unsigned char *)malloc(width*height*4);
//     unsigned char *p=image32;
//     for(i=0; i<width; i++)
//     {
//         for(j=0; j<height; j++)
//         {
//             if((i<256)&&(j<256))
//             {
//                 *p++=rand()%256; // blue
//                 *p++=rand()%256; // green
//                 *p++=rand()%256; // red
//             }
//             else
//             {
//                 *p++=i%256; // blue
//                 *p++=j%256; // green
//                 if(i<256)
//                     *p++=i%256; // red
//                 else if(j<256)
//                     *p++=j%256; // red
//                 else
//                     *p++=(256-j)%256; // red
//             }
//             p++;
//         }
//     }
//     return XCreateImage(display, visual, 24, ZPixmap, 0, image32, width, height, 32, 0);
// }

// fw
class X11Window; 

static bool xerror;

//----------

int errorHandler(Display *dp, XErrorEvent *e)
{
  xerror = true;
  return 0;
}

long getProperty(Display *d, Window handle, Atom atom)
{
  long result = 0;
  int userSize;
  unsigned long bytes, userCount;
  unsigned char *data;
  Atom userType;
  xerror = false;
  XErrorHandler olderrorhandler = XSetErrorHandler(errorHandler);
  XGetWindowProperty(
    d,                // display
    handle,           // window
    atom,             // property
    0, 1,             // offset, length
    false,            // delete?
    AnyPropertyType,  // req type
    &userType,        // actual type return
    &userSize,        // actual format return
    &userCount,       // n items return
    &bytes,           // bytes after return
    &data             // prop return
  );
  if (xerror==false && userCount==1) result = *(long*)data;
  XSetErrorHandler(olderrorhandler);
  return result;
}



class X11Core {
public:
  ::Display *display;
  ::Visual *visual;
  
  sprint::mutex m_lock;
  int m_token;
  
  
  std::map<Window, X11Window*> m_windows;
  
public:
  
  /// allocate new window
  Window create_new_window(int width, int height)
  {
    Window window=XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, width, height, 1, 0, 0);
    if(visual->c_class!=TrueColor)
    {
        throw std::runtime_error("Cannot handle non true color visual");
    }
    XSelectInput(display, window, ButtonPressMask|ExposureMask);
    XMapWindow(display, window); 
    return window;
  }
  
  void main_loop();
  
public:
  
  void register_new_window(Window w, X11Window *pw)
  {
    m_lock.lock();
    m_windows[w] = pw;
    m_lock.unlock();
  }
  
  int send_message(int aValue)
  {
  int token;
  m_lock.lock();
  token = m_token;
  m_token++;
  m_lock.unlock();
  
  XClientMessageEvent event;
  event.type = ClientMessage;
  event.data.l[0] = aValue;
  event.data.l[1] = token;
  
  //event.message_type = 0;
  XSendEvent(display,0,0,0,(XEvent *)&event);
  XFlush(display);
  return token;
  }
      
  
  X11Core() : m_token(0) { 
    display=XOpenDisplay(NULL);
    visual=DefaultVisual(display, 0);
    
    // 
    
  }
  ~X11Core() { send_message(-1); }
};

static X11Core *core = 0;

//--------------------------------------------------



/// A generic Window object
class X11Window: public XWindow {
  ::Window window;
  
  sprint::mutex m_lock;
  
  int m_width, m_height;
  ::XImage *ximage;
  
public:

public:
  X11Window(int width, int height) : m_width(width), m_height(height)
  {
   
  }
  virtual ~X11Window() { }
  
  /// Display an Image
  void Display(const ImageHandle & i) 
  {
    m_lock.lock();
    // TODO
    m_lock.unlock();
    
    XEvent exppp;
    exppp.type = Expose;
    exppp.xexpose.window = window;
    XSendEvent(core->display, window, False,ExposureMask,&exppp);
    XFlush(core->display);
  }
  
  void eventHandler(XEvent *ev)
  {
  }
  
};

void eventProc(XEvent* ev)
{
  X11Window * win = (X11Window*)getProperty(core->display, ev->xany.window, XInternAtom(core->display,"_this",false));
  if (win==0) return;
  win->eventHandler(ev);
}

void X11Core::main_loop()
{
    while(1)
    {
    XEvent ev;
    XNextEvent(display, &ev);
    switch(ev.type)
    {
    case Expose:
      m_lock.lock();
//       if(ximage)
//       {
//         XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 0, 0, m_width, m_height);
//       }
      m_lock.unlock();
        break;
    case ClientMessage:
      // TODO:
      break;
    case ButtonPress:
      // TODO
      break;
    }
    }
}


void init_output_driver()
{
  core = new X11Core;
}

void done_output_driver()
{
  delete core;
}

/// allocate a new @a Window object
XWindow *new_window(const char *title, int width, int height)
{
}


#endif