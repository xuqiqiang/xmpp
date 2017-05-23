package com.xuqiqiang.xmpp.game;

import org.json.JSONException;
import org.json.JSONObject;

import android.graphics.Color;
import android.graphics.Paint.Style;
import android.view.MotionEvent;

import com.snailstdio.software.xsdk.logger.Logger;
import com.xuqiqiang.xmpp.Xmpp;

public class Gobang {

    private int size_x;
    private int size_y;

    private int[][] map;

    private int status;
    private final int STATUS_START = 0, STATUS_WAITING = 1;

    private int time_player1;
    private int time_player2;

    public Gobang() {
        size_x = 10;
        size_y = 16;

        map = new int[size_y][size_x];
        for (int i = 0; i < size_y; i++)
            for (int j = 0; j < size_x; j++)
                map[i][j] = -1;

        time_player1 = 300000;
        time_player2 = 300000;

        status = PD.role;
    }

    public void show() {

        showBg();
        showPiece();
        showTime();
        showRound();
    }

    private void showBg() {

        PD.paint.setStyle(Style.STROKE);
        PD.paint.setStrokeWidth(2);
        PD.paint.setColor(Color.BLACK);

        double start_x = PD.W * 10;
        double space_x = PD.W * 80 / (size_x - 1);
        double start_y = PD.H * 10;
        double space_y = PD.H * 80 / (size_y - 1);
        int i;
        for (i = 0; i < size_y; i++) {
            drawLine(start_x, start_y + space_y * i, start_x + PD.W * 80,
                    start_y + space_y * i);// »­Ïß
        }
        for (i = 0; i < size_x; i++) {
            drawLine(start_x + space_x * i, start_y, start_x + space_x * i,
                    start_y + PD.H * 80);// »­Ïß
        }
    }

    private void showPiece() {

        double start_x = PD.W * 10;
        double space_x = PD.W * 80 / (size_x - 1);
        double start_y = PD.H * 10;
        double space_y = PD.H * 80 / (size_y - 1);

        PD.paint.setStyle(Style.FILL);

        for (int i = 0; i < size_y; i++)
            for (int j = 0; j < size_x; j++) {
                if (map[i][j] != -1) {

                    int color = Color.BLACK;
                    if (map[i][j] == 1)
                        color = Color.WHITE;

                    drawCircle(start_x + space_x * j, start_y + space_y * i,
                            PD.H * 2, color);
                }
            }

    }

    private void showTime() {
        PD.paint.setColor(Color.BLACK);
        PD.paint.setTextSize((float) (PD.H * 3));
        PD.canvas.drawText("ºÚ:" + time_player1 / 1000, (float) (PD.W * 10),
                (float) (PD.H * 6), PD.paint);
        PD.canvas.drawText("°×:" + time_player2 / 1000, (float) (PD.W * 70),
                (float) (PD.H * 6), PD.paint);
    }

    private void showRound() {
        int color = Color.BLACK;

        if (PD.role == 0) {
            if (status == STATUS_START)
                color = Color.BLACK;
            else if (status == STATUS_WAITING)
                color = Color.WHITE;
        } else if (PD.role == 1) {
            if (status == STATUS_START)
                color = Color.WHITE;
            else if (status == STATUS_WAITING)
                color = Color.BLACK;
        }

        drawCircle(PD.W * 50, PD.H * 5, PD.H * 2, color);
    }

    private void drawLine(double x1, double y1, double x2, double y2) {
        PD.canvas.drawLine((float) x1, (float) y1, (float) x2, (float) y2,
                PD.paint);// »­Ïß
    }

    private void drawCircle(double x, double y, double radius, int color) {
        PD.paint.setColor(color);
        PD.canvas.drawCircle((float) x, (float) y, (float) radius, PD.paint);
    }

    public void onTouch(MotionEvent event) {

        if (status != STATUS_START) {
            PD.showMessage("»¹Ã»ÂÖµ½Äã");
            return;
        }
        double X = event.getX();
        double Y = event.getY();

        double space_x = PD.W * 80 / (size_x - 1);
        double space_y = PD.H * 80 / (size_y - 1);
        double start_x = PD.W * 10 - space_x / 2;
        double start_y = PD.H * 10 - space_y / 2;

        int index_j = (int) ((X - start_x) / space_x);
        int index_i = (int) ((Y - start_y) / space_y);

        if (index_i >= 0 && index_i < size_y && index_j >= 0
                && index_j < size_x) {
            if (map[index_i][index_j] == -1) {
                map[index_i][index_j] = PD.role;
                check(index_i, index_j);
                sendMessage(index_i, index_j);
                status = STATUS_WAITING;
                if (PD.situation == PD.SITUATION_WIN) {
                    PD.showPrompt("¹§Ï²ÄãÓ®ÁË");
                }
            }
        }
    }

    private void check(int index_i, int index_j) {
        int i, j, k;
        boolean find;
        for (i = index_i - 4; i <= index_i; i++) {
            if (i >= 0 && i + 5 <= size_y) {
                find = false;
                for (k = 0; k < 5; k++)
                    if (map[i + k][index_j] != PD.role) {
                        find = true;
                        break;
                    }

                if (find == false) {
                    PD.situation = PD.SITUATION_WIN;
                    return;
                }
            }
        }

        for (j = index_j - 4; j <= index_j; j++) {
            if (j >= 0 && j + 5 <= size_x) {
                find = false;

                for (k = 0; k < 5; k++)
                    if (map[index_i][j + k] != PD.role) {
                        find = true;
                        break;
                    }

                if (find == false) {
                    PD.situation = PD.SITUATION_WIN;
                    return;
                }
            }
        }

        for (i = index_i - 4, j = index_j - 4; i <= index_i; i++, j++) {
            if (i >= 0 && i + 5 <= size_y && j >= 0 && j + 5 <= size_x) {
                find = false;

                for (k = 0; k < 5; k++)
                    if (map[i + k][j + k] != PD.role) {
                        find = true;
                        break;
                    }

                if (find == false) {
                    PD.situation = PD.SITUATION_WIN;
                    return;
                }
            }
        }

        for (i = index_i + 4, j = index_j - 4; i >= index_i; i--, j++) {
            if (i - 4 >= 0 && i < size_y && j >= 0 && j + 5 <= size_x) {
                find = false;
                for (k = 0; k < 5; k++)
                    if (map[i - k][j + k] != PD.role) {
                        find = true;

                        Logger.d("check map[" + (i - k) + "][" + (j + k) + "]="
                                + map[i - k][j + k]);

                        break;
                    }

                if (find == false) {
                    PD.situation = PD.SITUATION_WIN;
                    return;
                }
            }
        }
    }

    public void sendMessage(int i, int j) {

        try {
            JSONObject json = new JSONObject();

            json.put("i", i);
            json.put("j", j);

            if (PD.role == 0) {
                json.put("time_player1", time_player1);
                json.put("time_player2", time_player2);
            }

            if (PD.situation == PD.SITUATION_WIN) {
                json.put("situation", 1);
            } else {
                json.put("situation", 0);
            }

            Xmpp.sendGameMessage(PD.friend, json.toString());
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void onReceiveMessage(String message) {

        try {

            JSONObject json = new JSONObject(message);
            int i = json.getInt("i");
            int j = json.getInt("j");
            if (i < 0 || j < 0) {
                PD.promptEnd();
                return;
            }
            map[i][j] = 1 - PD.role;

            if (PD.role == 1) {
                time_player1 = json.getInt("time_player1");
                time_player2 = json.getInt("time_player2");
            }

            int situation = json.getInt("situation");
            if (situation == 1) {
                PD.situation = PD.SITUATION_LOSE;
                PD.showPrompt("ÄãÊäÁË");
            }

            status = STATUS_START;

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void logic() {

        if (PD.role == 0) {
            if (status == STATUS_START)
                time_player1 -= PD.delay;
            else if (status == STATUS_WAITING)
                time_player2 -= PD.delay;
        } else if (PD.role == 1) {
            if (status == STATUS_START)
                time_player2 -= PD.delay;
            else if (status == STATUS_WAITING)
                time_player1 -= PD.delay;
        }

        if (PD.role == 0) {
            if (time_player1 <= 0) {

            } else if (time_player2 <= 0) {

            }
        }
    }

}