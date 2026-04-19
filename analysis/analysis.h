#ifndef ANALYSIS_H
#define ANALYSIS_H
#include <QMainWindow>

namespace Ui {
class Analysis;
}

class Analysis : public QMainWindow
{
    Q_OBJECT

public:
    explicit Analysis(QWidget *parent = nullptr);
    ~Analysis();
signals:
    void to_mainwindow();

private slots:
    void on_back_clicked();

    void on_start_clicked();

    void on_input_choose_clicked();

    void on_output_choose_clicked();

private:
    Ui::Analysis *ui;
    std::string rcg_path;
    QString output_path;
};

#endif // ANALYSIS_H
