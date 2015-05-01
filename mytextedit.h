#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QTextEdit>

class myTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit myTextEdit(QWidget *parent = 0);
    void ustawJezyk(int jezyk);
    QString getZaznaczonyTekst();

private:
    int jezykPisania;
    int *tabHebKonw, *tabGrekKonw;

    QChar *alfabetKlaw;
    int rozmiarAlfabetuKlaw;
    QChar *alfabetHeb;
    int rozmiarAlfabetuHeb;
    int pierwszaLiteraHeb;
    QChar *alfabetGrek;
    int rozmiarAlfabetuGrek;
    int pierwszaMalaLiteraGrek;
    int pierwszaLiteraGrek;

protected:
    virtual void keyPressEvent(QKeyEvent *e);

signals:
    
public slots:
    
};

#endif // MYTEXTEDIT_H
