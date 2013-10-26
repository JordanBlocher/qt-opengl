#include <QtGui>
#include <QObject>

#include "DockWidget.hpp"

DockWidget::DockWidget(QWidget *parent) : QWidget(parent)
{
    QPushButton *button1 =
         new QPushButton(tr("Change Paddle"));
    button1->setMaximumWidth(100);
    QPushButton *button2 =
         new QPushButton(tr("Change Paddle"));
    button2->setMaximumWidth(100);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(button1);
    layout->addSpacing(this->width() - 100);
    layout->addWidget(button2);

    setLayout(layout);
 
    connect(button1, SIGNAL(clicked()), this, SLOT(player1()));
    connect(button2, SIGNAL(clicked()), this, SLOT(player2()));
}

void DockWidget::getPaddle(int player)
{
      QStringList items;
      items << tr("Circle") << tr("Square");
      bool ok;
      QString item = QInputDialog::getItem(this, tr("Change Paddle"), tr("Paddle Types:"), items, 0, false, &ok);

      if(ok)
          emit updatePaddle(item.toStdString().c_str(), player); 
}

void DockWidget::player1()
{
    this->getPaddle(1);
}

void DockWidget::player2()
{
    this->getPaddle(2);
}
