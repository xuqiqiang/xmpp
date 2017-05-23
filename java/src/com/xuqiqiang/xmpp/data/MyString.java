package com.xuqiqiang.xmpp.data;

import android.text.TextUtils;

public class MyString {

    public final static String PHOTO_NAME_PREFIX = "<xmpp_photo>";

    public static String getNameFromJid(String jid) {
        int index = jid.indexOf('@');
        String name;
        if (index != -1) {
            name = jid.substring(0, index);
        } else {
            name = jid;
        }
        return name;
    }

    public static String getJidFromString(String str) {
        int index = str.indexOf('/');
        String name;
        if (index != -1) {
            name = str.substring(0, index);
        } else {
            name = str;
        }
        return name;
    }

    public static String getJidFromMessage(String message) {
        int index = message.indexOf(':');
        String str;
        if (index != -1) {
            str = message.substring(0, index);
        } else {
            str = message;
        }
        return getJidFromString(str);
    }

    public static String check_photo(String str) {
        if (TextUtils.isEmpty(str))
            return null;
        if (str.startsWith(PHOTO_NAME_PREFIX)) {
            return str.substring(PHOTO_NAME_PREFIX.length(), str.length());
        }

        return null;
    }

    public static String getSender(String message) {
        int index = message.indexOf(':');
        String str;
        if (index != -1) {
            str = message.substring(0, index);
        } else {
            str = message;
        }
        return str;
    }

    public static String getContent(String message) {
        int index = message.indexOf(':');
        String str;
        if (index != -1) {
            str = message.substring(index + 1, message.length());
        } else {
            str = message;
        }
        return str;
    }
}