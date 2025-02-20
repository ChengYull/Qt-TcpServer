#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
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
    clientSocket = server->nextPendingConnection();
    ui->textEdit_info->append("new connection form :" + clientSocket->localAddress().toString());
    // 读取客户端传来的数据
    connect(clientSocket, &QTcpSocket::readyRead, this, &Widget::handleReadData);
    // 断开连接后 移除Socket释放内存
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
}

void Widget::handleReadData()
{
    QByteArray data = clientSocket->readAll();
    QString utf8Text = QString::fromUtf8(data);
    ui->textEdit_info->append("receive: " + utf8Text);
    qDebug() << "message:" << utf8Text;
}


void Widget::on_pushButton_send_clicked()
{
    QString mes = ui->textEdit_sendData->toPlainText();
    if(clientSocket->state() == QAbstractSocket::ConnectedState){
        clientSocket->write(mes.toStdString().c_str());
        ui->textEdit_info->append("send: " + mes);
        ui->textEdit_sendData->clear();
    }
    else{
        qDebug() << "client socket is disconnected";
    }
}


void Widget::on_pushButton_stop_listen_clicked()
{
    if(server->isListening()){
        // 停止监听（停止接收新的连接）
        server->pauseAccepting();
        ui->textEdit_info->append("stop listening...");
        ui->pushButton_stop_listen->setDisabled(true);
        ui->pushButton_listen->setDisabled(false);
    }

}


void Widget::on_pushButton_cutoff_clicked()
{

    server->close();
    ui->pushButton_stop_listen->setDisabled(true);
    ui->pushButton_listen->setDisabled(false);
    // 取消连接
    //clientSocket->disconnectFromHost();
    // 强制断开所有已连接的客户端（可选）
    QList<QTcpSocket*> clients = server->findChildren<QTcpSocket*>();
    for (QTcpSocket *client : clients) {
        client->disconnectFromHost();
        client->deleteLater();
    }
    ui->textEdit_info->append("server closed...");
}

