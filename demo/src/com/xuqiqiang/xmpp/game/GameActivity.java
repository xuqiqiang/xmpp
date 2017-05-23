package com.xuqiqiang.xmpp.game;

import android.app.Activity;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Window;

import com.snailstdio.software.xsdk.view.CustomDialog;
import com.xuqiqiang.xmpp.R;

public class GameActivity extends Activity {
    private static GameActivity instance = null;

    public static boolean hasInstance() {
        return instance != null;
    }

    public static GameActivity getInstance() {
        return instance;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        instance = this;
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        Intent intent = getIntent();
        Bundle bundle = intent.getExtras();

        String friend = bundle.getString("friend");
        int role = bundle.getInt("role");

        PD.init(this, role, friend);
        PD.startGame();
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onDestroy() {
        instance = null;
        PD.stopGame();
        super.onDestroy();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_BACK) {
            new CustomDialog.Builder(this)
                    .setTitle("退出游戏")
                    .setMessage("确认要退出游戏吗？")
                    .setPositiveButton(getString(R.string.ok),
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                        int id) {
                                    dialog.cancel();
                                    finish();
                                }
                            })
                    .setNegativeButton(getString(R.string.cancel), null)
                    .create().show();

        }
        return super.onKeyDown(keyCode, event);
    }
}