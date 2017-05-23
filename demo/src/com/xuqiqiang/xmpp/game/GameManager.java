package com.xuqiqiang.xmpp.game;

import org.json.JSONException;
import org.json.JSONObject;

import android.graphics.Color;
import android.view.MotionEvent;

public class GameManager {

    public int round;

    public int gamespeed;

    public int time;

    private Thread gameThread;
    private GameRunnable gameRunnable;

    public Gobang gobang;

    public GameManager(int round) {
        this.round = round;
        gamespeed = 0;
        time = 0;
        initObject();
    }

    public void initObject() {
        gobang = new Gobang();
    }

    public void start() {
        gameRunnable = new GameRunnable();
        gameRunnable.flag = true;
        gameThread = new Thread(gameRunnable);
        gameThread.start();
    }

    public void pause() {
        gameRunnable.flag = false;
    }

    /**
     * ”Œœ∑ªÊÕº
     */
    public void myDraw() {
        try {
            PD.canvas = PD.sfh.lockCanvas();

            PD.canvas.drawColor(0xfffbcbd3);

            // if (PD.situation == PD.SITUATION_WIN) {
            // drawwin();
            // } else if (PD.situation == PD.SITUATION_LOSE) {
            // drawlose();
            // }
            // else
            DrawGame();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (PD.canvas != null)
                PD.sfh.unlockCanvasAndPost(PD.canvas);
        }
    }

    private void DrawGame() {
        gobang.show();
    }

    private void drawwin() {
        PD.paint.setColor(Color.BLACK);
        PD.paint.setTextSize((float) (PD.H * 6));
        PD.canvas.drawText("ƒ„”Æ¡À!", (float) (PD.W * 40), (float) (PD.H * 40),
                PD.paint);
    }

    private void drawlose() {
        PD.paint.setColor(Color.BLACK);
        PD.paint.setTextSize((float) (PD.H * 6));
        PD.canvas.drawText("ƒ„ ‰¡À!", (float) (PD.W * 40), (float) (PD.H * 40),
                PD.paint);
    }

    /**
     * ”Œœ∑¬ﬂº≠
     */
    private void logic() {
        if (PD.situation == PD.SITUATION_STARTING) {
            gobang.logic();
        }
    }

    class GameRunnable implements Runnable {
        public boolean flag;

        @Override
        public void run() {
            while (flag) {
                long start = System.currentTimeMillis();
                myDraw();
                logic();

                time += PD.delay;
                long end = System.currentTimeMillis();
                try {
                    if (end - start < PD.delay) {
                        Thread.sleep(PD.delay - (end - start));
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public boolean onTouchEvent(MotionEvent event) {
        if (PD.situation == PD.SITUATION_STARTING
                && event.getAction() == MotionEvent.ACTION_DOWN) {
            gobang.onTouch(event);
        }
        return true;
    }
}