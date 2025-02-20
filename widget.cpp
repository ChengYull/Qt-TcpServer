#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QMessageBox>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setLayout(ui->mainLayout);
    ui->pushButton_stop_listen->setDisabled(true);
    server = new QTcpServer(this);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_listen_clicked()
{
    // 开始监听
    int port = ui->lineEdit_port->text().toInt();
    QHostAddress ip;
    if(ui->comboBox_ip->currentIndex() == 0)
        ip = QHostAddress::Any;
    else
        ip = QHostAddress(ui->comboBox->currentText());
    if(!server->listen(ip, port)){
        qDebug() << "listen Error!";
        return;
    }
    // qDebug() << "Server listening on port " + ui->lineEdit_port->text();
    ui->textEdit_info->append("Server listening on port " + ui->lineEdit_port->text());
    connect(server, &QTcpServer::newConnection, this, &Widget::handleNewConnection);

    ui->pushButton_listen->setDisabled(true);
    ui->pushButton_stop_listen->setDisabled(false);
}

void Widget::handleNewConnection()
{
    // 更新在线人数
    QString onlineNum = QString::fromStdString(
                std::to_string(server->findChildren<QTcpSocket*>().size()));
    ui->label_onlineNum->setText(onlineNum);
    QTcpSocket *currentClient = server->nextPendingConnection();
    clients.append(currentClient);
    ui->textEdit_info->append("new connection form :" + currentClient->localAddress().toString());
    // 读取客户端传来的数据
    connect(currentClient, &QTcpSocket::readyRead, [=](){ handleReadData(currentClient); });
    // 断开连接后 移除Socket释放内存
    connect(currentClient, &QTcpSocket::disconnected, [=](){ handleDisconnect(currentClient); });
}

void Widget::handleReadData(QTcpSocket* client)
{
    QByteArray data = client->readAll();
    QString utf8Text = QString::fromUtf8(data);
    ui->textEdit_info->append("receive: " + utf8Text);
    qDebug() << "message:" << utf8Text;
}


void Widget::on_pushButton_send_clicked()
{
    QString mes = ui->textEdit_sendData->toPlainText();
    // 广播发送给连接的所有客户端
    if(clients.size() == 0){
        QMessageBox::warning(nullptr, "无连接", "没有客户端连接到服务器");
        return;
    }
    for(auto client : clients){
        if(client->state() == QAbstractSocket::ConnectedState){
            client->write(mes.toStdString().c_str());
        }
    }
    ui->textEdit_info->append("send: " + mes);
    ui->textEdit_sendData->clear();

}


void Widget::on_pushButton_stop_listen_clicked()
{
    if(server->isListening()){
        // 断开连接槽函数
        disconnect(server, &QTcpServer::newConnection, this, &Widget::handleNewConnection);
        // 停止监听（停止接收新的连接）
        server->close();
        ui->pushButton_stop_listen->setDisabled(true);
        ui->pushButton_listen->setDisabled(false);

        // 强制断开所有已连接的客户端
        for (QTcpSocket *client : clients) {
            client->disconnectFromHost();
            client->deleteLater();
        }
        ui->textEdit_info->append("server closed...");
    }

}


void Widget::on_pushButton_clear_clicked()
{
    ui->textEdit_info->clear();
}

void Widget::handleDisconnect(QTcpSocket *client)
{
    // 显示信息
    QString c_ip = client->peerAddress().toString();
    ui->textEdit_info->append(c_ip + " leaved.");
    // 移除list
    clients.removeOne(client);
    // 释放内存
    client->deleteLater();
    // 更新在线人数
    QString onlineNum = QString::fromStdString(std::to_string(clients.size()));
    ui->label_onlineNum->setText(onlineNum);
}

