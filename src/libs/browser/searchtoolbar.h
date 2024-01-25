// Copyright (C) Oleg Shparber, et al. <https://zealdocs.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ZEAL_BROWSER_SEARCHTOOLBAR_H
#define ZEAL_BROWSER_SEARCHTOOLBAR_H

#include <QWidget>

class QLineEdit;
class QToolButton;
#if 0
class QWebEngineView;
#endif

namespace Zeal::Browser {
class WebView;

class SearchToolBar final : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(SearchToolBar)
public:
#if 0
    explicit SearchToolBar(QWebEngineView *webView, QWidget *parent = nullptr);
#endif
    explicit SearchToolBar(WebView *webView, QWidget *parent = nullptr);
    ~SearchToolBar() override = default;

    void setText(const QString &text);
    void activate();

    bool eventFilter(QObject *object, QEvent *event) override;

protected:
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    void findNext();
    void findPrevious();

    void hideHighlight();

    QLineEdit *m_lineEdit = nullptr;
    QToolButton *m_findNextButton = nullptr;
    QToolButton *m_findPreviousButton = nullptr;
    QToolButton *m_matchCaseButton = nullptr;

    WebView *m_webView = nullptr;
};

} // namespace Zeal::Browser

#endif // ZEAL_BROWSER_SEARCHTOOLBAR_H
