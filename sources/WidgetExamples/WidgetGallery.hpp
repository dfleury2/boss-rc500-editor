#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QGroupBox;
class QProgressBar;
class QTabWidget;
class QTextBrowser;
class QToolBox;
QT_END_NAMESPACE

class Q_DECL_EXPORT WidgetGallery : public QDialog {
    Q_OBJECT

public:
    explicit WidgetGallery(QWidget* parent = nullptr);
    void setVisible(bool visible) override;

private slots:
    void changeStyle(const QString& styleName);
    void advanceProgressBar();
    void helpOnCurrentWidget();
    void updateSystemInfo();

private:
    static QGroupBox* createButtonsGroupBox();
    static QTabWidget* createItemViewTabWidget();
    static QGroupBox* createSimpleInputWidgetsGroupBox();

    QToolBox* createTextToolBox();
    QProgressBar* createProgressBar();

    QProgressBar* progressBar;
    QTextBrowser* systemInfoTextBrowser;
};
