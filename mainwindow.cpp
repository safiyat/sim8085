#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QRegExp re("[_a-z*+()]+");
	QRegExpValidator *v = new QRegExpValidator(re);
	ui->input->setValidator(v);

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::process()
{
	QString inputText = ui->input->text();

	QRegExp re("[_a-z]");

	if(inputText.count('(') != inputText.count(')'))
	{
		ui->radioButton->setText("Matching parentheses not found!");
		ui->tableWidget->setColumnCount(ui->tableWidget->columnCount() - 1);
		return;
	}
	if(inputText.contains("*+") || inputText.contains("+*") || inputText.contains("()"))
	{
		ui->radioButton->setText("Operators occuring together!");
		ui->tableWidget->setColumnCount(ui->tableWidget->columnCount() - 1);
		return;
	}
	if(!inputText.contains(re))
	{
		ui->radioButton->setText("NO input string!");
		ui->tableWidget->setColumnCount(ui->tableWidget->columnCount() - 1);
		return;
	}

	ui->radioButton->setText("All's Well!");
	ui->radioButton->toggle();

	QString inputSymbols;
	int i = 0;
	while(i < inputText.length())
	{
		if(inputText.toAscii().at(i) < 97)
		{
			i++;
			continue;
		}
		if(i == 0)
			inputSymbols.append(inputText.toAscii().at(i));
		else
		{
			if (!inputSymbols.contains(inputText.toAscii().at(i)))
				inputSymbols.append(inputText.toAscii().at(i));
		}
		i++;
	}

	QStringList list;

	ui->tableWidget->setColumnCount(inputSymbols.length());

	for(char i = 0; i < inputSymbols.length(); i++)
	{
		list.append(QString::fromLocal8Bit(inputSymbols.toAscii().data() + i, 1));
		ui->tableWidget->setColumnWidth(i, 520 / ui->tableWidget->columnCount());
	}

	ui->tableWidget->setHorizontalHeaderLabels(list);

	list.clear();

	for(int i = 0; i <= ui->tableWidget->rowCount(); i++)
		list.append(tr("q%1").arg(i));
	ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
	ui->tableWidget->setVerticalHeaderLabels(list);

}
