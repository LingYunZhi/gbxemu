#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
class CEmuGBA; // saves compile time compared to including complete header file
class QTimer;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CEmuGBA *m_emuGBA;
    QString m_fileName;
    bool m_playing; // true: playing  false: paused
    QTimer *m_timer;

private slots:
    void timer_timeout();
    void on_actionPlay_Pause_triggered();
    void on_actionUnload_ROM_triggered();
    void on_actionLoad_ROM_triggered();
};

#endif // MAINWINDOW_H
