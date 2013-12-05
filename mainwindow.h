#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidgetItem>

//#include "instructionset.h"


#ifdef byte
#undef byte
#endif

#ifdef word
#undef word
#endif

#ifdef reg
#undef reg
#endif

#define word quint16

#define byte quint8

#define reg short

/******************ERROR CODES***********************/
#define OK 0
#define UNKOWN_ERROR -1
#define INVALID_INSTRUCTION -2;
#define INVALID_REGISTER -3;
#define INVALID_REGISTER_PAIR -4;
#define INVALID_LABEL -5;
#define INVALID_BYTE -6;
#define INVALID_WORD -7;
#define INVALID_OPERAND -8;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
//	enum reg { A=1, B, C, D, E, H, L};
	QString &extractWord(const QString &string, int from, QString &word);

public slots:
	void decChanged();
	void hexChanged();
	void SAChanged();
	void memChanged(QTableWidgetItem *item);
	void memChanged(int row, int col);
	void parse();
	void capitalizeCode();

private:
	Ui::MainWindow *ui;
	byte memory[65536];
	byte AReg, BReg, CReg, DReg, EReg, HReg, LReg;
	word PC, SP;
	bool S, Z, Ac, P, CY;
	int SAstatus;
	int rowCount;

	void initializeSymbols();
	int interpret(QString instruction, int index);
	reg identifyReg(QString regName);
//	int identifyReg(QString regName);
	int arity(QString instruction);
	void updateListing();

	int NOP();
	int LXI(reg , word);
	int STAX(reg);

	QString instructions;
	QString registers;
	QString labels;
};

#endif // MAINWINDOW_H
