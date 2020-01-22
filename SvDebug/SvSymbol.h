#ifndef SVSYMBOL_H
#define SVSYMBOL_H

#include <QString>
#include <QMap>

struct SvSymbol
  {
    quint32 mAddress;
    quint32 mLenght;

    SvSymbol() : mAddress(0), mLenght(0) {}
  };

using SvSymbolMap = QMap<QString,SvSymbol>;

#endif // SVSYMBOL_H
