#ifndef _XBOOST_WINDOW_H
#define _XBOOST_WINDOW_H

#include "Image.h"

/// A generic Window object
class XWindow {
public:
  virtual ~XWindow();
  /// Display an Image
  virtual void Display(const ImageHandle & i) = 0;
};

void init_output_driver();

void done_output_driver();

/// allocate a new @a Window object
XWindow *new_window(const char *title, int width, int height);

#endif