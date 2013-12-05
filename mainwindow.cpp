#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	initializeSymbols();

	rowCount = 10;

	SAstatus = 0;

	QIntValidator *intval = new QIntValidator(0, 65535, this);

	QRegExp re("[A-Fa-f0-9]+");
	QRegExpValidator *hexval = new QRegExpValidator(re);

	ui->lineEditDec->setValidator(intval);
	ui->lineEditHex->setValidator(hexval);
	ui->lineEditSA->setValidator(hexval);

	ui->tableWidget->setRowCount(rowCount);

	MainWindow::SAChanged();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::initializeSymbols()
{
	instructions = " NOP LXI STAX INX INR DCR MVI RLC XTHL DAD LDAX DCX RRC PCHL RAL RAR SHLD DAA LHLD CMA STA STC LDA CMC MOV HLT ADD ADC SUB SBB ANA XRA ORA CMP RNZ POP JNZ JMP CNZ PUSH ADI RST RZ RET JZ CZ CALL ACI RNC JNC CNC SUI RC IN SBI RPO POP JPO CPO ANI RPE PCHL JPE XCHG CPE XRI RP JP CP ORI RM SPHL JM CM CPI ";

	registers = " A B C D E H L M PC SP ";

	labels = " ";

	S = Z = Ac = P = CY = 0;

	AReg = BReg = CReg = DReg = EReg = HReg = LReg = PC = SP = 0;

	for(int i = 0; i < 65536; i++)
		memory[i] = 0;

	memory[6432] = memory[25] = 20;
//	memory[6433] = 20;
}

void MainWindow::decChanged()
{
	uint num = ui->lineEditDec->text().toUInt();
	QString hex;
	hex.setNum(num, 16);
	ui->lineEditHex->setText(hex.toUpper());
}

void MainWindow::hexChanged()
{
	bool ok;
	int pos = ui->lineEditHex->cursorPosition();
	ui->lineEditHex->setText(ui->lineEditHex->text().toUpper());
	ui->lineEditHex->setCursorPosition(pos);
	ui->lineEditDec->setText(tr("%1").arg(ui->lineEditHex->text().toUInt(&ok, 16)));
}

void MainWindow::SAChanged()
{
	SAstatus = 1;
	ui->lineEditSA->setText(ui->lineEditSA->text().toUpper());
	if(rowCount < 10)
	{
		rowCount = 10;
		ui->tableWidget->setRowCount(rowCount);
	}
	bool ok;
	QStringList rowHeaders;
	QString header;
	int initVal = ui->lineEditSA->text().toUInt(&ok, 16);

	for(int i = 0; i < rowCount; i++)
	{
		header.setNum(initVal + i, 16);
		if(initVal + i > 65535)
		{
			rowCount = i;
			ui->tableWidget->setRowCount(rowCount);
			break;
		}
		rowHeaders.append(header.toUpper());
		ui->tableWidget->setItem(i, 0, new QTableWidgetItem(tr("%1").arg(memory[initVal + i])));
	}
	ui->tableWidget->setVerticalHeaderLabels(rowHeaders);

	SAstatus = 0;
}

void MainWindow::memChanged(QTableWidgetItem *item)
{
	if(SAstatus)
		return;
	bool ok;
	int row = item->row();
	int col = item->column();
	int index = ui->tableWidget->verticalHeaderItem(row)->text().toUInt(&ok, 16);
	memory[index] = ui->tableWidget->item(row, col)->text().toUShort();
	ui->tableWidget->item(row, col)->setText(tr("%1").arg(memory[index]));
}

void MainWindow::memChanged(int row, int col)
{
	if(SAstatus)
		return;
	bool ok;
	int index = ui->tableWidget->verticalHeaderItem(row)->text().toUInt(&ok, 16);
	memory[index] = ui->tableWidget->item(row, col)->text().toUShort();
	ui->tableWidget->item(row, col)->setText(tr("%1").arg(memory[index]));
}

QString & MainWindow::extractWord(const QString &string, int from, QString &word)
{
	word = " ";
/*
	if(string.at(from).isDigit())
	{
		while(string.at(from).isLetterOrNumber())
		{
			word.append(string.at(from));
			from++;
		}
	}
	else
*/
	if(string.at(from) == ';')
	{
		word.append(";");
		from++;
		while(string.at(from) != '\n')
		{
			word.append(string.at(from));
			from++;
			if(from == string.length())
				break;
		}
	}
	else
		while(string.at(from).isLetterOrNumber()/* && from < string.length()*/)
		{
			word.append(string.at(from));
			from++;
			if(from == string.length())
				break;
		}
	if(string.at(from) == ':')
		word.append(":");
	word.append(" ");
//	QMessageBox::information(this, tr("extractWord()"), tr("Returning word: \"%1\" ").arg(word));
	return word;
}

void MainWindow::parse()
{
	QString code = ui->textEdit->toPlainText();

//	QMessageBox::information(this, tr("Parse()"), tr("Beginning parse"));

	int index = 0;
	while (index < code.length())
	{
//		QMessageBox::information(this, tr("Parse()"), tr("In the loop"));
		QString word;

		(extractWord(code, index, word));
//		QMessageBox::information(this, tr("Parse() got the word"), tr("%1").arg(word));
		if(word.length() == 2)
		{
			while (!(code.at(index).isLetterOrNumber() || (code.at(index) == ';')))
			{
				index++;
				if(index == code.length())
					break;
			}
			continue;
		}
		else
			index += word.length() - 1;

		QString type = " ";

		if(word.at(1) == ';')
			type = "comment";
		else if(instructions.contains(word, Qt::CaseInsensitive))
		{
			type.append(tr("instruction with arity %1").arg(arity(word)));
			interpret(word, index);


			updateListing();
		}
		else if(registers.contains(word, Qt::CaseInsensitive))
		{
			type = "register";
		}
		else if(word.at(word.length() - 2) == 'H' || word.at(word.length() - 1) == 'H')
			type = "number";
		else if(word.at(word.length() - 2) == ':' || labels.contains(word))
		{
			if(!labels.contains(word))
				labels.append(tr("%1").arg(word.left(word.length() - 2)));
			type = "label";
		}
		else
			type = "unknown";

//		QMessageBox::information(this, tr("Word found"), tr("\"%1\"\n%2").arg(word).arg(type));
	}
	ui->statusBar->showMessage(tr("Labels: %1").arg(labels));
}

int MainWindow::arity(QString instruction)
{
	QString arity0 = " NOP RLC XTHL RRC PCHL RAL RAR DAA CMA STC CMC HLT RNZ RZ RET RNC RC RPO RPE PCHL XCHG RP RM SPHL ";
	QString arity1 = " STAX INX INR DCR DAD LDAX DCX SHLD LHLD STA LDA ADD ADC SUB SBB ANA XRA ORA CMP POP JNZ JMP CNZ PUSH ADI RST JZ CZ CALL ACI JNC CNC SUI IN SBI POP JPO CPO ANI JPE CPE XRI JP CP ORI JM CM CPI ";
	QString arity2 = " LXI MVI MOV ";

	if(arity0.contains(instruction))
		return 0;
	if(arity1.contains(instruction))
		return 1;
	if(arity2.contains(instruction))
		return 2;
	else
		return -1;
}


int MainWindow::interpret(QString instruction, int index)
{
	QMessageBox::information(this, tr("Interpret()"), tr("Interpreting the instruction %1").arg(instruction));
	switch(arity(instruction))
	{
		case 0:
		{
			if(instruction == " NOP ")
			{
				return OK;
			}

			if(instruction == " RLC ")
			{

			}

			if(instruction == " XTHL ")
			{
			}

			if(instruction == " RRC ")
			{
			}

			if(instruction == " PCHL ")
			{
			}

			if(instruction == " RAL ")
			{
			}

			if(instruction == " RAR ")
			{
			}

			if(instruction == " DAA ")
			{
			}

			if(instruction == " CMA ")
			{
				AReg = ~AReg;
			}

			if(instruction == " STC ")
			{
				CY = 1;
			}

			if(instruction == " CMC ")
			{
				CY = !CY;
			}

			if(instruction == " HLT ")
			{
			}

			if(instruction == " RNZ ")
			{
			}

			if(instruction == " RZ ")
			{
			}

			if(instruction == " RET ")
			{
			}

			if(instruction == " RNC ")
			{
			}

			if(instruction == " RC ")
			{
			}

			if(instruction == " RPO ")
			{
			}

			if(instruction == " RPE ")
			{
			}

			if(instruction == " PCHL ")
			{
			}

			if(instruction == " XCHG ")
			{
			}

			if(instruction == " RP ")
			{
			}

			if(instruction == " RM ")
			{
			}

			if(instruction == " SPHL ")
			{
			}
		}

		case 1:
		{
			if(instruction == " STAX ")
			{
			}

			if(instruction == " INX ")
			{
				QString operand;
				extractWord(ui->textEdit->toPlainText(), index, operand);
				if(operand == " B ")
				{
					CReg++;
					if(!CReg)
						BReg++;
					return OK;
				}
				else if(operand == " D ")
				{
					EReg++;
					if(!EReg)
						DReg++;
					return OK;
				}
				else if(operand == " H ")
				{
					LReg++;
					if(!LReg)
						HReg++;
					return OK;
				}
				else
					return INVALID_REGISTER_PAIR;
			}

			if(instruction == " INR ")
			{
//				QMessageBox::information(this, tr("Interpret()::if"), tr("Ïnterpreting the instruction INR"));
				QString operand;
				extractWord(ui->textEdit->toPlainText(), index, operand);
				ui->statusBar->showMessage(tr("\"%1\"").arg(operand));
//				QMessageBox::information(this, tr("Interpret()::if"), tr("Here?"));
				if(operand == " A ")
				{
					AReg++;
				}
				else if(operand == " B ")
				{
					BReg++;
				}
				else if(operand == " C ")
				{
					CReg++;
				}
				else if(operand == " D ")
				{
					DReg++;
				}
				else if(operand == " E ")
				{
					EReg++;
				}
				else if(operand == " H ")
				{
					HReg++;
				}
				else if(operand == " L ")
				{
					LReg++;
				}
				else if(operand == " M ")
				{
					uint addr = 256 * HReg + LReg;
					memory[addr]++;
				}
			}

			if(instruction == " DCR ")
			{
				QString operand;
				extractWord(ui->textEdit->toPlainText(), index, operand);
				ui->statusBar->showMessage(tr("\"%1\"").arg(operand));
//				QMessageBox::information(this, tr("Interpret()::if"), tr("Here?"));
				if(operand == " A ")
				{
					AReg--;
				}
				else if(operand == " B ")
				{
					BReg--;
				}
				else if(operand == " C ")
				{
					CReg--;
				}
				else if(operand == " D ")
				{
					DReg--;
				}
				else if(operand == " E ")
				{
					EReg--;
				}
				else if(operand == " H ")
				{
					HReg--;
				}
				else if(operand == " L ")
				{
					LReg--;
				}
				else if(operand == " M ")
				{
					uint addr = 256 * HReg + LReg;
					memory[addr]--;
				}
			}

			if(instruction == " DAD ")
			{
			}

			if(instruction == " LDAX ")
			{
			}

			if(instruction == " DCX ")
			{
			}

			if(instruction == " SHLD ")
			{
			}

			if(instruction == " LHLD ")
			{
			}

			if(instruction == " STA ")
			{
			}

			if(instruction == " LDA ")
			{
			}

			if(instruction == " ADD ")
			{
			}

			if(instruction == " ADC ")
			{
			}

			if(instruction == " SUB ")
			{
			}

			if(instruction == " SBB ")
			{
			}

			if(instruction == " ANA ")
			{
			}

			if(instruction == " XRA ")
			{
			}

			if(instruction == " ORA ")
			{
			}

			if(instruction == " CMP ")
			{
			}

			if(instruction == " POP ")
			{
			}

			if(instruction == " JNZ ")
			{
			}

			if(instruction == " JMP ")
			{
			}

			if(instruction == " CNZ ")
			{
			}

			if(instruction == " PUSH ")
			{
			}

			if(instruction == " ADI ")
			{
			}

			if(instruction == " RST ")
			{
			}

			if(instruction == " JZ ")
			{
			}

			if(instruction == " CZ ")
			{
			}

			if(instruction == " CALL ")
			{
			}

			if(instruction == " ACI ")
			{
			}

			if(instruction == " JNC ")
			{
			}

			if(instruction == " CNC ")
			{
			}

			if(instruction == " SUI ")
			{
			}

			if(instruction == " IN ")
			{
			}

			if(instruction == " SBI ")
			{
			}

			if(instruction == " POP ")
			{
			}

			if(instruction == " JPO ")
			{
			}

			if(instruction == " CPO ")
			{
			}

			if(instruction == " ANI ")
			{
			}

			if(instruction == " JPE ")
			{
			}

			if(instruction == " CPE ")
			{
			}

			if(instruction == " XRI ")
			{
			}

			if(instruction == " JP ")
			{
			}

			if(instruction == " CP ")
			{
			}

			if(instruction == " ORI ")
			{
			}

			if(instruction == " JM ")
			{
			}

			if(instruction == " CM ")
			{
			}

			if(instruction == " CPI ")
			{
			}
		}

		case 2:
		{
			if(instruction == " LXI ")
			{
			}

			if(instruction == " MVI ")
			{
			}

			if(instruction == " MOV ")
			{
			}
		}
	}
	return UNKOWN_ERROR;
}

void MainWindow::capitalizeCode()
{
	int pos = ui->textEdit->textCursor().position();
	ui->textEdit->setText(ui->textEdit->toPlainText().toUpper());
	ui->textEdit->textCursor().setPosition(pos);
}

void MainWindow::updateListing()
{
	QString text;

	text.setNum(AReg, 16);
	if(text.length() == 1)
		text.prepend("0");
	ui->labelA->setText(text.toUpper());

	text.setNum(BReg, 16);
	if(text.length() == 1)
		text.prepend("0");
	ui->labelB->setText(text.toUpper());

	text.setNum(CReg, 16);
	if(text.length() == 1)
		text.prepend("0");
	ui->labelC->setText(text.toUpper());

	text.setNum(DReg, 16);
	if(text.length() == 1)
		text.prepend("0");
	ui->labelD->setText(text.toUpper());

	text.setNum(EReg, 16);
	if(text.length() == 1)
		text.prepend("0");
	ui->labelE->setText(text.toUpper());

	text.setNum(HReg, 16);
	if(text.length() == 1)
		text.prepend("0");
	ui->labelH->setText(text.toUpper());

	text.setNum(LReg, 16);
	if(text.length() == 1)
		text.prepend("0");
	ui->labelL->setText(text.toUpper());

	text.setNum(PC, 16);
	switch(text.length())
	{
		case 1:
			text.prepend("000");
			break;
		case 2:
			text.prepend("00");
			break;
		case 3:
			text.prepend("0");
			break;
	}
	ui->labelPC->setText(text.toUpper());

	text.setNum(SP, 16);
	switch(text.length())
	{
		case 1:
			text.prepend("000");
			break;
		case 2:
			text.prepend("00");
			break;
		case 3:
			text.prepend("0");
			break;
	}
	ui->labelSP->setText(text.toUpper());

	if(S)
		ui->labelS->setText("1");
	else
		ui->labelS->setText("0");

	if(Z)
		ui->labelZ->setText("1");
	else
		ui->labelZ->setText("0");

	if(Ac)
		ui->labelAc->setText("1");
	else
		ui->labelAc->setText("0");

	if(P)
		ui->labelP->setText("1");
	else
		ui->labelP->setText("0");

	if(CY)
		ui->labelCY->setText("1");
	else
		ui->labelCY->setText("0");

	SAChanged();
}
