package com.xuqiqiang.xmpp.activity;

import java.util.ArrayList;

import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.snailstdio.software.xsdk.logger.Logger;
import com.snailstdio.software.xsdk.utils.BitmapUtils;
import com.snailstdio.software.xsdk.utils.Cache;
import com.snailstdio.software.xsdk.utils.IntentUtils;
import com.snailstdio.software.xsdk.view.CustomDialog;
import com.snailstdio.software.xsdk.view.CustomEditText;
import com.snailstdio.software.xsdk.view.PopMenu;
import com.snailstdio.software.xsdk.view.PopMenu.OnPopMenuItemClickListener;
import com.xuqiqiang.xmpp.R;
import com.xuqiqiang.xmpp.Xmpp;
import com.xuqiqiang.xmpp.Xmpp.OnHandleMessageListener;
import com.xuqiqiang.xmpp.data.Friend;
import com.xuqiqiang.xmpp.data.MyString;
import com.xuqiqiang.xmpp.data.XmppMessage;
import com.xuqiqiang.xmpp.game.GameActivity;
import com.xuqiqiang.xmpp.game.PD;
import com.xuqiqiang.xmpp.util.RequestCode;
import com.xuqiqiang.xmpp.util.Util;
import com.xuqiqiang.xmpp.view.FriendsListAdapter;
import com.xuqiqiang.xmpp.view.XmppNotification;

public class XmppUI extends LoadAcitivity implements OnHandleMessageListener {
    private static final String TAG = XmppUI.class.getSimpleName();
    private View login_view;
    private View cloud_view;

    private ListView lv;
    private FriendsListAdapter friends_list_adapter;

    private ImageButton imageButton;
    private PopMenu popMenu;

    private boolean auto_login;

    private boolean auto_login_setting;

    public ProgressDialog progressDialog;

    private static XmppUI instance;

    private static final int PASSWORD_MIN_LENGTH = 6;

    private boolean isExit;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        Util.createInstance(this);
        instance = this;
        Xmpp.init(this, this);
        XmppNotification.init(this, getClass());
        if (!readSetting())
            onInit();
    }

    public void onInit() {
        scene = SCENE_LOGIN;

        initLoginView();

        setContentView(login_view);

    }

    private boolean readSetting() {
        auto_login = Cache.read_Boolean("auto_login", false);
        Logger.d("readSetting : " + auto_login);
        if (auto_login) {
            Xmpp.user_name = Cache.read_String("user_name", "");
            Xmpp.user_password = Cache.read_String("user_password", "");
            if (Xmpp.user_name == "" || Xmpp.user_password == "")
                return false;
            Log.d("user_name", Xmpp.user_name);
            Log.d("user_password", Xmpp.user_password);
            onLoad("正在登录中，请稍后");
            onLoadEvent();
            return true;
        }
        return false;
    }

    private void initLoginView() {
        if (login_view == null) {

            LayoutInflater mInflater = getLayoutInflater();
            login_view = mInflater.inflate(R.layout.login, null);

            setTitle(login_view, "登录云端");

            login_view.findViewById(R.id.login_btn_login).setOnClickListener(
                    new OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            EditText et_username = (EditText) (findViewById(R.id.username));
                            EditText et_password = (EditText) (findViewById(R.id.password));
                            String username = et_username.getText().toString();
                            String password = et_password.getText().toString();
                            if (TextUtils.isEmpty(username)
                                    || TextUtils.isEmpty(password)) {

                                new CustomDialog.Builder(XmppUI.this)
                                        .setTitle("登录")
                                        .setMessage("用户名或密码输入为空!")
                                        .setPositiveButton(R.string.ok, null)
                                        .create().show();

                                return;
                            }
                            Xmpp.user_name = username;
                            Xmpp.user_password = password;
                            auto_login_setting = ((CheckBox) findViewById(R.id.login_cb_auto_login))
                                    .isChecked();
                            onLoad("正在登录中，请稍后");
                            onLoadEvent();

                        }
                    });

            login_view.findViewById(R.id.tv_newuser).setOnClickListener(
                    new OnClickListener() {
                        @Override
                        public void onClick(View v) {

                            registerDialog();
                        }
                    });

            login_view.findViewById(R.id.tv_setting).setOnClickListener(
                    new OnClickListener() {
                        @Override
                        public void onClick(View v) {

                            loginSetting();
                        }
                    });

        }
        initLoginPopmenu();
    }

    private void initLoginPopmenu() {
        imageButton = (ImageButton) login_view
                .findViewById(R.id.btn_title_popmenu);
        imageButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                popMenu.showAsDropDown(v);
            }
        });

        ArrayList<String> popMenu_itemList = new ArrayList<String>();
        popMenu_itemList.add("设置");
        popMenu_itemList.add("注册");
        popMenu_itemList.add("关于");

        ArrayList<Integer> popMenu_itemListDrawable = new ArrayList<Integer>();
        popMenu_itemListDrawable.add(R.drawable.menu_settings);
        popMenu_itemListDrawable.add(R.drawable.menu_register);
        popMenu_itemListDrawable.add(R.drawable.menu_about);

        OnPopMenuItemClickListener popMenu_listener = new OnPopMenuItemClickListener() {

            @Override
            public boolean onItemClick(int position) {
                if (position == 0)
                    loginSetting();
                else if (position == 1)
                    registerDialog();
                else if (position == 2) {
                    aboutDialog();
                }
                return true;
            }
        };

        popMenu = new PopMenu(this, PopMenu.STYLE_BLACK);
        popMenu.setItems(popMenu_itemList);
        popMenu.setItemsDrawable(popMenu_itemListDrawable);
        popMenu.setOnItemClickListener(popMenu_listener);
    }

    @Override
    public void onLoadEvent() {

        setTitle("登录中");

        Xmpp.connectXmpp(Xmpp.user_name, Xmpp.user_password, Xmpp.server_name,
                Xmpp.server_ip, Xmpp.server_altport);

        friends_list_adapter = null;

    }

    private void onLogin() {

        Log.v(TAG, "onLogin");
        if (!auto_login) {
            auto_login = auto_login_setting;
            Cache.write_Boolean("auto_login", auto_login);
            if (auto_login) {
                Cache.write_String("user_name", Xmpp.user_name);
                Cache.write_String("user_password", Xmpp.user_password);
                Log.d("user_name", Xmpp.user_name);
                Log.d("user_password", Xmpp.user_password);
            }

        }

        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {

                try {

                    Xmpp.getVCard(null);
                } catch (Exception e) {

                    e.printStackTrace();
                }
            }
        }, 300);

        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                try {

                    setTitle("获取好友列表");
                    Xmpp.getFriendsList();
                } catch (Exception e) {

                    e.printStackTrace();
                }
            }
        }, 500);

    }

    private void onGetFriendsList(String data) {

        Xmpp.friend_list = new ArrayList<Friend>();

        if (!TextUtils.isEmpty(data)) {
            try {
                JSONObject json = new JSONObject(data);

                int friend_sum = json.getInt("friend_sum");

                for (int i = 0; i < friend_sum; i++) {
                    String friend_name = json.getString("friend_" + (i + 1));
                    Xmpp.friend_list.add(new Friend(friend_name));
                }

            } catch (JSONException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

        }

        for (Friend friend : Xmpp.friend_list) {
            Xmpp.getVCard(friend.name);
        }

        initCloudView();

        Xmpp.handleMessageList();
    }

    private void initCloudView() {

        Log.v("initCloudView", "initCloudView");
        onCompleted();

        if (cloud_view == null) {
            LayoutInflater mInflater = getLayoutInflater();
            cloud_view = mInflater.inflate(R.layout.cloud, null);

            lv = (ListView) cloud_view.findViewById(R.id.listview);

            lv.setOnItemClickListener(new OnItemClickListener() {

                @Override
                public void onItemClick(AdapterView<?> view, View arg1,
                        final int position, long arg3) {

                    DialogInterface.OnClickListener listener = new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog,
                                final int which) {
                            if (which == 0) {
                                Intent intent = new Intent(XmppUI.this,
                                        ChatActivity.class);
                                intent.putExtra("chat_object",
                                        Xmpp.friend_list.get(position).name);
                                startActivity(intent);

                                if (Xmpp.friend_list.get(position).new_message > 0)
                                    Xmpp.friend_list.get(position).new_message = 0;
                                if (friends_list_adapter != null)
                                    friends_list_adapter.notifyDataSetChanged();
                            }
                            if (which == 1) {
                                Xmpp.requestGame(Xmpp.friend_list.get(position).name);
                                Toast.makeText(XmppUI.this, "已发送请求",
                                        Toast.LENGTH_LONG).show();
                            }

                            dialog.cancel();
                        }
                    };

                    CustomDialog.Builder customBuilder = new CustomDialog.Builder(
                            XmppUI.this);
                    customBuilder.setTitle("交互")
                            .setItems(new String[] { "聊天", "五子棋" }, listener)
                            .setPositiveButton(null, null)
                            .setNegativeButton(null, null).create().show();
                }

            });

            lv.setOnItemLongClickListener(new OnItemLongClickListener() {
                @Override
                public boolean onItemLongClick(AdapterView<?> arg0, View arg1,
                        final int position, long id) {

                    DialogInterface.OnClickListener listener = new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog,
                                final int which) {
                            if (which == 0) {

                            }
                            if (which == 1) {

                                new CustomDialog.Builder(XmppUI.this)
                                        .setTitle("删除好友")
                                        .setMessage(
                                                "确认要删除好友"
                                                        + Xmpp.friend_list
                                                                .get(position).name
                                                        + "吗？")
                                        .setPositiveButton(
                                                getString(R.string.ok),
                                                new DialogInterface.OnClickListener() {
                                                    public void onClick(
                                                            DialogInterface dialog,
                                                            int id) {
                                                        deleteFriend(Xmpp.friend_list
                                                                .get(position).name);
                                                        dialog.cancel();
                                                    }
                                                })
                                        .setNegativeButton(
                                                getString(R.string.cancel),
                                                null).create().show();

                            }

                            dialog.cancel();
                        }
                    };

                    CustomDialog.Builder customBuilder = new CustomDialog.Builder(
                            XmppUI.this);
                    customBuilder
                            .setTitle("好友操作")
                            .setItems(new String[] { "TA的信息", "删除TA" },
                                    listener).setPositiveButton(null, null)
                            .setNegativeButton(null, null).create().show();

                    return true;
                }
            });

        }

        setTitle(cloud_view, "好友列表");

        initCloudPopmenu();

        friends_list_adapter = new FriendsListAdapter(this, Xmpp.friend_list);
        lv.setAdapter(friends_list_adapter);

        if (Xmpp.friend_list.size() == 0)
            cloud_view.findViewById(R.id.info_empty)
                    .setVisibility(View.VISIBLE);
        else
            cloud_view.findViewById(R.id.info_empty).setVisibility(View.GONE);

        setContentView(cloud_view);

        Xmpp.handleMessageList();
    }

    private void initCloudPopmenu() {
        imageButton = (ImageButton) cloud_view
                .findViewById(R.id.btn_title_popmenu);
        imageButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                popMenu.showAsDropDown(v);
            }
        });

        ArrayList<String> popMenu_itemList = new ArrayList<String>();
        popMenu_itemList.add("刷新列表");
        popMenu_itemList.add("更改头像");
        popMenu_itemList.add("添加好友");
        popMenu_itemList.add("删除好友");
        popMenu_itemList.add("注销");
        popMenu_itemList.add("退出");

        ArrayList<Integer> popMenu_itemListDrawable = new ArrayList<Integer>();
        popMenu_itemListDrawable.add(R.drawable.menu_refresh);
        popMenu_itemListDrawable.add(R.drawable.menu_image);
        popMenu_itemListDrawable.add(R.drawable.menu_find);
        popMenu_itemListDrawable.add(R.drawable.menu_delete);
        popMenu_itemListDrawable.add(R.drawable.menu_logout);
        popMenu_itemListDrawable.add(R.drawable.menu_return);

        OnPopMenuItemClickListener popMenu_listener = new OnPopMenuItemClickListener() {

            @Override
            public boolean onItemClick(int position) {
                if (position == 0)
                    refresh_friends_list();
                else if (position == 1)
                    setPhoto();
                else if (position == 2)
                    addFriendsDialog();
                else if (position == 3)
                    deleteFriendsDialog();
                else if (position == 4)
                    loginout();
                else if (position == 5) {
                    isExit = true;
                    finish();
                }
                return true;
            }
        };

        popMenu = new PopMenu(this, PopMenu.STYLE_BLACK);
        popMenu.setItems(popMenu_itemList);
        popMenu.setItemsDrawable(popMenu_itemListDrawable);
        popMenu.setOnItemClickListener(popMenu_listener);
    }

    private void onGetFriendNewMessage(String friend_name) {

        if (Xmpp.friend_list == null)
            return;
        for (Friend friend : Xmpp.friend_list) {
            if (friend_name.equals(friend.name)) {
                friend.new_message++;
            }
        }

        if (friends_list_adapter != null)
            friends_list_adapter.notifyDataSetChanged();
    }

    private void onGetFriendsStatus(String data) {

        if (Xmpp.friend_list == null)
            return;
        String friend_name;
        String friend_status = "";

        int i;

        for (i = 1; i < data.length(); i++) {
            if (data.charAt(i) == '|') {
                break;
            }
        }

        friend_name = data.substring(0, i);

        if (i < data.length() - 1) {
            friend_status = data.substring(i + 1, data.length());

            Log.v("friend_name", friend_name);
            Log.v("friend_status", friend_status);
        }

        for (Friend friend : Xmpp.friend_list) {
            if (friend_name.equals(friend.name)) {// friend.jid.equals(friend_name)){
                friend.setStatus(friend_status);
            }
        }

        if (friends_list_adapter != null)
            friends_list_adapter.notifyDataSetChanged();
    }

    private void refreshView() {
        if (friends_list_adapter != null)
            friends_list_adapter.notifyDataSetChanged();
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        Logger.d("onActivityResult requestCode:" + requestCode + ",resultCode:"
                + resultCode);

        if (resultCode == Activity.RESULT_OK) {
            if (RequestCode.REQUEST_GET_LOCAL_IMAGE_FOR_VCARD == requestCode) {
                if (data != null) {
                    Bitmap bitmap = BitmapUtils.getBitmapFromUri(this,
                            data.getData(), Xmpp.USER_PHOTO_WIDTH);
                    Xmpp.setVCard(Xmpp.user_nick_name,
                            Cache.bitmapToBase64(bitmap));
                }

            }
        }
    }

    private void loginSetting() {

        if (scene != SCENE_LOGIN)
            return;

        CustomDialog.Builder builder = new CustomDialog.Builder(this);

        builder.setTitle("登录设置");

        LinearLayout view = (LinearLayout) getLayoutInflater().inflate(
                R.layout.login_setting_dialog, null);
        builder.setContentView(view);

        final CustomEditText et_server_name = (CustomEditText) view
                .findViewById(R.id.et_server_name);
        final CheckBox cb_set_server_ip = (CheckBox) view
                .findViewById(R.id.cb_set_server_ip);
        final CustomEditText et_server_ip = (CustomEditText) view
                .findViewById(R.id.et_server_ip);
        final CustomEditText et_server_altport = (CustomEditText) view
                .findViewById(R.id.et_server_altport);
        et_server_name.setText(getString(R.string.server_name));
        et_server_ip.setText(getString(R.string.server_ip));
        et_server_altport.setText(getString(R.string.server_altport));
        cb_set_server_ip
                .setOnCheckedChangeListener(new OnCheckedChangeListener() {
                    @Override
                    public void onCheckedChanged(CompoundButton buttonView,
                            boolean isChecked) {
                        et_server_ip.setEnabled(isChecked);
                        et_server_altport.setEnabled(isChecked);
                    }
                });

        builder.setPositiveButton(R.string.ok,
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {

                        String server_name = et_server_name.getText()
                                .toString();
                        String server_ip = et_server_ip.getText().toString();
                        String server_altport = et_server_altport.getText()
                                .toString();

                        if (TextUtils.isEmpty(server_name)) {
                            et_server_ip.dismissPopupPrompt();
                            et_server_altport.dismissPopupPrompt();
                            et_server_name
                                    .showPopupPrompt(R.string.login_input_server_name_empty);
                            return;
                        }

                        Xmpp.server_name = server_name;
                        if (cb_set_server_ip.isChecked()) {

                            if (TextUtils.isEmpty(server_ip)) {
                                et_server_name.dismissPopupPrompt();
                                et_server_altport.dismissPopupPrompt();
                                et_server_ip
                                        .showPopupPrompt(R.string.login_input_server_ip_empty);
                                return;
                            }

                            if (TextUtils.isEmpty(server_altport)) {
                                et_server_name.dismissPopupPrompt();
                                et_server_ip.dismissPopupPrompt();
                                et_server_altport
                                        .showPopupPrompt(R.string.login_input_server_port_empty);
                                return;
                            }

                            Xmpp.server_ip = server_ip;
                            Xmpp.server_altport = 5222;
                            try {
                                Xmpp.server_altport = Integer
                                        .parseInt(server_altport);
                            } catch (NumberFormatException e) {
                                e.printStackTrace();
                            }
                        } else {
                            Xmpp.server_ip = "";
                            Xmpp.server_altport = 5222;
                        }

                        Logger.d("Xmpp.server_name:" + Xmpp.server_name);
                        Logger.d("Xmpp.server_ip:" + Xmpp.server_ip);
                        Logger.d("Xmpp.server_altport:" + Xmpp.server_altport);

                        dialog.cancel();

                    }
                });
        builder.setNegativeButton(R.string.cancel, null);
        builder.create().show();
    }

    private void registerDialog() {

        if (scene != SCENE_LOGIN)
            return;

        CustomDialog.Builder builder = new CustomDialog.Builder(this);

        builder.setTitle(R.string.login_input_register);

        LinearLayout fragment = (LinearLayout) getLayoutInflater().inflate(
                R.layout.register_dialog, null);
        builder.setContentView(fragment);

        final CustomEditText et_username = (CustomEditText) fragment
                .findViewById(R.id.username);
        final CustomEditText et_password_1 = (CustomEditText) fragment
                .findViewById(R.id.password_1);
        final CustomEditText et_password_2 = (CustomEditText) fragment
                .findViewById(R.id.password_2);

        builder.setPositiveButton(R.string.ok,
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        String username = et_username.getText().toString();
                        final String password_1 = et_password_1.getText()
                                .toString();
                        String password_2 = et_password_2.getText().toString();

                        if (TextUtils.isEmpty(username)) {
                            et_password_1.dismissPopupPrompt();
                            et_password_2.dismissPopupPrompt();
                            et_username
                                    .showPopupPrompt(R.string.login_input_username_empty);
                            return;
                        }

                        if (TextUtils.isEmpty(password_1)) {
                            et_username.dismissPopupPrompt();
                            et_password_2.dismissPopupPrompt();
                            et_password_1
                                    .showPopupPrompt(R.string.login_input_password_empty);
                            return;
                        }

                        if (TextUtils.isEmpty(password_2)) {
                            et_username.dismissPopupPrompt();
                            et_password_1.dismissPopupPrompt();
                            et_password_2
                                    .showPopupPrompt(R.string.login_input_password_empty);
                            return;
                        }

                        if (!TextUtils.equals(password_1, password_2)) {
                            et_username.dismissPopupPrompt();
                            et_password_1.dismissPopupPrompt();
                            et_password_2
                                    .showPopupPrompt(R.string.login_input_password_not_equal);
                            return;
                        }

                        if (password_1.length() < PASSWORD_MIN_LENGTH) {
                            et_username.dismissPopupPrompt();
                            et_password_2.dismissPopupPrompt();
                            et_password_1.showPopupPrompt(getString(
                                    R.string.login_input_password_too_short,
                                    PASSWORD_MIN_LENGTH));
                            return;
                        }

                        progressDialog = ProgressDialog.show(XmppUI.this, null,
                                "正在注册...", true, true);

                        Xmpp.register(username + "@" + Xmpp.server_name,
                                password_1, Xmpp.server_ip, Xmpp.server_altport);
                        dialog.cancel();

                    }
                });
        builder.setNegativeButton(R.string.cancel, null);
        builder.create().show();

    }

    private void aboutDialog() {
        String message = "本软件是基于XMPP通信协议的Android客户端，"
                + "底层通信模块基于开源C库Libstrophe，" + "可实现注册、登录、聊天、发送图片、添加好友、删除好友"
                + "以及进行五子棋对战游戏等功能。\n" + "Copyright(c)2014，xuqiqiang。\n"
                + "email:1239749058@qq.com";
        CustomDialog.Builder customBuilder = new CustomDialog.Builder(
                XmppUI.this);
        customBuilder
                .setTitle("关于xmpp")
                .setMessage(message)
                .setPositiveButton(getString(R.string.ok), null)
                .setNegativeButton("我的博客",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                Uri uri = Uri
                                        .parse("https://xuqiqiang.github.io/");
                                Intent it = new Intent(Intent.ACTION_VIEW, uri);
                                startActivity(it);
                                dialog.cancel();
                            }
                        }).create().show();
    }

    private void addFriendsDialog() {

        if (scene != SCENE_CONMPLETE)
            return;

        LinearLayout loginForm = (LinearLayout) getLayoutInflater().inflate(
                R.layout.input_dialog, null);

        final CustomEditText editText = (CustomEditText) loginForm
                .findViewById(R.id.et_input);

        new CustomDialog.Builder(this)
                .setTitle("添加好友")
                .setContentView(loginForm)
                .setPositiveButton(R.string.ok,
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog,
                                    int which) {

                                String input = editText.getText().toString()
                                        .trim();
                                if (TextUtils.isEmpty(input)) {
                                    editText.showPopupPrompt(R.string.login_input_username_empty);
                                    return;
                                }
                                Xmpp.addFriend(input);

                                Toast.makeText(XmppUI.this, "已发送请求。",
                                        Toast.LENGTH_LONG).show();
                                dialog.cancel();

                            }
                        }).setNegativeButton(R.string.cancel, null).create()
                .show();
    }

    private void deleteFriendsDialog() {

        if (scene != SCENE_CONMPLETE)
            return;

        if (Xmpp.friend_list.size() == 0)
            return;

        final String str_friend_list[] = new String[Xmpp.friend_list.size()];

        for (int i = 0; i < Xmpp.friend_list.size(); i++) {
            str_friend_list[i] = Xmpp.friend_list.get(i).name;
        }

        DialogInterface.OnClickListener listener = new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, final int which) {
                new CustomDialog.Builder(XmppUI.this)
                        .setTitle("删除好友")
                        .setMessage("确认要删除好友" + str_friend_list[which] + "吗？")
                        .setPositiveButton(getString(R.string.ok),
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog,
                                            int id) {
                                        deleteFriend(Xmpp.friend_list
                                                .get(which).name);
                                        dialog.cancel();
                                    }
                                })
                        .setNegativeButton(getString(R.string.cancel), null)
                        .create().show();

                dialog.cancel();
            }
        };

        CustomDialog.Builder customBuilder = new CustomDialog.Builder(this);
        customBuilder.setTitle("删除好友").setItems(str_friend_list, listener)
                .setPositiveButton(null, null).setNegativeButton(null, null)
                .create().show();
    }

    private void deleteFriend(String friend_name) {
        Xmpp.deleteFriend(friend_name);

        onLoad();
        setTitle("正在删除好友");

        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                setTitle("获取好友列表");
                Xmpp.getFriendsList();
            }
        }, 1000);
    }

    private void refresh_friends_list() {
        String str = null;
        str.length();
        if (scene != SCENE_CONMPLETE)
            return;
        onLoad();
        setTitle("获取好友列表");
        Xmpp.getFriendsList();
    }

    private void setPhoto() {
        // Intent intent = new Intent(this, FileList.class);
        // intent.putExtra("Type", FileList.TYPE_IMAGE);
        // startActivityForResult(intent, 0);
        IntentUtils.getLocalImage(this,
                RequestCode.REQUEST_GET_LOCAL_IMAGE_FOR_VCARD);
    }

    private void loginout() {
        Xmpp.stopXmpp();

        onInit();
        ((EditText) login_view.findViewById(R.id.username))
                .setText(Xmpp.user_name);
        ((EditText) login_view.findViewById(R.id.password))
                .setText(Xmpp.user_password);
        auto_login = false;
    }

    @Override
    public void onResume() {
        super.onResume();
        XmppNotification.close();
    }

    @Override
    public void onPause() {
        super.onPause();
        if (scene == SCENE_CONMPLETE && !isExit) {
            XmppNotification.show();
        }
    }

    @Override
    public void onStop() {
        super.onStop();

        Logger.d("onStop");

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        Logger.d("onDestroy");

        if (scene != SCENE_LOGIN)
            Xmpp.stopXmpp();

        Xmpp.unregisterReceiver();

        Xmpp.releaseXmpp();

        XmppNotification.close();

        instance = null;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_MENU) {
            if ((scene == SCENE_LOGIN || scene == SCENE_CONMPLETE)
                    && popMenu != null) {
                popMenu.showAsDropDown(imageButton);
                return true;
            }

        } else if (keyCode == KeyEvent.KEYCODE_BACK) {
            if (scene == SCENE_WAIT) {
                loginout();
                return true;
            } else if (scene == SCENE_CONMPLETE) {

                moveTaskToBack(true);
                return true;
            }

        }

        isExit = true;
        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onHandler_MESSAGE_COMPLETE() {
        // TODO Auto-generated method stub

    }

    @Override
    protected void onHandler_MESSAGE_ERROR() {
        // TODO Auto-generated method stub

    }

    @Override
    public void onHandleMessage(String name, String data) {

        if (instance != null && scene != LoadAcitivity.SCENE_CONMPLETE) {
            if (name.equals(Xmpp.broadcast_action[Xmpp.ON_RECEIVE_MESSAGE_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_RECEIVE_FEEDBACK_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_REQUEST_GAME_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_ACCEPT_GAME_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_RECEIVE_GAME_MESSAGE_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_RECEIVE_PICTURE_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_ADD_FRIEND_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_ADD_FRIEND_RESPOND_AGREE_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_ADD_FRIEND_RESPOND_DISAGREE_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_NEED_SET_VCARD_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_GET_VCARD_ID])
                    || name.equals(Xmpp.broadcast_action[Xmpp.ON_SET_VCARD_SUCCESSFUL_ID])) {

                Xmpp.message_list.add(new XmppMessage(name, data));
                return;
            }
        }

        if (name.equals(Xmpp.broadcast_action[Xmpp.ON_LOGIN_ID])) {
            if (instance != null)
                onLogin();
        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_GET_FRIENDS_LIST_ID])) {
            // String data = intent.getStringExtra("data");
            if (instance != null)
                onGetFriendsList(data);
        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_GET_FRIENDS_STATUS_ID])) {
            // String data = intent.getStringExtra("data");
            if (instance != null)
                onGetFriendsStatus(data);
        }
        if (name.equals(Xmpp.broadcast_action[Xmpp.ON_RECEIVE_MESSAGE_ID])) {
            // String data = intent.getStringExtra("data");

            Log.v("receiveMessage", data);

            String chat_object_jid = MyString.getJidFromMessage(data);

            String chat_object_name = MyString.getNameFromJid(chat_object_jid);

            Log.v("chat_object_jid", chat_object_jid);
            Log.v("chat_object_name", chat_object_name);

            if (ChatActivity.hasInstance()
                    && ChatActivity.getInstance().checkChatObjectName(
                            chat_object_jid)) {
                ChatActivity.getInstance().onReceiveMessage(data);
            } else {
                if (XmppUI.instance != null)
                    XmppUI.instance.onGetFriendNewMessage(chat_object_name);

                Cache cache = Cache.getInstance();
                cache.initWrite(new String[] { Xmpp.user_name,
                        getString(R.string.file_chat), chat_object_name },
                        "chat_data", true);

                cache.writeString(data);
                cache.SaveWrite();
            }

        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_RECEIVE_FEEDBACK_ID])) {
            if (ChatActivity.hasInstance()) {
                ChatActivity.getInstance().onReceiveFeedbackId(data);
            }
        } else if (name.equals(Xmpp.broadcast_action[Xmpp.ON_REQUEST_GAME_ID])) {
            final String friend = data;

            new CustomDialog.Builder(XmppUI.this)
                    .setTitle("游戏请求")
                    .setMessage(
                            MyString.getNameFromJid(friend) + "请求与您进行游戏，是否同意？")
                    .setPositiveButton(R.string.ok,
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                        int id) {
                                    Xmpp.acceptGame(friend);

                                    Intent i = new Intent(XmppUI.this,
                                            GameActivity.class);
                                    i.putExtra("friend", friend);
                                    i.putExtra("role", 1);
                                    startActivity(i);
                                    dialog.cancel();
                                }
                            }).setNegativeButton(R.string.cancel, null)
                    .create().show();

        } else if (name.equals(Xmpp.broadcast_action[Xmpp.ON_ACCEPT_GAME_ID])) {
            String friend = data;
            Intent i = new Intent(this, GameActivity.class);
            i.putExtra("friend", friend);
            i.putExtra("role", 0);
            startActivity(i);

        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_RECEIVE_GAME_MESSAGE_ID])) {
            String message = data;
            if (GameActivity.hasInstance()) {
                if (PD.gm != null && PD.gm.gobang != null) {
                    PD.gm.gobang.onReceiveMessage(message);
                }
            }
        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_XMPP_REGISTER_SUCCESSFUL_ID])) {

            if (instance != null) {
                if (progressDialog != null) {
                    progressDialog.dismiss();
                }
            }
            new CustomDialog.Builder(XmppUI.this).setTitle("注册")
                    .setMessage("注册成功!").setPositiveButton(R.string.ok, null)
                    .create().show();
        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_XMPP_REGISTER_NOT_SUCCESSFUL_ID])) {
            if (instance != null) {
                if (progressDialog != null) {
                    progressDialog.dismiss();
                }
            }
            new CustomDialog.Builder(XmppUI.this).setTitle("注册")
                    .setMessage("注册不成功!").setPositiveButton(R.string.ok, null)
                    .create().show();
        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_XMPP_REGISTER_ALREADY_EXIST_ID])) {
            if (instance != null) {
                if (progressDialog != null) {
                    progressDialog.dismiss();
                }
            }
            new CustomDialog.Builder(XmppUI.this).setTitle("注册")
                    .setMessage("用户已存在!").setPositiveButton(R.string.ok, null)
                    .create().show();
        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_XMPP_REGISTER_NETWORK_CUT_DOWN_ID])) {
            if (instance != null) {
                if (progressDialog != null) {
                    progressDialog.dismiss();
                }
            }
            new CustomDialog.Builder(XmppUI.this).setTitle("注册")
                    .setMessage("网络断开!").setPositiveButton(R.string.ok, null)
                    .create().show();
        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_RECEIVE_PICTURE_ID])) {

            Log.v("ON_RECEIVE_PICTURE_ID", "1");

            try {
                JSONObject json = new JSONObject(Xmpp.large_str);

                String sender_name = json.getString("sender_name");
                String id = json.getString("id");
                String file_name = json.getString("file_name");
                String content = json.getString("content");

                Bitmap bitmap = Cache.getImageFromStr(content);

                if (ChatActivity.hasInstance()
                        && ChatActivity.getInstance().checkChatObjectName(
                                sender_name)) {
                    ChatActivity.getInstance().onReceivePicture(sender_name,
                            id, file_name, bitmap);
                } else {
                    if (instance != null)
                        onGetFriendNewMessage(sender_name);
                    Util.saveBitmap(this, sender_name, sender_name, id + "_"
                            + file_name, bitmap);
                }

            } catch (JSONException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

        } else if (name.equals(Xmpp.broadcast_action[Xmpp.ON_ADD_FRIEND_ID])) {
            final String add_friend_jid = data;// intent.getStringExtra("data");

            new CustomDialog.Builder(XmppUI.this)
                    .setTitle("请求")
                    .setMessage(
                            MyString.getNameFromJid(add_friend_jid)
                                    + "请求添加您为好友，是否同意？")
                    .setPositiveButton("同意并加TA为好友",
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                        int id) {
                                    Xmpp.addFriendRespond(add_friend_jid,
                                            Xmpp.XMPP_ADD_FRIEND_RESPOND_AGREE);

                                    if (instance != null)
                                        refresh_friends_list();
                                    dialog.cancel();
                                }
                            })
                    .setNegativeButton("拒绝",
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                        int id) {
                                    Xmpp.addFriendRespond(
                                            add_friend_jid,
                                            Xmpp.XMPP_ADD_FRIEND_RESPOND_DISAGREE);
                                    dialog.cancel();
                                }
                            }).create().show();
        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_ADD_FRIEND_RESPOND_AGREE_ID])) {
            final String add_friend_jid = data;// intent.getStringExtra("data");

            new CustomDialog.Builder(XmppUI.this)
                    .setTitle("请求")
                    .setMessage(
                            MyString.getNameFromJid(add_friend_jid)
                                    + "同意了您的请求。")
                    .setPositiveButton(R.string.ok,
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                        int id) {
                                    if (XmppUI.instance != null) {
                                        XmppUI.instance.setTitle("获取好友列表");
                                        Xmpp.getFriendsList();
                                    }
                                    dialog.cancel();
                                }
                            }).create().show();
        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_ADD_FRIEND_RESPOND_DISAGREE_ID])) {
            final String add_friend_jid = data;
            new CustomDialog.Builder(XmppUI.this)
                    .setTitle("请求")
                    .setMessage(
                            MyString.getNameFromJid(add_friend_jid)
                                    + "拒绝了您的请求。")
                    .setPositiveButton(R.string.ok, null).create().show();

        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_NEED_SET_VCARD_ID])) {

            LinearLayout loginForm = (LinearLayout) getLayoutInflater()
                    .inflate(R.layout.input_dialog, null);
            TextView prompt = (TextView) loginForm.findViewById(R.id.prompt);
            prompt.setText("请输入昵称");
            final CustomEditText editText = (CustomEditText) loginForm
                    .findViewById(R.id.et_input);

            new CustomDialog.Builder(this)
                    .setTitle("设置你的名片")
                    .setContentView(loginForm)
                    .setPositiveButton(R.string.ok,
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                        int which) {

                                    String input = editText.getText()
                                            .toString().trim();

                                    if (TextUtils.isEmpty(input)) {
                                        editText.showPopupPrompt(R.string.login_input_nickname_empty);
                                        return;
                                    }

                                    Xmpp.setVCard(input, null);
                                    dialog.cancel();

                                }
                            }).setNegativeButton(R.string.cancel, null)
                    .create().show();

        } else if (name.equals(Xmpp.broadcast_action[Xmpp.ON_GET_VCARD_ID])) {
            Log.v("ON_GET_VCARD_ID", data);

            if (Xmpp.handleVCardId(data)) {
                if (instance != null)
                    refreshView();
            }

        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_SET_VCARD_SUCCESSFUL_ID])) {
            Toast.makeText(this, "已设置个人信息。", Toast.LENGTH_LONG).show();
            Xmpp.getVCard(null);
        } else if (name.equals(Xmpp.broadcast_action[Xmpp.ON_AUTH_ERROR_ID])) {
            if (instance != null) {
                if (scene == LoadAcitivity.SCENE_WAIT) {
                    onInit();
                    auto_login = false;
                    Toast.makeText(this, "用户或密码不正确！", Toast.LENGTH_LONG).show();
                }
            }
        } else if (name
                .equals(Xmpp.broadcast_action[Xmpp.ON_NETWORK_DISCONNECT_ID])) {
            if (instance != null) {
                if (scene == LoadAcitivity.SCENE_WAIT) {
                    showError("无法连接到服务器");
                }
            }
        }
    }

}