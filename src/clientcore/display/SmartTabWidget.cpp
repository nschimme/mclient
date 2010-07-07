#include "SmartTabWidget.h"

#include <QDebug>
#include <QTabBar>

SmartTabWidget::SmartTabWidget(QWidget *parent) : QTabWidget(parent) {
  qDebug() << "* SmartTabWidget created";
}


SmartTabWidget::~SmartTabWidget() {
}


void SmartTabWidget::tabInserted(int /*index*/) {
  (count() <= 1) ? tabBar()->hide() : tabBar()->show(); 
}


void SmartTabWidget::tabRemoved(int /*index*/) {
  (count() <= 1) ? tabBar()->hide() : tabBar()->show(); 
}
