#include "mytextedit.h"
#include <QKeyEvent>
#include <QMessageBox>

myTextEdit::myTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    //alfabet klawiatura
    QChar alfabetKlawTmp[] = { 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
                               'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
                               'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.'};
    rozmiarAlfabetuKlaw = 27;
    alfabetKlaw = new QChar[rozmiarAlfabetuKlaw];
    for( int przepisz=0; przepisz<rozmiarAlfabetuKlaw; przepisz++ )
        alfabetKlaw[przepisz] = alfabetKlawTmp[przepisz];

    //alfabet hebrajski
    rozmiarAlfabetuHeb = 27;
    pierwszaLiteraHeb = 1488;
    alfabetHeb = new QChar[rozmiarAlfabetuHeb];
    //litery alfabetu hebrajskiego
    for(int i=0; i<rozmiarAlfabetuHeb; i++)
        alfabetHeb[i]=pierwszaLiteraHeb+i;

    //konwersja heb
    tabHebKonw = new int[27];

    tabHebKonw[0] = 23;
    tabHebKonw[1] = 24;
    tabHebKonw[2] = 0;
    tabHebKonw[3] = 8;
    tabHebKonw[4] = 5;
    tabHebKonw[5] = 15;
    tabHebKonw[6] = 13;
    tabHebKonw[7] = 20;
    tabHebKonw[8] = 25;
    tabHebKonw[9] = 3;
    tabHebKonw[10] = 2;
    tabHebKonw[11] = 11;
    tabHebKonw[12] = 18;
    tabHebKonw[13] = 9;
    tabHebKonw[14] = 7;
    tabHebKonw[15] = 12;
    tabHebKonw[16] = 10;
    tabHebKonw[17] = 19;
    tabHebKonw[18] = 6;
    tabHebKonw[19] = 17;
    tabHebKonw[20] = 1;
    tabHebKonw[21] = 4;
    tabHebKonw[22] = 16;
    tabHebKonw[23] = 14;
    tabHebKonw[24] = 22;
    tabHebKonw[25] = 26;
    tabHebKonw[26] = 21;

    //konwersja grek
    tabGrekKonw = new int[50];
}

void myTextEdit::keyPressEvent(QKeyEvent *e){
    int iKey = e->key();
    if( iKey == Qt::Key_Space || iKey == Qt::Key_Backspace || iKey == Qt::Key_Return ||
            iKey == Qt::Key_Delete ||
            e->matches(QKeySequence::Copy) || e->matches(QKeySequence::Paste) ||
            e->matches(QKeySequence::SelectAll) )
    {
        QTextEdit::keyPressEvent( e );
        return;
    }

    if( jezykPisania == 0 ){
        for( int klaw=0; klaw<rozmiarAlfabetuKlaw; ++klaw )
            if( alfabetKlaw[klaw] == *e->text().begin() ){
                int indeksAlfHeb = tabHebKonw[klaw];
                this->insertPlainText( QString(alfabetHeb[ indeksAlfHeb ]) );
                return;
            }
    }/* else if( jezykPisania == 1 ){
        return;
    }*/
}

void myTextEdit::ustawJezyk(int jezyk){
    jezykPisania = jezyk;

    if ( jezykPisania == 0 ) // heb
    {
        QTextOption textOpt = this->document()->defaultTextOption();
        textOpt.setTextDirection(Qt::RightToLeft);

        QTextCursor textCur = this->textCursor();
        QTextBlockFormat textBlockF = textCur.blockFormat();
        textBlockF.setLayoutDirection( Qt::RightToLeft );
        textCur.setBlockFormat( textBlockF );

        this->document()->setDefaultTextOption( textOpt );
        this->setTextCursor( textCur );
    }/* else if( jezykPisania == 1 ){

    }*/
}

QString myTextEdit::getZaznaczonyTekst(){
    QTextCursor kursor = this->textCursor();
    if( kursor.hasSelection() ){
        return kursor.selectedText();
    } else {
        return NULL;
    }
}
