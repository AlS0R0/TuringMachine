#include "changealphabetforturingmachine.h"
#include "ui_changealphabetforturingmachine.h"
#include <QMessageBox>

ChangeAlphabetForTuringMachine::ChangeAlphabetForTuringMachine(QWidget *parent, QVector<QChar>* vec)
    : QDialog(parent)
    , ui(new Ui::ChangeAlphabetForTuringMachine)
    , new_alphabet(vec)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &ChangeAlphabetForTuringMachine::pushButton_clicked);
}

ChangeAlphabetForTuringMachine::~ChangeAlphabetForTuringMachine()
{
    delete ui;
}


void ChangeAlphabetForTuringMachine::pushButton_clicked() {

    QString text1 = ui->lineEdit_1->text();
    QString text2 = ui->lineEdit_2->text();

    new_alphabet->clear();

    auto addChars = [&](const QString &s) {
        for (const QChar &ch : s) {
            if (!ch.isSpace()) {
                new_alphabet->append(ch);
            }
        }
    };

    addChars(text1);
    addChars(text2);

    qDebug() << *new_alphabet << '\n';

    if (new_alphabet->isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Алфавит не может быть пустым");
        return;
    }

    QSet<QChar> unique;
    for (const QChar &ch : *new_alphabet) unique.insert(ch);
    *new_alphabet = unique.values().toVector();

    close();
}
