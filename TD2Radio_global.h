#ifndef TD2RADIO_GLOBAL_H
#define TD2RADIO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TD2RADIO_LIBRARY)
#  define TD2RADIO_EXPORT Q_DECL_EXPORT
#else
#  define TD2RADIO_EXPORT Q_DECL_IMPORT
#endif

#endif // TD2RADIO_GLOBAL_H
