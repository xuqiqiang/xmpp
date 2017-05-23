package com.xuqiqiang.xmpp.game;

public class MyCircle {
    public double x, y, radius;
    public int color;

    public MyCircle(double x, double y, double radius, int color) {
        this.x = x;
        this.y = y;
        this.radius = radius;
        this.color = color;
    }

    public void show() {
        PD.paint.setColor(color);
        PD.canvas.drawCircle((float) (PD.W * x), (float) (PD.H * y),
                (float) (PD.H * radius), PD.paint);
    }
}