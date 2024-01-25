// Copyright (C) Oleg Shparber, et al. <https://zealdocs.org>
// Copyright (C) 2013-2014 Jerzy Kozera
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ZEAL_BROWSER_WEBCONTROL_H
#define ZEAL_BROWSER_WEBCONTROL_H

#include <QWidget>
#include "historyitem.h"

class QWebEngineHistory;

namespace Zeal::Browser {

class SearchToolBar;
class WebView;

class WebControl final : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(WebControl)
public:
    explicit WebControl(QWidget *parent = nullptr);
    ~WebControl() override = default;

    void focus();
    void load(const QUrl &url);
    bool canGoBack() const;
    bool canGoForward() const;

    QString title() const;
    QUrl url() const;

    void restoreHistory(const QByteArray &array);
    QByteArray saveHistory() const;
    const std::vector<HistoryItem> &backHistory() const;
    const std::vector<HistoryItem> &forwardHistory() const;

    int zoomLevel() const;
    void setZoomLevel(int level);
    void setJavaScriptEnabled(bool enabled);

    void setWebBridgeObject(const QString &name, QObject *object);

signals:
    void titleChanged(const QString &title);
    void urlChanged(const QUrl &url);
    void openLinkInNewTab(const QUrl &url);

public slots:
    void activateSearchBar();
    void back();
    void forward();

    void zoomIn();
    void zoomOut();
    void resetZoom();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    friend class WebView;

    WebView *m_webView = nullptr;
    SearchToolBar *m_searchToolBar = nullptr;
};

} // namespace Zeal::Browser

#endif // ZEAL_BROWSER_WEBCONTROL_H
