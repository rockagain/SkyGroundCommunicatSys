#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QByteArray>

//挂机  被呼叫消失， 在这里调节时间
#define PHONE_TIME  5000

//视频音频呼叫的，  现被改为只有视频呼叫了
#define VIDEOCHAT 0
#define AUDIOCHAT 1

//判断哪个短信的
#define HAISHI_MSG 0
#define BEIDOU_MSG 1

//调节是哪个仓的参数
#define FRONT_CHAMBER 0
#define BACK_CHAMBER 1



enum ClientSTATUS{
    //这个基本没用上，因为电话流程总是更改   //只用了 前两个
    FREE_STATE  = 0,
    PHONING_STATE ,
    ANSWERING_STATE ,
    CALLING_STATE,
};




enum NetSTATUS{
    //未启用  ，  我测试客户端一断开就不能发信息了  ，太不友好了
    NET_IS_NOT_CONNECTED = 0,//判断是否有心跳
    NET_IS_CONNECTED ,    //心跳帧控制
};



//看是哪个端
enum Which_Client{
    LEFT = 0,
    RIGHT,
    GROUND
};




#endif // CONFIG_H


