#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QClipboard>
#include <QKeyEvent>
#include <QProcess>
#include <QDesktopServices>
#include <qmath.h>
#include <quadmath.h>
#include <limits>
#include <qdebug.h>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Liczbowanie " + QString::number(wersjaProgramu(),'f',2) );

    ui->tableWidgetHeb->setColumnWidth(0,40);
    ui->tableWidgetHeb->setColumnWidth(1,50);
    ui->tableWidgetHeb->setColumnWidth(2,100);
    ui->tableWidgetGrek->setColumnWidth(0,40);
    ui->tableWidgetGrek->setColumnWidth(1,50);
    ui->tableWidgetGrek->setColumnWidth(2,100);

    ustawAlfabety();

    // Sprawdz aktualizacje po cichu
    QProcess::startDetached("LUpdate.exe "+wersjaProgramuStr()+" silent");
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool testFloat128( __float128 & iloczyn, int wartoscHeb )
{
    char float128char[64];
    int pos = 35;
    const __float128 eps = 1.0e-32;
    // Iloczyn from char
    quadmath_snprintf(float128char, sizeof float128char, "%.31Qe", iloczyn );
    float128char[pos] = '0';
    float128char[pos+1] = '0';
    float128char[pos+2] = '\0';

    __float128 iloczynFromChar = strtoflt128(float128char, NULL);

    // Test iloczyn from char
    __float128 testIloczyn = iloczynFromChar * wartoscHeb;
    quadmath_snprintf(float128char, sizeof float128char, "%.31Qe", testIloczyn );

    __float128 testIloczynFromChar = strtoflt128(float128char, NULL);
    quadmath_snprintf(float128char, sizeof float128char, "%.31Qe", testIloczynFromChar );

    __float128 delta = fabsq( testIloczynFromChar / wartoscHeb - iloczynFromChar );
    quadmath_snprintf(float128char, sizeof float128char, "%.31Qe", delta );

    if ( delta > eps ) return true;
    return false;
}

bool testDouble( double iloczyn, int wartoscHeb )
{
    const double eps = 1.0e-14;
    double testIloczyn = iloczyn * wartoscHeb; // Possible overflow

    qDebug() << QString::number( fabs( (testIloczyn / wartoscHeb) - iloczyn), 'g', 20);
    qDebug() << QString::number(1.0e-13, 'g', 20);

    if ( fabs( (testIloczyn / wartoscHeb) - iloczyn) > eps ) {
        return true;
    }
    return false;
}

void MainWindow::hebOblicz(){
    QString text;
    if( !ui->hebTextEdit->getZaznaczonyTekst().isNull() )
        text = ui->hebTextEdit->getZaznaczonyTekst();
    else
        text=ui->hebTextEdit->toPlainText();

    if(text.size()>=0){
        quint64 suma = 0, sumaNorm = 0, sumaPierwsze = 0, sumaOstatnie = 0;
        __float128 iloczyn = 1.0, iloczynNorm = 1.0;
        double iloczynPierwsze = 1, iloczynOstatnie = 1;
        quint64 gzyms = 1, gzymsNorm = 1, gzymsSuma = 0, gzymsSumaNorm = 0;
        unsigned int litery = 0, slowa = 0;

        // Suma i iloczyn
        bool iloczynIsMax = false, iloczynNormIsMax = false;

        QMap<int,int> literyMapa;
        for(int t=0; t < text.size() ;t++)
            for(int a=0; a < rozmiarAlfabetuHeb; a++)
                if(text.at(t) == alfabetHeb[a]) {
                    suma += wartosciHeb[a];
                    sumaNorm += wartosciHebNorm[a];

                    // Testuj
                    if( testFloat128( iloczyn, wartosciHeb[a] ) )
                        iloczynIsMax = true;

                    if( testFloat128( iloczynNorm, wartosciHebNorm[a] ) )
                        iloczynNormIsMax = true;

                    iloczyn *= wartosciHeb[a];
                    iloczynNorm *= wartosciHebNorm[a];

                    // Zliczanie liter do tabeli
                    if( literyMapa.contains( a ) )
                        literyMapa[ a ]++;
                    else
                        literyMapa[ a ] = 1;

                    litery++;
                    break;
                }

        // Czyszczenie tabeli liter
        ui->tableWidgetHeb->clearContents();
        while (ui->tableWidgetHeb->rowCount() > 0)
            ui->tableWidgetHeb->removeRow(0);

        // Wpisanie wierszy z literami
        QMapIterator<int, int> i(literyMapa);
        while (i.hasNext()) {
            i.next();
            int rowInsert = ui->tableWidgetHeb->rowCount();
            ui->tableWidgetHeb->insertRow( rowInsert );
            QTableWidgetItem *tmpLitera = new QTableWidgetItem( QString( alfabetHeb[i.key()]) );
            QTableWidgetItem *tmpIlosc = new QTableWidgetItem( QString::number(i.value()) );
            quint64 suma = i.value()*wartosciHeb[i.key()];
            QTableWidgetItem *tmpSuma = new QTableWidgetItem( QString::number( suma,'g', 15 ) );
            qreal iloczyn = qPow(wartosciHeb[i.key()],i.value());
            QTableWidgetItem *tmpIloczyn = new QTableWidgetItem( QString::number( iloczyn,'g', 15 ) );
            ui->tableWidgetHeb->setItem( rowInsert, 0, tmpLitera );
            ui->tableWidgetHeb->setItem( rowInsert, 1, tmpIlosc );
            ui->tableWidgetHeb->setItem( rowInsert, 2, tmpSuma );
            ui->tableWidgetHeb->setItem( rowInsert, 3, tmpIloczyn );
        }

        // Suma i iloczyn liter
        ui->hebIleLiterEdit->setText(QString::number(litery));
        ui->hebSumaEdit->setText(QString::number(suma));
        ui->hebSumaNormEdit->setText(QString::number(sumaNorm));
        if (text.size() > 1) {
            if( !litery )
                iloczyn = iloczynNorm = 0;
            // Iloczyn
            if( !iloczynIsMax )
            {
                char float128char[64];
                quadmath_snprintf(float128char, sizeof float128char, "%.31Qe", iloczyn );
                ui->hebIloczynEdit->setText(QString::fromLatin1(float128char));
                ui->hebIloczynEdit->setDisabled(false);
            }
            else
            {
                ui->hebIloczynEdit->setDisabled(true);
                ui->hebIloczynEdit->setText("Wynik niedokładny");
            }

            // Iloczyn norm
            if( !iloczynNormIsMax )
            {
                char float128char[64];
                quadmath_snprintf(float128char, sizeof float128char, "%.31Qe", iloczynNorm );
                ui->hebIloczynNormEdit->setText(QString::fromLatin1(float128char));
                ui->hebIloczynNormEdit->setDisabled(false);
            }
            else
            {
                ui->hebIloczynNormEdit->setDisabled(true);
                ui->hebIloczynNormEdit->setText("Wynik niedokładny");
            }

        } else {
            ui->hebIloczynEdit->clear();
            ui->hebIloczynNormEdit->clear();
        }

        // Liczenie gzymsu
        QString sumaString=ui->hebSumaEdit->text();
        ui->hebGzymsLog->clear();
        for(int g=0; g < sumaString.size(); g++){
            if(g > 0) ui->hebGzymsLog->append(QString::number(gzyms)+"*"+sumaString.mid(g,1)+"="
                                    +QString::number(gzyms*sumaString.at(g).digitValue()));
            gzyms*=sumaString.at(g).digitValue();
            gzymsSuma+=sumaString.at(g).digitValue();
        }

        // Liczenie gzymsu normalnego
        QString sumaNormString=ui->hebSumaNormEdit->text();
        ui->hebGzymsLogNorm->clear();
        for(int g=0; g < sumaNormString.size(); g++){
            if(g > 0) ui->hebGzymsLogNorm->append(QString::number(gzymsNorm)+"*"+sumaNormString.mid(g,1)+"="
                                    +QString::number(gzymsNorm*sumaNormString.at(g).digitValue()));
            gzymsNorm*=sumaNormString.at(g).digitValue();
            gzymsSumaNorm+=sumaNormString.at(g).digitValue();
        }
        if (text.size() > 1) {
            ui->hebGzymsEdit->setText(QString::number(gzyms));
            ui->hebGzymsSumaEdit->setText(QString::number(gzymsSuma));
            ui->hebGzymsNormEdit->setText(QString::number(gzymsNorm));
            ui->hebGzymsSumaNormEdit->setText(QString::number(gzymsSumaNorm));
        } else {
            ui->hebGzymsEdit->clear();
            ui->hebGzymsSumaEdit->clear();
            ui->hebGzymsNormEdit->clear();
            ui->hebGzymsSumaNormEdit->clear();
        }

        // Pierwsze litery
        bool iloczynPierwszeIsMax = false, iloczynOstatniIsMax = false;
        int t = 0;
        while(t < text.size()){
            for(int a=0; a < rozmiarAlfabetuHeb; a++){
                if(text.at(t) == alfabetHeb[a]) {
                    sumaPierwsze += wartosciHeb[a];

                    iloczynPierwsze *= wartosciHeb[a];
                    if( testDouble( iloczynPierwsze, wartosciHeb[a] ) )
                        iloczynPierwszeIsMax = true;

//                    const double eps = 1.0e-14;
//                    double testIloczynPierwsze = iloczynPierwsze * wartosciHeb[a]; // Possible overflow
//                    //if (testIloczynPierwsze / wartosciHeb[a] != iloczynPierwsze) { iloczynPierwszeIsMax = true;/* Overflow*/}
//                    //std::cout << abs( (testIloczynPierwsze / wartosciHeb[a]) - iloczynPierwsze);

//                    qDebug() << QString::number( fabs( (testIloczynPierwsze / wartosciHeb[a]) - iloczynPierwsze), 'g', 20);
//                    qDebug() << QString::number(1.0e-10, 'g', 20);
//                    if ( fabs( (testIloczynPierwsze / wartosciHeb[a]) - iloczynPierwsze) > eps ) {
//                        iloczynPierwszeIsMax = true;
//                    }
//                    iloczynPierwsze = testIloczynPierwsze; // No overflow


                    while(t<text.size() &&
                          text.at(t) != QChar(32) && // Znak Spacji
                          text.at(t) != QChar(13) && // Znak CR
                          text.at(t) != QChar(10)){  // Znak LF
                        ++t;
                    }
                    // Ostatenie litery
                    for(int aOst=0; aOst < rozmiarAlfabetuHeb; aOst++)
                        if(text.at(t - 1) == alfabetHeb[aOst]) {
                            sumaOstatnie += wartosciHeb[aOst];
                        }

                    ++slowa;
                    break;
                }
            }
            ++t;
        }
        ui->hebIleSlowEdit->setText(QString::number(slowa));
        ui->hebPierwszeSuma->setText(QString::number(sumaPierwsze));
        ui->hebNormPierwSuma->setText(QString::number(sumaOstatnie));
        if (text.size()>2){
            if( !iloczynPierwszeIsMax/*iloczynPierwsze < std::numeric_limits<qreal>::max()*/ ){
                ui->hebPierwszeIloczyn->setText(QString::number(iloczynPierwsze, 'g', 20));
                ui->hebPierwszeIloczyn->setDisabled(false);
            } else{
                //ui->hebPierwszeIloczyn->setText("Wynik niedokładny");
                ui->hebPierwszeIloczyn->setText(QString::number(iloczynPierwsze, 'g', 20));
                ui->hebPierwszeIloczyn->setDisabled(true);
            }
        } else {
            ui->hebPierwszeIloczyn->clear();
        }

    }
}

void MainWindow::grekOblicz(){
    QString text;
    if( !ui->grekTextEdit->getZaznaczonyTekst().isNull() ){
        text = ui->grekTextEdit->getZaznaczonyTekst();
    } else {
        text = ui->grekTextEdit->toPlainText();
        if(text.size()>0){
            //grekUsunAkcenty(text);
            //ui->grekTextEdit->insertPlainText(text);
        }
    }

    if(text.size()>0){
        quint64 suma=0, sumaPierwsze=0;
        qreal iloczyn=1, iloczynPierwsze=1;
        quint64 gzyms=1, gzymsSuma=0;
        int litery=0,slowa=0;

        // Suma i iloczyn
        QMap<int,int> literyMapa;
        for(int t=0;t<text.size();t++){
            for(int a=0; a<rozmiarAlfabetuGrek; a++){
                if(text.at(t)==alfabetGrek[a]) {
                    suma+=wartosciGrek[a];
                    iloczyn*=wartosciGrek[a];

                    // Zliczanie liter do tabeli
                    if( literyMapa.contains( a ) )
                        literyMapa[ a ]++;
                    else
                        literyMapa[ a ] = 1;

                    ++litery;
                    break;
                }
            }
        }
        ui->grekIleLiterEdit->setText(QString::number(litery));
        ui->grekSumaEdit->setText(QString::number(suma));
        if (text.size()>1) {
            if( !litery )
                iloczyn = 0;
            if( iloczyn < std::numeric_limits<qreal>::max() ){
                ui->grekIloczynEdit->setText(QString::number(iloczyn,'g', 15));
                ui->grekIloczynEdit->setDisabled(false);
            } else {
                ui->grekIloczynEdit->setText("Zbyt duża wartość");
                ui->grekIloczynEdit->setDisabled(true);
            }
        } else {
            ui->grekIloczynEdit->clear();
        }

        // Czyszczenie tabeli liter
        ui->tableWidgetGrek->clearContents();
        while (ui->tableWidgetGrek->rowCount() > 0)
            ui->tableWidgetGrek->removeRow(0);
        // Wpisanie wierszy z literami
        QMapIterator<int, int> i(literyMapa);
        while (i.hasNext()) {
            i.next();
            int rowInsert = ui->tableWidgetGrek->rowCount();
            ui->tableWidgetGrek->insertRow( rowInsert );
            QTableWidgetItem *tmpLitera = new QTableWidgetItem( QString( alfabetGrek[i.key()]) );
            QTableWidgetItem *tmpIlosc = new QTableWidgetItem( QString::number(i.value()) );
            quint64 suma = i.value()*wartosciGrek[i.key()];
            QTableWidgetItem *tmpSuma = new QTableWidgetItem( QString::number( suma,'g', 15 ) );
            qreal iloczyn = qPow(wartosciGrek[i.key()],i.value());
            QTableWidgetItem *tmpIloczyn = new QTableWidgetItem( QString::number( iloczyn,'g', 15 ) );
            ui->tableWidgetGrek->setItem( rowInsert, 0, tmpLitera );
            ui->tableWidgetGrek->setItem( rowInsert, 1, tmpIlosc );
            ui->tableWidgetGrek->setItem( rowInsert, 2, tmpSuma );
            ui->tableWidgetGrek->setItem( rowInsert, 3, tmpIloczyn );
        }

        // Liczymy gzyms
        ui->grekGzymsLog->clear();
        QString sumaString=ui->grekSumaEdit->text();
        for(int g=0;g<sumaString.size();g++){
            if(g>0) ui->grekGzymsLog->append(QString::number(gzyms)+"*"+sumaString.mid(g,1)+"="
                                    +QString::number(gzyms*sumaString.at(g).digitValue()));
            gzyms*=sumaString.at(g).digitValue();
            gzymsSuma+=sumaString.at(g).digitValue();
        }
        if (text.size()>1) {
            ui->grekGzymsEdit->setText(QString::number(gzyms));
            ui->grekGzymsSumaEdit->setText(QString::number(gzymsSuma));
        } else {
            ui->grekGzymsEdit->clear();
            ui->grekGzymsSumaEdit->clear();
        }
        // Pierwsze litery
        ui->grekPierwszeSuma->clear();
        int t=0;

        while(t<text.size()){
            for(int a=0; a<rozmiarAlfabetuGrek; a++){
                if(text.at(t)==alfabetGrek[a]) {
                    sumaPierwsze+=wartosciGrek[a];
                    iloczynPierwsze*=wartosciGrek[a];
                    while(t<text.size() &&
                          text.at(t)!=QChar(32) && // Znak Spacji
                          text.at(t)!=QChar(13) && // Znak CR
                          text.at(t)!=QChar(10)){  // Znak LF
                        t++;
                    }
                    ++slowa;
                    break;
                }
            }
            t++;
        }
        ui->grekIleSlowEdit->setText(QString::number(slowa));
        ui->grekPierwszeSuma->setText(QString::number(sumaPierwsze));
        if (text.size()>2){
            if( iloczynPierwsze < std::numeric_limits<qreal>::max() ){
                ui->grekPierwszeIloczyn->setText(QString::number(iloczynPierwsze,'g', 15));
                ui->grekPierwszeIloczyn->setDisabled(false);
            } else {
                ui->grekPierwszeIloczyn->setText("Zbyt duża wartość");
                ui->grekPierwszeIloczyn->setDisabled(true);
            }
        } else {
            ui->grekPierwszeIloczyn->clear();
        }
    } else {
        ui->grekSumaEdit->clear();
        ui->grekIloczynEdit->clear();
        ui->grekGzymsEdit->clear();
        ui->grekGzymsSumaEdit->clear();
        ui->grekPierwszeSuma->clear();
        ui->grekPierwszeIloczyn->clear();
    }
}

void MainWindow::on_hebWyczysc_clicked()
{
    ui->hebTextEdit->clear();
    ui->hebSumaEdit->clear();
    ui->hebIloczynEdit->clear();
    ui->hebGzymsEdit->clear();
    ui->hebGzymsSumaEdit->clear();
    ui->hebSumaNormEdit->clear();
    ui->hebIloczynNormEdit->clear();
    ui->hebGzymsNormEdit->clear();
    ui->hebGzymsSumaNormEdit->clear();
    ui->hebPierwszeSuma->clear();
    ui->hebPierwszeIloczyn->clear();
    ui->hebIleLiterEdit->clear();
    ui->hebIleSlowEdit->clear();
}

void MainWindow::on_hebWklej_clicked()
{
    QClipboard *schowek = QApplication::clipboard();
    ui->hebTextEdit->append( schowek->text() );
}

void MainWindow::on_grekWyczysc_clicked()
{
    ui->grekTextEdit->clear();
    ui->grekSumaEdit->clear();
    ui->grekIloczynEdit->clear();
    ui->grekGzymsEdit->clear();
    ui->grekGzymsSumaEdit->clear();
    ui->grekGzymsLog->clear();
    ui->grekPierwszeIloczyn->clear();
    ui->grekPierwszeSuma->clear();
    ui->grekIleLiterEdit->clear();
    ui->grekIleSlowEdit->clear();
}

void MainWindow::on_grekWklej_clicked()
{
    QClipboard *schowek = QApplication::clipboard();
    QString text = schowek->text();
    grekUsunAkcenty(text);
    ui->grekTextEdit->append(text);
}

void MainWindow::on_hebKopiuj_clicked()
{
    QClipboard *schowek = QApplication::clipboard();
    if( !ui->hebTextEdit->getZaznaczonyTekst().isNull() )
        schowek->setText( ui->hebTextEdit->getZaznaczonyTekst() );
    else
        schowek->setText( ui->hebTextEdit->toPlainText() );
}

void MainWindow::on_grekKopiuj_clicked()
{
    QClipboard *schowek = QApplication::clipboard();
    if( !ui->grekTextEdit->getZaznaczonyTekst().isNull() )
        schowek->setText( ui->grekTextEdit->getZaznaczonyTekst() );
    else
        schowek->setText( ui->grekTextEdit->toPlainText() );
}

void MainWindow::ustawAlfabety(){
    // Ustawienie jezyka dla editTextow
    ui->hebTextEdit->ustawJezyk( 0 );
    ui->grekTextEdit->ustawJezyk( 1 );

    //alfabet hebrajski
    rozmiarAlfabetuHeb = 27;
    pierwszaLiteraHeb = 1488;
    alfabetHeb = new QChar[rozmiarAlfabetuHeb];
    wartosciHeb = new int[rozmiarAlfabetuHeb];
    wartosciHebNorm = new int[rozmiarAlfabetuHeb];

    wartosciHeb[0]=1; //alef
    wartosciHeb[1]=2; //bet
    wartosciHeb[2]=3; //gimel
    wartosciHeb[3]=4; //dalet
    wartosciHeb[4]=5; //he
    wartosciHeb[5]=6;
    wartosciHeb[6]=7;
    wartosciHeb[7]=8;
    wartosciHeb[8]=9;
    wartosciHeb[9]=10; //yod
    wartosciHeb[10]=500; //FINAL kaf
    wartosciHeb[11]=20; //kaf
    wartosciHeb[12]=30; //lamed
    wartosciHeb[13]=600; //FINAL mem
    wartosciHeb[14]=40; //mem
    wartosciHeb[15]=700; //FINAL nun
    wartosciHeb[16]=50; //nun
    wartosciHeb[17]=60;
    wartosciHeb[18]=70;
    wartosciHeb[19]=800; //FINAL pe
    wartosciHeb[20]=80; //pe
    wartosciHeb[21]=900; //FINAL tsadi
    wartosciHeb[22]=90; //tsadi
    wartosciHeb[23]=100;
    wartosciHeb[24]=200;
    wartosciHeb[25]=300;
    wartosciHeb[26]=400;

    for(int i=0; i<rozmiarAlfabetuHeb; i++)
        wartosciHebNorm[i]=wartosciHeb[i];

    wartosciHebNorm[10]=20;
    wartosciHebNorm[13]=40;
    wartosciHebNorm[15]=50;
    wartosciHebNorm[19]=80;
    wartosciHebNorm[21]=90;

    //litery alfabetu hebrajskiego
    for(int i=0; i<rozmiarAlfabetuHeb; i++)
        alfabetHeb[i]=pierwszaLiteraHeb+i;

    // Alfabet grecki
    rozmiarAlfabetuGrek = 49;
    pierwszaLiteraGrek = 913;
    pierwszaMalaLiteraGrek = 945;
    alfabetGrek = new QChar[rozmiarAlfabetuGrek];
    wartosciGrek = new int[rozmiarAlfabetuGrek];

    wartosciGrek[0]=1; // α
    wartosciGrek[1]=2; // β
    wartosciGrek[2]=3; // γ
    wartosciGrek[3]=4; // δ
    wartosciGrek[4]=5; // ε
    wartosciGrek[5]=7; // ζ
    wartosciGrek[6]=8; // η
    wartosciGrek[7]=9; // θ
    wartosciGrek[8]=10; // ι
    wartosciGrek[9]=20; // κ
    wartosciGrek[10]=30; // λ
    wartosciGrek[11]=40; // μ
    wartosciGrek[12]=50; // ν
    wartosciGrek[13]=60; // ξ
    wartosciGrek[14]=70; // ο
    wartosciGrek[15]=80; // π
    wartosciGrek[16]=100; // ρ
    wartosciGrek[17]=200; // σ
    wartosciGrek[18]=300; // τ
    wartosciGrek[19]=400; // υ
    wartosciGrek[20]=500; // φ
    wartosciGrek[21]=600; // χ
    wartosciGrek[22]=700; // ψ
    wartosciGrek[23]=800; // ω

    for(int i=0; i<24; i++)
        wartosciGrek[i+24]=wartosciGrek[i];

    //litery alfabetu greckiego
    for(int i=0; i<17; i++){
        alfabetGrek[i+24]=pierwszaMalaLiteraGrek+i;
        alfabetGrek[i]=pierwszaLiteraGrek+i;
    }
    for(int i=17; i<24; i++){
        alfabetGrek[i+24]=pierwszaMalaLiteraGrek+i+1;
        alfabetGrek[i]=pierwszaLiteraGrek+i+1;
    }
    alfabetGrek[48]=962;
    wartosciGrek[48]=6; // ς

    // Grecki akcenty
    grekAkcentList.append("αάὰᾶἀἄἂἆἁἅἃἇᾱᾰᾳᾴᾲᾷᾀᾄᾂᾆᾁᾅᾃᾇ");
    grekAkcentList.append("εέὲἐἔἒἑἕἓ");
    grekAkcentList.append("ηήὴῆἠἤἢἦἡἥἣἧῃῄῂῇᾐᾔᾒᾖᾑᾕᾓᾗ");
    grekAkcentList.append("ιίὶῖἰἴἲἶἱἵἳἷϊΐῒῗῑῐ");
    grekAkcentList.append("οόὸὀὄὂὁὅὃ");
    grekAkcentList.append("υύὺῦὐὔὒὖὑὕὓὗϋΰῢῧῡῠ");
    grekAkcentList.append("ωώὼῶὠὤὢὦὡὥὣὧῳῴῲῷᾠᾤᾢᾦᾡᾥᾣᾧ");
    grekAkcentList.append("ρῤῥ");
    grekAkcentList.append("ΑΆᾺἈἌἊἎἉἍἋἏᾹᾸᾼᾈᾌᾊᾎᾉᾍᾋᾏ");
    grekAkcentList.append("ΕΈῈἘἜἚἙἝἛ");
    grekAkcentList.append("ΗΉῊἨἬἪἮἩἭἫἯῌᾘᾜᾚᾞᾙᾝᾛᾟ");
    grekAkcentList.append("ΙΊῚἸἼἺἾἹἽἻἿΪῙῘ");
    grekAkcentList.append("ΟΌῸὈὌὊὉὍὋ");
    grekAkcentList.append("ΥΎῪὙὝὛὟΫῩῨ");
    grekAkcentList.append("ΩΏῺὨὬὪὮὩὭὫὯῼᾨᾬᾪᾮᾩᾭᾫᾯ");
    grekAkcentList.append("ΡῬ");
}

float MainWindow::wersjaProgramu(){
    return wersjaProgramuLiczbowanie;
}

QString MainWindow::wersjaProgramuStr(){
    return QString::number(wersjaProgramuLiczbowanie,'f',2);
}

void MainWindow::on_actionWyjscie_triggered()
{
    qApp->exit(0);
}

void MainWindow::on_actionOProgramie_triggered()
{
    QMessageBox::information(this, tr("O programie"),tr("Program dedykuję mojej Kochanej Esterce \noraz całej wspaniałej Rodzinie Bożków :) \n - Mateusz"));
}

void MainWindow::on_actionWyswietl_alfabety_triggered()
{
    ui->hebTextEdit->clear();
    ui->grekTextEdit->clear();

    for(int i=0; i<rozmiarAlfabetuGrek; i++)
        ui->grekTextEdit->insertPlainText( QString( alfabetGrek[i] )+ "("+ QString::number(wartosciGrek[i]) +") " );
    for(int i=0; i<rozmiarAlfabetuHeb; i++)
        ui->hebTextEdit->insertPlainText( QString( alfabetHeb[i] ) + "("+ QString::number(wartosciHeb[i]) +") " );
}

void MainWindow::on_actionAktualizuj_program_triggered()
{
    QProcess::startDetached("LUpdate.exe "+wersjaProgramuStr());
}

void MainWindow::grekUsunAkcenty(QString &text){
    foreach (QString strAkc, grekAkcentList)
        for( int c=1; c<strAkc.size(); ++c)
            text.replace( strAkc.at(c), strAkc.at(0) );

}

void MainWindow::on_actionPobierz_Liczbowanie_triggered()
{
     QDesktopServices::openUrl(QUrl("http://przenz.linuxpl.info/liczbowanie/"));
}
