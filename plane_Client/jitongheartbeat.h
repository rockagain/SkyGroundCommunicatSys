#ifndef JITONGHEARTBEAT_H
#define JITONGHEARTBEAT_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QByteArray>
#include <QMessageBox>
#include <QTimer>
#include <QMovie>
#include <QLabel>
#include <QTextCodec>
#include <QDate>
#include <QDateTime>
#include <QSettings>


//已被废弃
class JiTongHeartbeat : public QObject
{
    Q_OBJECT

public:

    explicit JiTongHeartbeat(QObject *parent = nullptr);
    QUdpSocket* heartbeatSocket;
    int HEARTBEAT_PORT;
    QString SERVER_ADRESS;
private:

    void init();
    QTimer* heartbeatTimer;
    void heartbeatPortReceived();
    void checkMsg(QByteArray reply);

    QByteArray ShortToByte(short num);
    QString giveCurrentTime();
private slots:

    void JiTongHeartBeat();

signals:

public slots:
};

#endif // JITONGHEARTBEAT_H
