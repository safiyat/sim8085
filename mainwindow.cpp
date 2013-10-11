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
//    QString inputText = ui->input->text();

    QRegExp re("[_a-z]");

    if(!ui->input->text().contains(re))
    {
        ui->radioButton->setText("NO input string!");
        ui->tableWidget->setColumnCount(ui->tableWidget->columnCount() - 1);
        return;
    }

    QByteArray text = ui->input->text().toAscii();
    int i, parenthesis, alcount;
    i = parenthesis = alcount = 0;

    if(text.contains("*+") || text.contains("+*") || text.contains("()"))
        return;

    while( i < text.length())
    {
        if(text.at(i) == '(')
            parenthesis++;
        else if(text.at(i) == ')')
            parenthesis--;
        if(parenthesis < 0)
            return;

        if(isalpha(text.at(i)))
            alcount++;
        else if((text.at(i) == '*') || (text.at(i) == '+'))
        {
            if(alcount == 0)
                return;
            if(text.at(i - 1) != ')' && !isalpha(text.at(i - 1)))
                return;
        }
        i++;
    }
    if(alcount == 0 || parenthesis != 0)
        return;
    ui->radioButton->setText("All's Well!");
    ui->radioButton->toggle();

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
