#ifndef ZEAL_BROWSER_HISTORY_ITEM_H
#define ZEAL_BROWSER_HISTORY_ITEM_H

#include <QUrl>
#include <QString>

namespace Zeal {
namespace Browser {

struct HistoryItem {
  QUrl url;
  QString title;
  int vScrollPos = 0;
};
}
}

#endif
