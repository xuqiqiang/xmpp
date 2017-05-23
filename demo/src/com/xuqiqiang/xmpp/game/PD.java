package com.xuqiqiang.xmpp.game;

import android.app.Activity;
import android.content.DialogInterface;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnTouchListener;

import com.snailstdio.software.xsdk.logger.Logger;
import com.snailstdio.software.xsdk.view.CustomDialog;
import com.snailstdio.software.xsdk.view.ToastMaster;
import com.xuqiqiang.xmpp.R;
import com.xuqiqiang.xmpp.data.MyString;

public class PD {

    public static Activity context;
    public static double W, H;
    public static int screen_width, screen_height;

    public static int select_round;

    public static int process;

    public static final int PROCESS_MAINMENU = 1, PROCESS_SELECT = 2,
            PROCESS_START = 3, PROCESS_CONTINUE = 4, PROCESS_SETTING = 5,
            PROCESS_HELP = 6, PROCESS_LOADING = 7;

    public static int situation, presituation;

    public static final int SITUATION_WAITREADY = 0, SITUATION_STARTING = 1,
            SITUATION_PAUSE = 2, SITUATION_WIN = 3, SITUATION_LOSE = 4,
            SITUATION_CLEAN = 5, SITUATION_ACHIEVE = 6, SITUATION_BACK = 7;

    public static final int ButtonType1 = 0, ButtonType2 = 1, ButtonType3 = 2,
            ButtonType_HelpBarSwitch = 3;

    public static Paint paint;
    public static Canvas canvas;
    public static SurfaceHolder sfh;

    public static GameManager gm = null;

    public static double lengthX;
    public static double lengthW;
    public static double lengthH;

    public static int delay = 30;

    public static SurfaceView gameView;

    public static int role;
    public static String friend;

    public static void init(GameActivity ctx, int my_role, String friend_jid) {
        context = ctx;
        role = my_role;
        friend = friend_jid;
    }

    public static void startGame() {
        process = PROCESS_START;

        gameView = new SurfaceView(context);
        gameView.setKeepScreenOn(true);
        gameView.setFocusable(true);
        paint = new Paint();
        paint.setAntiAlias(true);

        sfh = gameView.getHolder();
        sfh.addCallback(new Callback() {

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format,
                    int width, int height) {
            }

            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Logger.d("surfaceCreated");

                if (gm == null) {
                    screen_width = gameView.getWidth();
                    screen_height = gameView.getHeight();
                    W = screen_width / 100f;
                    H = screen_height / 100f;

                    lengthH = H * 10;
                    lengthW = W * 6.25;
                    lengthX = lengthH;
                    gm = new GameManager(select_round);
                }

                gm.start();
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                Logger.d("surfaceDestroyed");
                if (gm != null)
                    gm.pause();
            }

        });

        gameView.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View arg0, MotionEvent event) {
                return gm.onTouchEvent(event);
            }
        });

        context.setContentView(gameView);

        situation = SITUATION_STARTING;
    }

    public static void resumeGame() {
        context.setContentView(gameView);
    }

    public static void stopGame() {
        if (situation == SITUATION_STARTING) {
            gm.gobang.sendMessage(-1, -1);
        }
        gm.pause();
        gm = null;
    }

    public static void showMessage(final String message) {
        context.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                ToastMaster.showToast(context, message);
            }
        });
    }

    public static void showPrompt(final String message) {
        context.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                new CustomDialog.Builder(context).setTitle("提示")
                        .setMessage(message)
                        .setPositiveButton(R.string.ok, null).create().show();
            }
        });
    }

    public static void promptEnd() {
        context.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                new CustomDialog.Builder(context)
                        .setTitle("游戏结束")
                        .setMessage(MyString.getNameFromJid(friend) + "已经退出游戏")
                        .setPositiveButton(R.string.ok,
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog,
                                            int id) {
                                        context.finish();
                                        dialog.cancel();
                                    }
                                }).create().show();
            }
        });
    }

    public static void setsituation(int s) {
        presituation = situation;
        situation = s;
    }
}