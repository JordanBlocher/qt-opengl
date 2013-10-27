
#include "DockWidget.hpp"

DockWidget::DockWidget(int width, int height, QWidget *parent) : QWidget(parent), initialSize(width, height)
{
    QPushButton *button1 =
         new QPushButton(tr("Change Paddle"));
    button1->setMaximumWidth(100);
    QPushButton *button2 =
         new QPushButton(tr("Change Paddle"));
    button2->setMaximumWidth(100);
    this->layout = new QHBoxLayout;
    this->layout->addWidget(button1, Qt::AlignLeft);
    this->layout->addStretch(width);
    this->layout->addWidget(button2, Qt::AlignRight);

    setLayout(this->layout);
 
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

QSize DockWidget::sizeHint() const
{
    return initialSize;
}

void DockWidget::player1()
{
    this->getPaddle(1);
}

void DockWidget::player2()
{
    this->getPaddle(2);
}

