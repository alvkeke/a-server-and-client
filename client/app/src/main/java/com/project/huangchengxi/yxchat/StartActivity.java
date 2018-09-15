package com.project.huangchengxi.yxchat;

import android.content.Intent;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

public class StartActivity extends AppCompatActivity implements Runnable{

    @Override
    public void run() {
        //暂停1秒
        try{
            Thread.sleep(1000);
        }catch(Exception e){
            e.printStackTrace();
        }
        //跳转至主活动
        Intent intent = new Intent(StartActivity.this,MainActivity.class);
        startActivity(intent);
        finish();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_start);
        ActionBar actionBar=getSupportActionBar();
        actionBar.hide();
        Thread thread=new Thread(this);
        thread.start();
    }
}
