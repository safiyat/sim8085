#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QRegExp re("[a-z*+()]+");
	QRegExpValidator *v = new QRegExpValidator(re);
	ui->input->setValidator(v);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::process()
{
//    QString inputText = ui->input->text();

	ui->tableWidget->setColumnCount(0);
	ui->tableWidget->setRowCount(0);
	ui->statusBar->clearMessage();

	QRegExp re("[_a-z]");

	if(!ui->input->text().contains(re))
	{
		ui->statusBar->showMessage("No input string");
		ui->tableWidget->setColumnCount(0);
		ui->tableWidget->setRowCount(0);
		return;
	}

	QByteArray text = ui->input->text().toAscii();
	int i, j, parenthesis, alcount, k, pointer;
	i = parenthesis = alcount = 0;

	if(text.contains("*+") || text.contains("+*"))
	{
		ui->statusBar->showMessage("Operators occuring together");
		ui->tableWidget->setColumnCount(0);
		ui->tableWidget->setRowCount(0);
		return;
	}

	while( i < text.length())
	{
		if(text.at(i) == '(')
			parenthesis++;
		else if(text.at(i) == ')')
			parenthesis--;
		if(parenthesis < 0 || text.contains("()"))
		{
			ui->statusBar->showMessage("Parentheses are not used correctly");
			ui->tableWidget->setColumnCount(0);
			ui->tableWidget->setRowCount(0);
			return;
		}

		if(isalpha(text.at(i)))
			alcount++;
		else if((text.at(i) == '*') || (text.at(i) == '+'))
		{
			if(alcount == 0)
			{
				ui->statusBar->showMessage("Operators occur before input symbols");
				ui->tableWidget->setColumnCount(0);
				ui->tableWidget->setRowCount(0);
				return;
			}
			if(text.at(i - 1) != ')' && !isalpha(text.at(i - 1)))
			{
				ui->statusBar->showMessage("Operators occur against no input symbols");
				ui->tableWidget->setColumnCount(0);
				ui->tableWidget->setRowCount(0);
				return;
			}
		}
		i++;
	}

	if(alcount == 0 || parenthesis != 0)
	{
		ui->statusBar->showMessage("No input symbols OR incorrect usage of parentheses");
		ui->tableWidget->setColumnCount(0);
		ui->tableWidget->setRowCount(0);
		return;
	}

	QString inputSymbols;
	i = 0;
	while(i < text.length())
	{
		if(text.at(i) < 97)
		{
			i++;
			continue;
		}
		if(i == 0)
			inputSymbols.append(text.at(i));
		else
		{
			if (!inputSymbols.contains(text.at(i)))
				inputSymbols.append(text.at(i));
		}
		i++;
	}

	QStringList list;

	ui->tableWidget->setColumnCount(inputSymbols.length() + 1);

	list.append(trUtf8("ε"));

	for(char i = 0; i < inputSymbols.length(); i++)
	{
		list.append(QString::fromLocal8Bit(inputSymbols.toAscii().data() + i, 1));
		ui->tableWidget->setColumnWidth(i, 500 / ui->tableWidget->columnCount());
	}

	ui->tableWidget->setHorizontalHeaderLabels(list);

	list.clear();

/********************************************************************/
	int row = 0;
	for( i = j = 0; i < text.length(); i++)
	{
		if(text.at(i) > 96)
		{
			ui->tableWidget->setRowCount(++row);
			QTableWidgetItem *item;
			item = new QTableWidgetItem(tr("q%1").arg(j + 1 /*row*/));
			j++;
/********************************************************************/
			ui->tableWidget->setItem(row - 1, inputSymbols.indexOf(text.at(i)) + 1, item);
/********************************************************************/

		}
		else if (text.at(i) == '*')
		{
			k = i - 2;
			int charcount = 1;
			if(text.at(i - 1) == ')')
			{
				parenthesis = 1;
				k = i - 2;
				charcount = 0;
				while(parenthesis)
				{
					if(text.at(k) == '(')
						parenthesis--;
					else if(text.at(k) == ')')
						parenthesis++;
					else if(text.at(k) > 96)
						charcount++;
					k--;
				}
			}
			if(text.at(k) == '*' || text.at(k) == '+')
			{
				ui->tableWidget->insertRow(row++ - charcount);
				ui->tableWidget->setItem(row - (charcount + 1), 0, new QTableWidgetItem(tr("q%1, q%2").arg(row - (charcount + 2)).arg(row - charcount)));
				ui->tableWidget->takeItem(row - charcount, 0);

			}
			pointer = row - 1;
			if(text.at(i - 1) == ')')
			{
				parenthesis = 1;
				k = i - 2;
				while(parenthesis)
				{
					if(text.at(k) == '(')
						parenthesis--;
					else if(text.at(k) == ')')
						parenthesis++;
					else if(text.at(k) > 96)
						pointer--;
					k--;
				}
				pointer++;
			}

			ui->tableWidget->setRowCount(++row);
			if(ui->tableWidget->item(row - 1, 0) == 0)//Second Entry, loopback entry
				ui->tableWidget->setItem(row - 1, 0, new QTableWidgetItem(tr("q%1").arg(pointer)));
			else
				ui->tableWidget->setItem(row - 1, 0, new QTableWidgetItem(ui->tableWidget->item(row - 1, 0)->text().append(tr(", q%1").arg(pointer))));

			if(ui->tableWidget->item(pointer, 0) == 0)//First Entry, skipping entry
				ui->tableWidget->setItem(pointer, 0, new QTableWidgetItem(tr("q%1").arg(row - 1)));
			else
				ui->tableWidget->setItem(pointer, 0, new QTableWidgetItem(ui->tableWidget->item(pointer, 0)->text().append(tr(", q%1").arg(row - 1))));

			row--;
		}
		else if (text.at(i) == '+')
		{

			k = i - 2;
			int charcount = 1;
			if(text.at(i - 1) == ')')
			{
				parenthesis = 1;
				k = i - 2;
				charcount = 0;
				while(parenthesis)
				{
					if(text.at(k) == '(')
						parenthesis--;
					else if(text.at(k) == ')')
						parenthesis++;
					else if(text.at(k) > 96)
						charcount++;
					k--;
				}

			}
			if(text.at(k) == '*' || text.at(k) == '+')
			{
				ui->tableWidget->insertRow(row++ - charcount);
				ui->tableWidget->setItem(row - (charcount + 1), 0, new QTableWidgetItem(tr("q%1, q%2").arg(row - (charcount + 2)).arg(row - charcount)));
				ui->tableWidget->takeItem(row - charcount, 0);

			}
			pointer = row - 1;
			if(text.at(i - 1) == ')')
			{
				parenthesis = 1;
				k = i - 2;
				while(parenthesis)
				{
					if(text.at(k) == '(')
						parenthesis--;
					else if(text.at(k) == ')')
						parenthesis++;
					else if(text.at(k) > 96)
						pointer--;
					k--;
				}
				pointer++;
			}

			ui->tableWidget->setRowCount(++row);
			if(ui->tableWidget->item(row - 1, 0) == 0)//Second Entry, loopback entry
				ui->tableWidget->setItem(row - 1, 0, new QTableWidgetItem(tr("q%1").arg(pointer)));
			else
				ui->tableWidget->setItem(row - 1, 0, new QTableWidgetItem(ui->tableWidget->item(row - 1, 0)->text().append(tr(", q%1").arg(pointer))));

			row--;
		}
	}


	ui->tableWidget->setRowCount(++row);

	for( i = 0; i < ui->tableWidget->rowCount(); i++)
		list.append(tr("q%1").arg(i));
	ui->tableWidget->setVerticalHeaderLabels(list);
	for(int r = 0; r < row; r++)
	{
		for(int c = 1; c < ui->tableWidget->columnCount(); c++)
		{
			if(ui->tableWidget->item(r, c) != 0)
			{
				ui->tableWidget->setItem(r, c, new QTableWidgetItem(tr("q%1").arg(r+1)));
			}
		}
	}
	ui->tableWidget->setColumnWidth(ui->tableWidget->columnCount() - 1, ui->tableWidget->columnWidth(1) - 2);
}

void MainWindow::clearTable()
{
	ui->tableWidget->setColumnCount(0);
	ui->tableWidget->setRowCount(0);
	ui->statusBar->clearMessage();
}

/*
  a*bcd*(er)*q*y
*/
