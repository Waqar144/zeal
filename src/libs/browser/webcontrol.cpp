/****************************************************************************
**
** Copyright (C) 2015-2016 Oleg Shparber
** Copyright (C) 2013-2014 Jerzy Kozera
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

#include "webcontrol.h"

#include "searchtoolbar.h"
#include "webview.h"

#include <core/networkaccessmanager.h>

#include <QDataStream>
#include <QKeyEvent>
#include <QVBoxLayout>

using namespace Zeal::Browser;

WebControl::WebControl(QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_webView = new WebView();
    setFocusProxy(m_webView);

    connect(m_webView, &WebView::urlChanged, this, [this](const QUrl& url) {
        Q_EMIT titleChanged(m_webView->title());
        Q_EMIT urlChanged(url);
    });
    connect(m_webView, &WebView::urlChanged, this, &WebControl::urlChanged);
    connect(m_webView, &WebView::openLinkInNewTab, this, &WebControl::openLinkInNewTab);

    layout->addWidget(m_webView);

    setLayout(layout);
}

void WebControl::focus()
{
    m_webView->setFocus();
}

int WebControl::zoomLevel() const
{
    return m_webView->zoomLevel();
}

void WebControl::setZoomLevel(int level)
{
    m_webView->setZoomLevel(level);
}

void WebControl::zoomIn()
{
    m_webView->zoomIn();
}

void WebControl::zoomOut()
{
    m_webView->zoomOut();
}

void WebControl::resetZoom()
{
    m_webView->resetZoom();
}

void WebControl::setJavaScriptEnabled(bool)
{
}

void WebControl::setWebBridgeObject(const QString& name, QObject* object)
{
    // QWebEnginePage *page = m_webView->page();
    //
    // QWebChannel *channel = new QWebChannel(page);
    // channel->registerObject(name, object);
    //
    // page->setWebChannel(channel);
}

void WebControl::load(const QUrl& url)
{
    m_webView->load(url);
}

void WebControl::activateSearchBar()
{
    if (m_searchToolBar == nullptr) {
        m_searchToolBar = new SearchToolBar(m_webView);
        layout()->addWidget(m_searchToolBar);
    }

    const auto text = m_webView->selectedText().simplified();
    if (!text.isEmpty()) {
        if (!text.isEmpty()) {
            m_searchToolBar->setText(text);
        }
    }

    m_searchToolBar->activate();
}

void WebControl::back()
{
    m_webView->back();
}

void WebControl::forward()
{
    m_webView->forward();
}

bool WebControl::canGoBack() const
{
    return m_webView->canGoBack();
}

bool WebControl::canGoForward() const
{
    return m_webView->canGoForward();
}

QString WebControl::title() const
{
    return m_webView->title();
}

QUrl WebControl::url() const
{
    return m_webView->url();
}

void WebControl::restoreHistory(const QByteArray& array)
{
    // QDataStream stream(array);
    // stream >> *m_webView->history();
}

QByteArray WebControl::saveHistory() const
{
    QByteArray array;
    // QDataStream stream(&array, QIODevice::WriteOnly);
    // stream << *m_webView->history();
    return array;
}

const std::vector<HistoryItem> &WebControl::backHistory() const
{
    return m_webView->backHistoryItems();
}

const std::vector<HistoryItem> &WebControl::forwardHistory() const
{
    return m_webView->forwardHistoryItems();
}

void WebControl::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Slash:
        activateSearchBar();
        break;
    default:
        event->ignore();
        break;
    }
}
