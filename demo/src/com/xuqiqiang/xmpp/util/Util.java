package com.xuqiqiang.xmpp.util;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;

import com.snailstdio.software.xsdk.utils.Cache;
import com.snailstdio.software.xsdk.utils.ScreenInfo;
import com.snailstdio.software.xsdk.feedback.CrashHandler;
import com.snailstdio.software.xsdk.logger.Logger;
import com.snailstdio.software.xsdk.logger.Settings;
import com.snailstdio.software.xsdk.view.ThemeManager;
import com.xuqiqiang.xmpp.BuildConfig;
import com.xuqiqiang.xmpp.R;
import com.xuqiqiang.xmpp.Xmpp;
import com.xuqiqiang.xmpp.activity.XmppUI;
import com.xuqiqiang.xmpp.data.MyString;

public class Util {
    public static final String path = "xmpp";

    public static void createInstance(XmppUI context) {
        Logger.initialize(new Settings().isShowMethodLink(true)
                .isShowThreadInfo(false).setMethodOffset(0)
                .setLogPriority(BuildConfig.DEBUG ? Log.VERBOSE : Log.ASSERT));
        Cache.init(context, path);
        ScreenInfo.createInstance(context);
        ThemeManager.createInstance(context);
        CrashHandler.getInstance().init(context, context.getClass());
    }

    public static void saveBitmap(Context context, String chat_object_name,
            String sender_name, String file_name, Bitmap bitmap) {
        Cache cache = Cache.getInstance();
        cache.initWrite(
                new String[] { Xmpp.user_name,
                        context.getString(R.string.file_chat), chat_object_name },
                "chat_data", true);

        cache.writeString(sender_name + ":" + MyString.PHOTO_NAME_PREFIX
                + file_name);
        cache.SaveWrite();

        cache.initWrite(
                new String[] { Xmpp.user_name,
                        context.getString(R.string.file_chat), chat_object_name },
                file_name, false);
        cache.writeBitmap(bitmap);
        cache.SaveWrite();
    }
}