package com.xuqiqiang.xmpp.view;

import java.util.ArrayList;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.snailstdio.software.xsdk.utils.BitmapUtils;
import com.xuqiqiang.xmpp.R;
import com.xuqiqiang.xmpp.Xmpp;
import com.xuqiqiang.xmpp.data.Friend;

public class FriendsListAdapter extends BaseAdapter {
    private Context context;

    public FriendsListAdapter(Context context, ArrayList<Friend> friend_list) {
        this.context = context;
    }

    @Override
    public int getCount() {
        return Xmpp.friend_list.size();
    }

    @Override
    public Object getItem(int arg0) {
        return arg0;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        LayoutInflater mLayoutInflater = LayoutInflater.from(context);
        if (convertView == null) {
            convertView = mLayoutInflater.inflate(R.layout.friends_list_item,
                    parent, false);
        }
        ImageView iv_photo = (ImageView) convertView.findViewById(R.id.photo);
        TextView tv_name = (TextView) convertView.findViewById(R.id.name);
        TextView tv_status = (TextView) convertView.findViewById(R.id.status);

        if (Xmpp.friend_list.get(position).photo == null)
            iv_photo.setImageBitmap(Xmpp.default_photo);
        else
            iv_photo.setImageBitmap(Xmpp.friend_list.get(position).photo);

        String name = Xmpp.friend_list.get(position).name;
        if (Xmpp.friend_list.get(position).new_message > 0)
            name += "     ÐÂÏûÏ¢:" + Xmpp.friend_list.get(position).new_message;

        tv_name.setText(name);
        tv_status.setText("[" + Xmpp.friend_list.get(position).status + "]");

        return convertView;
    }

}