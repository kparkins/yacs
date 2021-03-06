#ifndef YACS_CONFIG_VERSION_H
#define YACS_CONFIG_VERSION_H

#define __PASTE(arg) #arg
#define _PASTE(arg) __PASTE(arg)

#define YACS_VERSION_MAJOR 0
#define YACS_VERSION_MINOR 1
#define YACS_VERSION_PATCH 0

#define YACS_VERSION         \
  _PASTE(YACS_VERSION_MAJOR) \
  "." _PASTE(YACS_VERSION_MINOR) "." _PASTE(YACS_VERSION_PATCH)

#endif