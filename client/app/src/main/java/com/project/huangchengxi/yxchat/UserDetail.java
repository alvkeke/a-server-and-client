package com.project.huangchengxi.yxchat;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.TextView;

public class UserDetail extends AppCompatActivity {
    private ImageView imageView;
    private TextView id;
    private TextView ip;
    private TextView nat;
    private TextView port;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_user_detail);

        imageView=(ImageView)findViewById(R.id.image_view_user_info);
        id=(TextView)findViewById(R.id.user_id);
        ip=(TextView)findViewById(R.id.ip);
        nat=(TextView)findViewById(R.id.nat);
        port=(TextView)findViewById(R.id.port);

        Intent intent=getIntent();

        id.setText("用户ID:"+intent.getStringExtra("UserId"));
        ip.setText("用户IP地址:"+intent.getStringExtra("UserIP"));
        nat.setText("用户NAT:"+intent.getIntExtra("NAT",0));
        port.setText("用户端口:"+intent.getIntExtra("PORT",0));
    }
}
