#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QByteArray>


#define PHONE_TIME  5000
#define VIDEOCHAT 0
#define AUDIOCHAT 1


enum ClientSTATUS{
    //三种状态
    FREE_STATE  = 0,
    PHONING_STATE ,
    ANSWERING_STATE ,
    CALLING_STATE,
};

enum NetSTATUS{
    NET_IS_NOT_CONNECTED = 0,//判断是否有心跳
    NET_IS_CONNECTED ,    //心跳帧控制
};


enum Which_Client{
    LEFT = 0,
    RIGHT,
    GROUND
};




#endif // CONFIG_H


