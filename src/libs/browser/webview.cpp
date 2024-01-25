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

#include "webview.h"

#include "webcontrol.h"

#include <core/application.h>
#include <core/settings.h>
#include <ui/browsertab.h>
#include <ui/mainwindow.h>
#include <core/networkaccessmanager.h>
#include <core/application.h>
#include <core/httpserver.h>

#include <QClipboard>
#include <QApplication>
#include <QCheckBox>
#include <QDesktopServices>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QVector>
#include <QScrollBar>
#include <QNetworkAccessManager>
#include <QWheelEvent>
#include <QNetworkReply>
#include<QElapsedTimer>

constexpr size_t s_maxHistoryItems = 15;

using namespace Zeal::Browser;

WebView::WebView(QWidget *parent)
    : QLiteHtmlWidget(parent)
    , m_nam(new QNetworkAccessManager(this))
{
    auto callback = [this](const QUrl &url) { return resourceLoadCallBack(url); };
    setResourceHandler(callback);

    setZoomLevel(defaultZoomLevel());

    // Make docs' contents visible in dark theme
    QPalette p = palette();
    p.setColor(QPalette::Inactive, QPalette::Highlight,
        p.color(QPalette::Active, QPalette::Highlight));
    p.setColor(QPalette::Inactive, QPalette::HighlightedText,
        p.color(QPalette::Active, QPalette::HighlightedText));
    p.setColor(QPalette::Base, Qt::white);
    p.setColor(QPalette::Text, Qt::black);
    setPalette(p);

    connect(this, &QLiteHtmlWidget::linkClicked, this, &WebView::load);
    connect(this, &QLiteHtmlWidget::contextMenuRequested, this, &WebView::onContextMenuRequested);
}

int WebView::zoomLevel() const
{
    return m_zoomLevel;
}

void WebView::setZoomLevel(int level)
{
    if (level == m_zoomLevel) {
        return;
    }

    level = qMax(0, level);
    level = qMin(level, availableZoomLevels().size() - 1);

    m_zoomLevel = level;

    // Scale the webview relative to the DPI of the screen.
    const double dpiZoomFactor = logicalDpiY() / 96.0;

    setZoomFactor(availableZoomLevels().at(level) / 100.0 * dpiZoomFactor);
    emit zoomLevelChanged();
}

const QVector<int> &WebView::availableZoomLevels()
{
    static const QVector<int> zoomLevels = {30, 40, 50, 67, 80, 90, 100,
                                            110, 120, 133, 150, 170, 200,
                                            220, 233, 250, 270, 285, 300};
    return zoomLevels;
}

int WebView::defaultZoomLevel()
{
    static const int level = availableZoomLevels().indexOf(100);
    return level;
}

void WebView::zoomIn()
{
    setZoomLevel(m_zoomLevel + 1);
}

void WebView::zoomOut()
{
    setZoomLevel(m_zoomLevel - 1);
}

void WebView::resetZoom()
{
    setZoomLevel(defaultZoomLevel());
}

void WebView::onContextMenuRequested(QPoint pos, const QUrl &linkUrl)
{
    QMenu menu(this);

    if (linkUrl.isValid()) {
        const QString scheme = linkUrl.scheme();

        if (scheme != QLatin1String("javascript")) {
            menu.addAction(tr("Open Link in New Tab"), this, [this, linkUrl]() {
                Q_EMIT openLinkInNewTab(linkUrl);
            });
        }

        if (scheme != QLatin1String("qrc")) {
            if (scheme != QLatin1String("javascript")) {
                menu.addAction(tr("Open Link in Desktop Browser"), this, [linkUrl]() {
                    QDesktopServices::openUrl(linkUrl);
                });
            }

            menu.addAction(tr("Copy Link"), this, [linkUrl]() {
                QApplication::clipboard()->setText(linkUrl.toString());
            });
        }
    }

    const QString selectedText = this->selectedText();

    if (!selectedText.isEmpty()) {
        if (!menu.isEmpty()) {
            menu.addSeparator();
        }
        menu.addAction(tr("Copy"), this, [selectedText]() {
            QApplication::clipboard()->setText(selectedText);
        });
    }

    if (!linkUrl.isValid() && url().scheme() != QLatin1String("qrc")) {
        if (!menu.isEmpty()) {
            menu.addSeparator();
        }

        auto b = menu.addAction(tr("Back"), this, &WebView::back);
        b->setEnabled(canGoBack());
        auto f = menu.addAction(tr("Forward"), this, &WebView::forward);
        f->setEnabled(canGoForward());
        menu.addSeparator();

        menu.addAction(tr("Open Page in Desktop Browser"), this, [this]() {
            QDesktopServices::openUrl(url());
        });
    }

    if (menu.isEmpty()) {
        return;
    }

    menu.exec(mapToGlobal(pos));
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
    case Qt::BackButton:
        back();
        event->accept();
        return;

    case Qt::ForwardButton:
        forward();
        event->accept();
        return;

    default:
        break;
    }
    return QLiteHtmlWidget::mousePressEvent(event);
}

void WebView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        const QPoint angleDelta = event->angleDelta();
        int delta = qAbs(angleDelta.x()) > qAbs(angleDelta.y()) ? angleDelta.x() : angleDelta.y();
        const int direction = delta > 0 ? 1 : -1;

        int levelDelta = 0;
        while (delta * direction >= 120) {
            levelDelta += direction;
            delta -= 120 * direction;
        }

        setZoomLevel(m_zoomLevel + levelDelta);
        event->accept();
        return ;
    }

    return QLiteHtmlWidget::wheelEvent(event);
}

QByteArray WebView::resourceLoadCallBack(const QUrl &url) {
    if (!url.isValid()) return {};

    QEventLoop loop;
    QByteArray data;

    QNetworkReply *reply = m_nam->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [&data, &loop, reply] {
        if (reply->error() == QNetworkReply::NoError) data = reply->readAll();
        reply->deleteLater();
        loop.exit();
    });
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    return data;
}

void WebView::load(const QUrl &url)
{
    if (url == this->url()) {
        return;
    }

    // add current pos to history
    if (this->url().isValid()) {
        auto curr = currentHistoryItem();
        m_historyBack.push_back(curr);
        // ensure we dont cross limits
        while (m_historyBack.size() > s_maxHistoryItems)
            m_historyBack.erase(m_historyBack.begin());
    }

    QUrl u = url;
    u.setFragment({});

    // only load html if url is different after removing fragment
    if (u != this->url()) {
        QElapsedTimer t;
        t.start();
        auto data = resourceLoadCallBack(url);
        setUrl(u);
        setHtml(QString::fromUtf8(data));
        qDebug() << url << "loaded in:" << t.elapsed();
        Q_EMIT urlChanged(u);
    }

    if (url.hasFragment()) {
        scrollToAnchor(url.fragment(QUrl::FullyEncoded));
    }
}

HistoryItem WebView::currentHistoryItem() const
{
    return HistoryItem {this->url(), this->title(), this->verticalScrollBar()->value()};
}

bool WebView::canGoBack()
{
    return !m_historyBack.empty();
}

bool WebView::canGoForward()
{
    return !m_historyForward.empty();
}

void WebView::back()
{
    if (m_historyBack.empty()) {
        return;
    }

    auto curr = currentHistoryItem();

    m_historyForward.insert(m_historyForward.begin(), curr);
    curr = m_historyBack.back();
    m_historyBack.pop_back();
    load(curr.url);
    if (curr.vScrollPos > 0) {
        verticalScrollBar()->setValue(curr.vScrollPos);
    }
}

void WebView::forward()
{
    if (m_historyForward.empty()) {
        return;
    }

    auto curr = currentHistoryItem();

    m_historyBack.push_back(curr);
    curr = m_historyForward.front();
    m_historyForward.erase(m_historyForward.begin());

    load(curr.url);
    if (curr.vScrollPos > 0) {
        verticalScrollBar()->setValue(curr.vScrollPos);
    }
}
