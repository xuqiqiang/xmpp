package com.xuqiqiang.xmpp.activity;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.TimeZone;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import com.snailstdio.software.xsdk.logger.Logger;
import com.snailstdio.software.xsdk.utils.BitmapUtils;
import com.snailstdio.software.xsdk.utils.Cache;
import com.snailstdio.software.xsdk.utils.DisplayUtils;
import com.snailstdio.software.xsdk.utils.FileUtils;
import com.snailstdio.software.xsdk.utils.IntentUtils;
import com.snailstdio.software.xsdk.utils.ScreenInfo;
import com.snailstdio.software.xsdk.view.CustomEditText;
import com.snailstdio.software.xsdk.view.PopMenu;
import com.snailstdio.software.xsdk.view.PopMenu.OnPopMenuItemClickListener;
import com.xuqiqiang.xmpp.R;
import com.xuqiqiang.xmpp.Xmpp;
import com.xuqiqiang.xmpp.data.Data;
import com.xuqiqiang.xmpp.data.MyString;
import com.xuqiqiang.xmpp.util.RequestCode;
import com.xuqiqiang.xmpp.util.Util;
import com.xuqiqiang.xmpp.view.XmppNotification;

public class ChatActivity extends LoadAcitivity {

    private String chat_object_name;

    private boolean receiveMessage;

    private static ChatActivity instance;

    private ArrayList<Data> data;

    private LinearLayout layout;

    private ImageButton imageButton;
    private PopMenu popMenu;

    private View view;

    public static boolean hasInstance() {
        return instance != null;
    }

    public static ChatActivity getInstance() {
        return instance;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        onLoad();

        instance = this;

        Intent intent = getIntent();
        Bundle bundle = intent.getExtras();

        chat_object_name = bundle.getString("chat_object");

        setTitle(chat_object_name);

        LayoutInflater mInflater = getLayoutInflater();
        view = mInflater.inflate(R.layout.chat, null);

        Thread thread = new Thread(new Runnable() {
            public void run() {

                initView();

                readCache();

                sendHandlerMessage(MESSAGE_COMPLETE);

            }
        });
        thread.start();
    }

    @Override
    public void onHandler_MESSAGE_COMPLETE() {
        onCompleted();
        setContentView(view);

        mHandler.post(new Runnable() {
            @Override
            public void run() {
                ((ScrollView) view.findViewById(R.id.scroll))
                        .fullScroll(ScrollView.FOCUS_DOWN);
            }
        });
    }

    @Override
    public void onHandler_MESSAGE_ERROR() {
        showError("发生错误！");
    }

    @Override
    public void onResume() {
        super.onResume();
        XmppNotification.close();
    }

    @Override
    public void onPause() {
        super.onPause();
        XmppNotification.show();
    }

    @Override
    public void onStop() {
        super.onStop();

        Log.v("onStop", "123");

    }

    @Override
    protected void onLoadEvent() {
    }

    private void initView() {

        layout = (LinearLayout) view.findViewById(R.id.layout);

        setTitle(view, chat_object_name);

        view.findViewById(R.id.send_message).setOnClickListener(
                new OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        sendMessage();

                        /*
                         * Thread thread = new Thread(new Runnable() { public
                         * void run() { test(); } }); thread.start();
                         */
                    }
                });

        initPopmenu();

        data = new ArrayList<Data>();

    }

    private void initPopmenu() {

        Button btn_title_back = (Button) view.findViewById(R.id.btn_title_back);
        btn_title_back.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                finish();
            }
        });

        imageButton = (ImageButton) view.findViewById(R.id.btn_title_popmenu);
        imageButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                popMenu.showAsDropDown(v);
            }
        });

        ArrayList<String> popMenu_itemList = new ArrayList<String>();
        popMenu_itemList.add("发送图片");
        popMenu_itemList.add("清空记录");

        ArrayList<Integer> popMenu_itemListDrawable = new ArrayList<Integer>();
        popMenu_itemListDrawable.add(R.drawable.menu_image);
        popMenu_itemListDrawable.add(R.drawable.menu_delete);

        OnPopMenuItemClickListener popMenu_listener = new OnPopMenuItemClickListener() {

            @Override
            public boolean onItemClick(int position) {
                if (position == 0)
                    selectFile();
                else if (position == 1)
                    clearCache();
                return true;
            }
        };

        popMenu = new PopMenu(this, PopMenu.STYLE_BLACK);
        popMenu.setItems(popMenu_itemList);
        popMenu.setItemsDrawable(popMenu_itemListDrawable);
        popMenu.setOnItemClickListener(popMenu_listener);
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Logger.d("onActivityResult requestCode:" + requestCode + ",resultCode:"
                + resultCode);

        if (resultCode == Activity.RESULT_OK) {
            if (RequestCode.REQUEST_GET_LOCAL_IMAGE_FOR_SEND == requestCode) {
                if (data != null) {
                    Bitmap bitmap = BitmapUtils.getBitmapFromUri(this,
                            data.getData());
                    sendPicture(bitmap, getFormatTime() + ".jpg");
                }

            }
        }

    }

    static String getFormatTime() {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd-HH_mm_ss_SSS");
        sdf.setTimeZone(TimeZone.getTimeZone("Asia/Shanghai"));
        return sdf.format(System.currentTimeMillis());
    }

    private void addTextToView(String str, boolean isSelf) {
        TextView tv = new TextView(this); // 动态添加文本框
        tv.setText(str);
        tv.setTextSize(20);
        tv.setPadding((int) DisplayUtils.dip2px(this, 9),
                (int) DisplayUtils.dip2px(this, 5),
                (int) DisplayUtils.dip2px(this, 5),
                (int) DisplayUtils.dip2px(this, 9));

        int pos = Gravity.RIGHT;
        if (isSelf)
            pos = Gravity.LEFT;
        tv.setGravity(pos);
        tv.setTextColor(Color.BLACK);
        layout.addView(tv, new LinearLayout.LayoutParams(
                LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT));
    }

    public void addText(String str, boolean isSelf) { // 添加一行文字

        addTextToView(str, isSelf);

        mHandler.post(new Runnable() {
            @Override
            public void run() {
                ((ScrollView) view.findViewById(R.id.scroll))
                        .fullScroll(ScrollView.FOCUS_DOWN);
            }
        });

        Cache cache = Cache.getInstance();
        cache.initWrite(
                new String[] { Xmpp.user_name,
                        this.getString(R.string.file_chat), chat_object_name },
                "chat_data", true);

        cache.writeString(str);
        cache.SaveWrite();
    }

    private void addBitmapToView(String sender, Bitmap bitmap, final File file,
            boolean isSelf) {
        TextView tv = new TextView(this); // 动态添加文本框
        tv.setText(sender + ":");
        tv.setTextSize(20);
        tv.setPadding((int) DisplayUtils.dip2px(this, 9),
                (int) DisplayUtils.dip2px(this, 5),
                (int) DisplayUtils.dip2px(this, 5),
                (int) DisplayUtils.dip2px(this, 9));
        int pos = Gravity.RIGHT;
        if (isSelf)
            pos = Gravity.LEFT;
        tv.setGravity(pos);
        tv.setTextColor(Color.BLACK);
        layout.addView(tv, new LinearLayout.LayoutParams(
                LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT));

        ImageView iv = new ImageView(this);
        iv.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                IntentUtils.showImage(ChatActivity.this, file);
            }
        });

        Bitmap b = BitmapUtils.getNewBitmap(bitmap, ScreenInfo.getInstance()
                .getWidthPixels() / 2);

        iv.setBackgroundDrawable(new BitmapDrawable(b));

        LinearLayout layout_iv = new LinearLayout(this);
        layout_iv.setGravity(pos);

        layout_iv.setPadding((int) DisplayUtils.dip2px(this, 9),
                (int) DisplayUtils.dip2px(this, 5),
                (int) DisplayUtils.dip2px(this, 5),
                (int) DisplayUtils.dip2px(this, 9));
        layout_iv.addView(iv,
                new LinearLayout.LayoutParams(b.getWidth(), b.getHeight()));

        layout.addView(layout_iv, new LinearLayout.LayoutParams(
                LayoutParams.MATCH_PARENT, b.getHeight()));
    }

    public void addBitmap(String sender_name, String id, String file_name,
            Bitmap bitmap, File file, boolean isSelf) {
        addBitmapToView(sender_name, bitmap, file, isSelf);

        mHandler.post(new Runnable() {
            @Override
            public void run() {
                ((ScrollView) view.findViewById(R.id.scroll))
                        .fullScroll(ScrollView.FOCUS_DOWN);
            }
        });

    }

    private void sendMessage() {
        CustomEditText send_message_content = (CustomEditText) (findViewById(R.id.send_message_content));

        String content = send_message_content.getText().toString();
        if (TextUtils.isEmpty(content)) {
            send_message_content.showPopupPrompt(R.string.chat_input_empty);
            return;
        }

        String id = Xmpp.sendChatMessage(chat_object_name, content);

        Logger.d("id:" + id);

        if (TextUtils.isEmpty(id)) {
            Toast.makeText(ChatActivity.this, "连接已断开", Toast.LENGTH_LONG)
                    .show();
            return;
        }

        Data new_data = new Data(id);
        data.add(new_data);

        addText(Xmpp.user_name + ":" + content, true);

        send_message_content.setText("");
    }

    private void sendPicture(Bitmap bitmap, String file_name) {
        String id = Xmpp.sendPicture(chat_object_name, file_name,
                Cache.bitmapToBase64(bitmap));
        Logger.d("id:" + id);
        file_name = id + "_" + file_name;
        File file = Cache.getFile(new String[] { Xmpp.user_name,
                getString(R.string.file_chat), chat_object_name }, file_name);
        addBitmap(Xmpp.user_name, id, file_name, bitmap, file, true);
        Util.saveBitmap(this, chat_object_name, Xmpp.user_name, file_name,
                bitmap);
    }

    public boolean checkChatObjectName(String name) {
        return chat_object_name.equals(name);
    }

    private void testCrazySend() {

        receiveMessage = true;

        while (true) {

            if (receiveMessage) {
                String id = Xmpp.sendChatMessage(chat_object_name, "abc");
                receiveMessage = false;

                Data new_data = new Data(id);
                data.add(new_data);
            }

            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }

    public void onReceiveMessage(String data) {
        /*
         * EditText chat_content=(EditText)(findViewById(R.id.chat_content));
         * 
         * chat_content.setText(chat_content.getText().toString()+"\n"+ data);
         */

        addText(data, false);
    }

    public void onReceivePicture(String sender_name, String id,
            String file_name, Bitmap bitmap) {
        file_name = id + "_" + file_name;
        File file = Cache.getFile(new String[] { Xmpp.user_name,
                getString(R.string.file_chat), chat_object_name }, file_name);
        addBitmap(sender_name, id, file_name, bitmap, file, false);
        Util.saveBitmap(this, sender_name, sender_name, file_name, bitmap);
    }

    public void onReceiveFeedbackId(String id) {
        for (Data d : data) {
            if (d.id.equals(id)) {

                long time = System.currentTimeMillis();
                Log.v("发送时间", (time - d.time) / 2 + "");

                // aver_time+=(time-d.time)/2;
                // timer++;

                /*
                 * if(timer==300){ flag=!flag;
                 * 
                 * Log.v("发送时间平均时间", aver_time/timer+""); }
                 */

                Toast.makeText(ChatActivity.this, "已送达！", Toast.LENGTH_LONG)
                        .show();
                data.remove(d);
                receiveMessage = true;
                break;
            }
        }
    }

    private boolean readCache() {
        Cache cache = Cache.getInstance();
        if (cache.initRead(
                new String[] { Xmpp.user_name,
                        this.getString(R.string.file_chat), chat_object_name },
                "chat_data")) {
            Logger.d("chat_object_name:" + chat_object_name);
            ArrayList<String> strs = cache.readStrings();
            cache.SaveRead();

            if (strs == null || strs.size() == 0)
                return false;

            for (String str : strs) {
                Logger.d("str:" + str);
                boolean isSelf = str.startsWith(Xmpp.user_name + ":");
                String sender_name = MyString.getSender(str);
                String content = MyString.getContent(str);
                String photo_name = MyString.check_photo(content);
                Logger.d("photo_name:" + photo_name);
                if (TextUtils.isEmpty(photo_name)) {
                    addTextToView(str, isSelf);
                } else {

                    File file = Cache.getFile(new String[] { Xmpp.user_name,
                            getString(R.string.file_chat), chat_object_name },
                            photo_name);
                    Bitmap bitmap = Cache.readBitmap(file);
                    Logger.d("bitmap:" + (bitmap != null));
                    addBitmapToView(sender_name, bitmap, file, isSelf);
                }
            }
            return true;
        }

        cache.SaveRead();
        return false;

    }

    private void clearCache() {

        // EditText chat_content=(EditText)(findViewById(R.id.chat_content));
        // chat_content.setText("");
        layout.removeAllViews();
        FileUtils.deleteCache(
                this,
                new String[] { Xmpp.user_name,
                        this.getString(R.string.file_chat), chat_object_name },
                "chat_data");
    }

    private void selectFile() {
        // Intent intent = new Intent(this,FileList.class);
        // intent.putExtra("Type", FileList.TYPE_IMAGE);
        // startActivityForResult(intent, 0);
        IntentUtils.getLocalImage(this,
                RequestCode.REQUEST_GET_LOCAL_IMAGE_FOR_SEND);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_MENU) {
            if (popMenu != null) {
                popMenu.showAsDropDown(imageButton);
                return true;
            }

        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        instance = null;

    }

}