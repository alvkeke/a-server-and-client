package com.project.huangchengxi.yxchat;

public class UserItem {
    private String UserId;
    private String IP;
    private int nat;
    private int port;
    public UserItem(String UserId,String IP,int nat,int port){
        this.UserId=UserId;
        this.IP=IP;
        this.nat=nat;
        this.port=port;
    }

    public String getUserId() {
        return UserId;
    }

    public int getNat() {
        return nat;
    }

    public int getPort() {
        return port;
    }

    public String getIP() {
        return IP;
    }
}
