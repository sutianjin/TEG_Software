
#include "commandwidget.h"
#include "ui_commandwidget.h"

#include <QRegExp>
#include <QRegExpValidator>
#include <QtDebug>
#include <QIcon>

CommandWidget::CommandWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandWidget)
{
    ui->setupUi(this);

#ifdef Q_OS_WIN
    ui->pbDelete->setIcon(QIcon(":/icons/list-remove"));
#endif // Q_OS_WIN

    connect(ui->pbDelete, &QPushButton::clicked, this, &CommandWidget::onDeleteClicked);
    connect(ui->pbSend, &QPushButton::clicked, this, &CommandWidget::onSendClicked);
    connect(ui->pbASCII, &QPushButton::toggled, this, &CommandWidget::onASCIIToggled);
}

CommandWidget::~CommandWidget()
{
    delete ui;
}

void CommandWidget::onDeleteClicked()
{
    this->deleteLater();
}

void CommandWidget::onSendClicked()
{
    auto command = ui->leCommand->text();

    if (command.isEmpty())
    {
        qWarning() << "Enter a command to send!";
        ui->leCommand->setFocus(Qt::OtherFocusReason);
        return;
    }

    if (isASCIIMode())
    {
        qDebug() << "Sending:" << command;
        emit sendCommand(ui->leCommand->unEscapedText().toLatin1());
    }
    else // hex mode
    {
        command = command.remove(' ');
        // check if nibbles are missing
        if (command.size() % 2 == 1)
        {
            qWarning() << "HEX command is missing a nibble at the end!";
            ui->leCommand->setFocus(Qt::OtherFocusReason);
            // highlight the byte that is missing a nibble (last byte obviously)
            int textSize = ui->leCommand->text().size();
            ui->leCommand->setSelection(textSize-1, textSize);
            return;
        }
        qDebug() << "Sending HEX:" << command;
        emit sendCommand(QByteArray::fromHex(command.toLatin1()));
    }
}

void CommandWidget::onASCIIToggled(bool checked)
{
    ui->leCommand->setMode(checked);
}

bool CommandWidget::isASCIIMode()
{
    return ui->pbASCII->isChecked();
}
