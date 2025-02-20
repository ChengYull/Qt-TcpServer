#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_listen_clicked();
    void handleNewConnection();
    void handleReadData();
    void on_pushButton_send_clicked();

    void on_pushButton_stop_listen_clicked();

    void on_pushButton_cutoff_clicked();

private:
    Ui::Widget *ui;
    QTcpServer *server;
    QTcpSocket *clientSocket;
};
#endif // WIDGET_H
