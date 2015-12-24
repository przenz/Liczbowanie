#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    static const float wersjaProgramuLiczbowanie = 0.90f;
    Ui::MainWindow *ui;
    QChar *alfabetHeb;
    int *wartosciHeb;
    int *wartosciHebNorm;
    int rozmiarAlfabetuHeb;
    int pierwszaLiteraHeb;
    QChar *alfabetGrek;
    int *wartosciGrek;
    int rozmiarAlfabetuGrek;
    int pierwszaMalaLiteraGrek;
    int pierwszaLiteraGrek;
    bool klawiaturaHeb;

    QStringList grekAkcentList;

private slots:
    float wersjaProgramu();
    QString wersjaProgramuStr();
    void ustawAlfabety();
    void hebOblicz();
    void grekOblicz();
    void on_actionWyjscie_triggered();
    void on_actionOProgramie_triggered();
    void on_hebWyczysc_clicked();
    void on_hebWklej_clicked();
    void on_grekWyczysc_clicked();
    void on_grekWklej_clicked();
    void on_actionWyswietl_alfabety_triggered();
    void on_actionAktualizuj_program_triggered();
    void on_hebKopiuj_clicked();
    void on_grekKopiuj_clicked();

    void grekUsunAkcenty(QString &text);
    void on_actionPobierz_Liczbowanie_triggered();
};

#endif // MAINWINDOW_H
