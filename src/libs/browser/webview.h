// Copyright (C) Oleg Shparber, et al. <https://zealdocs.org>
// Copyright (C) 2013-2014 Jerzy Kozera
// Copyright (C) 2024 Waqar Ahmed
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ZEAL_BROWSER_WEBVIEW_H
#define ZEAL_BROWSER_WEBVIEW_H

#include <3rdparty/qlitehtml/src/qlitehtmlwidget.h>
#include "historyitem.h"

class QNetworkAccessManager;
class QMenu;

namespace Zeal::Browser {

class WebView final : public QLiteHtmlWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(WebView)
public:
    explicit WebView(QWidget *parent = nullptr);
    ~WebView() override = default;

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

} // namespace Zeal::Browser

#endif // ZEAL_BROWSER_WEBVIEW_H
