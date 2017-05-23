package com.xuqiqiang.xmpp.view;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.widget.RemoteViews;

import com.snailstdio.software.xsdk.xmpp.R;
import com.xuqiqiang.xmpp.Xmpp;

public class XmppNotification {

    private static NotificationManager nm = null;
    private static Notification notif = null;

    public static void init(Context context, Class<?> mainActivity) {
        notif = new Notification();
        notif.icon = R.drawable.icon;
        notif.tickerText = null;

        // 通知栏显示所用到的布局文件
        notif.contentView = new RemoteViews(context.getPackageName(),
                R.layout.notification_view);

        Intent notificationIntent = new Intent(context, mainActivity);

        notificationIntent.setAction(Intent.ACTION_MAIN);

        notificationIntent.addCategory(Intent.CATEGORY_LAUNCHER);

        PendingIntent contentIntent = PendingIntent.getActivity(

        context, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        notif.contentIntent = contentIntent;

        notif.flags |= Notification.FLAG_ONGOING_EVENT; // 将此通知放到通知栏的"Ongoing"即"正在运行"组中

        notif.flags |= Notification.FLAG_NO_CLEAR;

        nm = (NotificationManager) context
                .getSystemService(context.NOTIFICATION_SERVICE);
    }

    public static void show() {
        notif.contentView.setImageViewBitmap(R.id.content_view_image,
                Xmpp.user_photo);
        notif.contentView.setTextViewText(R.id.tv_username, Xmpp.user_name);

        nm.notify(R.string.app_name, notif);
    }

    public static void close() {
        if (nm != null)
            nm.cancel(R.string.app_name);
    }
}