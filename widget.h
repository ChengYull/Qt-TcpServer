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
    void handleReadData(QTcpSocket* client);
    void on_pushButton_send_clicked();

    void on_pushButton_stop_listen_clicked();

    void on_pushButton_clear_clicked();

    void handleDisconnect(QTcpSocket* client);
private:
    Ui::Widget *ui;
    QTcpServer *server;
    QList<QTcpSocket*> clients;
};
#endif // WIDGET_H
