package com.xuqiqiang.xmpp.data;

import android.graphics.Bitmap;

public class Friend {

    public String name;
    public String nickname;
    public Bitmap photo;
    public String status;
    public int new_message;

    public Friend(String name) {
        this.name = name;
        nickname = name;
        photo = null;
        status = "离线";
        new_message = 0;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public void setNickname(String nickname) {
        this.nickname = nickname;
    }

    public void setPhoto(Bitmap photo) {
        this.photo = photo;
    }
}