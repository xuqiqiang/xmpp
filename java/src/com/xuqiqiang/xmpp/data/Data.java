package com.xuqiqiang.xmpp.data;

public class Data {

    public boolean arrive;
    public String id;
    public long time;

    public Data(String id) {
        this.id = id;
        arrive = false;
        time = System.currentTimeMillis();
    }
}