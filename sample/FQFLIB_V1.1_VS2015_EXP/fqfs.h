#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_fqfs.h"

class fqfs : public QMainWindow
{
	Q_OBJECT

public:
	fqfs(QWidget *parent = Q_NULLPTR);

private:
	Ui::fqfsClass ui;
};
