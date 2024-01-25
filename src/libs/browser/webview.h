/****************************************************************************
**
** Copyright (C) 2015-2016 Oleg Shparber
** Copyright (C) 2013-2014 Jerzy Kozera
** Copyright (C) 2024 Waqar Ahmed
** Contact: https://go.zealdocs.org/l/contact
**
** This file is part of Zeal.
**
** Zeal is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Zeal is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Zeal. If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef ZEAL_BROWSER_WEBVIEW_H
#define ZEAL_BROWSER_WEBVIEW_H

#include <3rdparty/qlitehtml/src/qlitehtmlwidget.h>
#include "historyitem.h"

class QNetworkAccessManager;
class QMenu;

namespace Zeal {
namespace Browser {

class WebView final : public QLiteHtmlWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(WebView)
public:
    explicit WebView(QWidget *parent = nullptr);

    int zoomLevel() const;
    void setZoomLevel(int level);

    void load(const QUrl &url);

    static const QVector<int> &availableZoomLevels();
    static int defaultZoomLevel();

    bool canGoBack();
    bool canGoForward();

    void back();
    void forward();

    const std::vector<HistoryItem> &backHistoryItems() const
    {
        return m_historyBack;
    }

    const std::vector<HistoryItem> &forwardHistoryItems() const
    {
        return m_historyForward;
    }

public slots:
    void zoomIn();
    void zoomOut();
    void resetZoom();

signals:
    void zoomLevelChanged();
    void openLinkInNewTab(const QUrl &url);
    void urlChanged(const QUrl &url);

protected:
    void onContextMenuRequested(QPoint pos, const QUrl &url);

private:
    HistoryItem currentHistoryItem() const;
    QByteArray resourceLoadCallBack(const QUrl &url);
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    QNetworkAccessManager *m_nam = nullptr;
    QUrl m_clickedLink;
    int m_zoomLevel = 0;
    std::vector<HistoryItem> m_historyBack;
    std::vector<HistoryItem> m_historyForward;
};

} // namespace Browser
} // namespace Zeal

#endif // ZEAL_BROWSER_WEBVIEW_H
