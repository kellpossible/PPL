Plugin Patterns Library
=======================

The Plugin Patterns Library provides classes for common tasks with the XPLM SDK for the X-Plane flight simulator. It is provided under BSD license. Questions, support and pull request for the original PPL are handled through http://forums.x-plane.org/index.php?showtopic=49958 

This fork is intended to allow PPL to be used as a header-only library, using the
pattern described here: http://http://stackoverflow.com/questions/25606736.

At this point, these PPL libraries will work in header-only mode:

PPL::DataRef
PPL::OwnedData
PPL::Log
PPL::LogWriter
PPL::PluginPath

To use these libraries in header-only mode, `#define LIBRARY_INLINE` before
including them:

```c++
//main.cpp
#define PRIVATENAMESPACE YourNamespaceHere
#define INLINE_LIBRARY

#include "dataref.h"
#include "log.h"
...
```

PRIVATENAMESPACE should be a string, unique to your project. I understand this
is necessary for 32-bit Windows plugins.

Included here is a demonstration plugin, which uses the PPL classes and SimpleIni
to create a plugin which retracts your flaps based on a value you can set in an
ini file.

Todo:
=====

[ ] Fix OwnedData<vector<float>> implementation
[ ] Add #ifdef INLINE_LIBRARY, #error to the other classes