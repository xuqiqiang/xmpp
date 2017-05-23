package com.xuqiqiang.xmpp;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.zip.GZIPOutputStream;

import org.json.JSONException;
import org.json.JSONObject;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.text.TextUtils;
import android.util.Log;

import com.snailstdio.software.xsdk.utils.BitmapUtils;
import com.snailstdio.software.xsdk.utils.Cache;
import com.snailstdio.software.xsdk.utils.DisplayUtils;
import com.snailstdio.software.xsdk.xmpp.R;
import com.xuqiqiang.xmpp.data.Friend;
import com.xuqiqiang.xmpp.data.XmppMessage;

public class Xmpp {

    static {
        System.loadLibrary("xmpp");
    }

    public native static void initXmpp();

    public native static void register(String strUserName, String strPassWord,
            String strServerIP, int serverAltport);

    public native static void connectXmpp(String strUserName,
            String strPassWord, String strServerName, String strServerIP,
            int serverAltport);

    public native static void setStatus(String status);

    public native static String sendChatMessage(String chat_object_name,
            String content);

    public native static void getFriendsList();

    public native static void addFriendRespond(String friend_name, int Respond);

    public native static void addFriend(String friend_name);

    public native static void deleteFriend(String friend_name);

    public native static void stopXmpp();

    public native static void releaseXmpp();

    public native static String sendPicture(String chat_object_name,
            String name, String content);

    public native static void getVCard(String user_name);

    public native static void setVCard(String nick_name, String photo);

    /**
     * 字符串的压缩
     * 
     * @param str
     *            待压缩的字符串
     * @return 返回压缩后的字符串
     * @throws IOException
     */
    public static String compress(String str) throws IOException {
        if (null == str || str.length() <= 0) {
            return str;
        }
        // 创建一个新的 byte 数组输出流
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        // 使用默认缓冲区大小创建新的输出流
        GZIPOutputStream gzip = new GZIPOutputStream(out);
        // 将 b.length 个字节写入此输出流
        gzip.write(str.getBytes());
        gzip.close();
        // 使用指定的 charsetName，通过解码字节将缓冲区内容转换为字符串
        return out.toString("ISO-8859-1");
    }

    // ///////////////////////////////////////////////////////////////////////
    public native static void requestGame(String friend_name);

    public native static void acceptGame(String friend_name);

    public native static void sendGameMessage(String friend_name, String content);

    public static final int STR_LEN = 9999;

    public static Context context;

    public static String user_name;

    public static String user_password;

    public static String user_nick_name;

    public static Bitmap user_photo;

    public static Bitmap default_photo;

    public static String server_name;

    public static String server_ip;

    public static int server_altport;

    public static String broadcast;

    public final static String broadcast_action[] = new String[] { "ON_LOGIN",
            "ON_GET_FRIENDS_LIST", "ON_RECEIVE_MESSAGE", "ON_RECEIVE_FEEDBACK",
            "ON_GET_FRIENDS_STATUS", "ON_REQUEST_GAME_ID", "ON_ACCEPT_GAME_ID",
            "ON_RECEIVE_GAME_MESSAGE_ID",

            "ON_XMPP_REGISTER_SUCCESSFUL", "ON_XMPP_REGISTER_NOT_SUCCESSFUL",
            "ON_XMPP_REGISTER_ALREADY_EXIST",
            "ON_XMPP_REGISTER_NETWORK_CUT_DOWN",

            "ON_RECEIVE_PICTURE_ID", "ON_ADD_FRIEND_ID",
            "ON_ADD_FRIEND_RESPOND_AGREE", "ON_ADD_FRIEND_RESPOND_DISAGREE",
            "ON_NEED_SET_VCARD_ID", "ON_GET_VCARD_ID",
            "ON_SET_VCARD_SUCCESSFUL_ID", "ON_AUTH_ERROR_ID",
            "ON_NETWORK_DISCONNECT_ID", };

    public final static int ON_LOGIN_ID = 0, ON_GET_FRIENDS_LIST_ID = 1,
            ON_RECEIVE_MESSAGE_ID = 2, ON_RECEIVE_FEEDBACK_ID = 3,
            ON_GET_FRIENDS_STATUS_ID = 4, ON_REQUEST_GAME_ID = 5,
            ON_ACCEPT_GAME_ID = 6, ON_RECEIVE_GAME_MESSAGE_ID = 7,
            ON_XMPP_REGISTER_SUCCESSFUL_ID = 8,
            ON_XMPP_REGISTER_NOT_SUCCESSFUL_ID = 9,
            ON_XMPP_REGISTER_ALREADY_EXIST_ID = 10,
            ON_XMPP_REGISTER_NETWORK_CUT_DOWN_ID = 11,
            ON_RECEIVE_PICTURE_ID = 12, ON_ADD_FRIEND_ID = 13,
            ON_ADD_FRIEND_RESPOND_AGREE_ID = 14,
            ON_ADD_FRIEND_RESPOND_DISAGREE_ID = 15, ON_NEED_SET_VCARD_ID = 16,
            ON_GET_VCARD_ID = 17, ON_SET_VCARD_SUCCESSFUL_ID = 18,
            ON_AUTH_ERROR_ID = 19, ON_NETWORK_DISCONNECT_ID = 20;

    public final static int XMPP_ADD_FRIEND_RESPOND_AGREE = 0,
            XMPP_ADD_FRIEND_RESPOND_DISAGREE = 1;

    public final static int USER_PHOTO_WIDTH = 136;
    public final static int USER_PHOTO_SHOW_DIP = 65;

    private static BroadcastReceiver broadcastReceiver;

    public static String large_str;

    public static ArrayList<Friend> friend_list = null;

    public static ArrayList<XmppMessage> message_list;

    public interface OnHandleMessageListener {
        public void onHandleMessage(String name, String data);
    }

    private static OnHandleMessageListener listener;

    public static void init(Context con, OnHandleMessageListener listener) {
        context = con;

        initXmpp();

        server_name = context.getString(R.string.server_name);
        server_ip = context.getString(R.string.server_ip);
        server_altport = Integer.parseInt(context
                .getString(R.string.server_altport));

        Log.d("server_altport", server_altport + "");

        message_list = new ArrayList<XmppMessage>();

        new Thread() {
            public void run() {
                default_photo = BitmapUtils.getNewBitmap(getDefaultPhoto(),
                        DisplayUtils.dip2px(context, USER_PHOTO_SHOW_DIP));
                user_photo = default_photo;
            }
        }.start();

        Xmpp.listener = listener;
        broadcast = context.getPackageName() + ".broadcast";
        broadcastReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                if (intent.getAction().equals(Xmpp.broadcast)) {

                    String name = intent.getStringExtra("name");
                    String data = intent.getStringExtra("data");
                    if (Xmpp.listener != null)
                        Xmpp.listener.onHandleMessage(name, data);
                }
            }
        };
        context.registerReceiver(broadcastReceiver, new IntentFilter(broadcast));
    }

    public static Bitmap getDefaultPhoto() {

        // 得到Resources对象
        Resources r = context.getResources();
        // 以数据流的方式读取资源
        InputStream is = r.openRawResource(R.drawable.default_photo);
        BitmapDrawable bmpDraw = new BitmapDrawable(is);
        Bitmap bmp = bmpDraw.getBitmap();

        return bmp;

    }

    public static void unregisterReceiver() {
        context.unregisterReceiver(broadcastReceiver);
        broadcastReceiver = null;
    }

    public static boolean handleVCardId(String data) {
        try {
            JSONObject json = new JSONObject(data);

            String friend_name = json.getString("user_name");
            String nick_name = json.getString("nick_name");
            String photo = json.getString("photo_data");
            if (friend_name.equals(Xmpp.user_name)) {
                Xmpp.user_nick_name = nick_name;
                if (!TextUtils.isEmpty(photo)) {
                    Bitmap bitmap = Cache.getImageFromStr(photo);
                    if (bitmap != null) {
                        Xmpp.user_photo = BitmapUtils.getNewBitmap(bitmap,
                                DisplayUtils.dip2px(context,
                                        USER_PHOTO_SHOW_DIP));
                    }

                }

            } else {
                if (Xmpp.friend_list == null)
                    return false;

                for (Friend friend : Xmpp.friend_list) {
                    if (friend_name.equals(friend.name)) {// friend.jid.equals(friend_name)){
                        if (!TextUtils.isEmpty(nick_name))
                            friend.setNickname(nick_name);

                        if (!TextUtils.isEmpty(photo)) {
                            Bitmap bitmap = Cache.getImageFromStr(photo);
                            if (bitmap != null) {
                                friend.setPhoto(BitmapUtils.getNewBitmap(
                                        bitmap, DisplayUtils.dip2px(context,
                                                USER_PHOTO_SHOW_DIP)));
                            }
                        }
                    }
                }

                return true;

            }

        } catch (JSONException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return false;
    }

    public static void handleMessageList() {
        if (message_list.isEmpty())
            return;
        if (Xmpp.listener == null)
            return;
        for (XmppMessage message : message_list) {
            Xmpp.listener.onHandleMessage(message.name, message.data);
        }

        message_list.clear();
    }

    public static String sendBroadcast(int name_id, String data) {

        Log.d("jni", "sendBroadcast:" + data);
        Log.d("jni", "sendBroadcast" + name_id);

        Intent intent = new Intent();
        intent.setAction(broadcast);
        intent.putExtra("name", broadcast_action[name_id]);

        if (name_id != ON_RECEIVE_PICTURE_ID) {// &&
            // name_id != ON_GET_VCARD_ID){
            intent.putExtra("data", data);
        } else {
            large_str = data;
        }

        context.sendBroadcast(intent);

        return null;
    }

}