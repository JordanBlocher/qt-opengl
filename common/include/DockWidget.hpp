#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QWidget>
#include <QObject>
#include <QtGui>

class QCheckBox;
class QLabel;
class QErrorMessage;

class DockWidget : public QWidget
{
    Q_OBJECT

 public:
    DockWidget(int, int, QWidget *parent = 0);

    QSize sizeHint() const;

 protected slots:
    void getPaddle(int);
    void player1();
    void player2();

 signals:
    void updatePaddle(const char*, int);

 private:
    QSize initialSize;

    QHBoxLayout *layout;
};

#endif
