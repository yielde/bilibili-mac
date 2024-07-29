#ifndef PUBLICLIB_H
#define PUBLICLIB_H

#include <QString>

struct URLS {

    QString QRrequestUrl = "https://passport.bilibili.com/x/passport-login/web/qrcode/generate";
    QString CheckLoginUrl = "https://passport.bilibili.com/x/passport-login/web/qrcode/poll?qrcode_key=";
    QString AreaListUrl = "https://api.live.bilibili.com/room/v1/Area/getList";
    QString StartLiveUrl = "https://api.live.bilibili.com/room/v1/Room/startLive";
    QString StopLiveUrl = "https://api.live.bilibili.com/room/v1/Room/stopLive";
    QString GetRoomId = "https://api.live.bilibili.com/live_user/v1/Master/info?uid=";
    inline void SetLoginUrl(QString qrcode)
    {
        CheckLoginUrl = CheckLoginUrl + qrcode;
    }
};

#endif // PUBLICLIB_H
